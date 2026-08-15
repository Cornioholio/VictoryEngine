#include "DX12Renderer.h"
bool VictoryRenderer::Initialize(HWND hwnd, int width, int height)
{
	Logger::Info("This is an info message.");
	Logger::Warning("This is a warning message.");
	Logger::Error("This is an error message.");
	Logger::Debug("This is a debug message.");
	
	// Clean out array memory to remove any native C++ stack junk values
	for (UINT n = 0; n < frameCount_; n++)
	{
		fenceValues_[n] = 0;
	}

	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG) 
	// Enable the D3D12 debug layer if in debug mode
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) 
	{
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	// Create DXGI factory
	if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory_)))) 
	{
		return false;
	}

	// Enumerate hardware adapters 
	Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
	Microsoft::WRL::ComPtr<IDXGIFactory6> factory;

	// Check for high perforamnce GPU first
	if (SUCCEEDED(factory_.As(&factory))) 
	{
		factory->EnumAdapterByGpuPreference(
			0, 
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, 
			IID_PPV_ARGS(&hardwareAdapter));
	}
	// Then fallback to default adapter
	if (hardwareAdapter == nullptr) 
	{
		for (UINT adapterIndex = 0; SUCCEEDED(factory_->EnumAdapters1(adapterIndex, &hardwareAdapter)); adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc;
			hardwareAdapter->GetDesc1(&desc);

			// Skip software adapters
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

			// Check if adapter supports DX12, 11 minimum but 12 preferred
			if (SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) 
			{
				break;
			}
		}
	}

	// Create actual D3D12 device 
	if (FAILED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device_))))
	{
		return false; // DX12 level not found
	}
	
	// Create command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(device_->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue_))))
	{
		return false;
	}

	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(nullptr, exePath, MAX_PATH);
	std::filesystem::path vertexShaderPath = std::filesystem::path(exePath).parent_path() / L"Shaders" / L"VertexShader.hlsl";
	std::filesystem::path pixelShaderPath = std::filesystem::path(exePath).parent_path() / L"Shaders" / L"PixelShader.hlsl";

	// Compile shaders
	if (!CompileShader(vertexShaderPath.c_str(), L"main", L"vs_6_0", vertexShader_))
	{
		OutputDebugStringW(L"FAILED TO LOAD VERTEX SHADER\n");
		return false;
	}
	if (!CompileShader(pixelShaderPath.c_str(), L"main", L"ps_6_0", pixelShader_))
	{
		OutputDebugStringW(L"FAILED TO LOAD SHADER SHADER\n");
		return false;
	}

	// Create empty root signature
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// Serialize root sig
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	if (FAILED(D3D12SerializeRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSig,
		&errorBlob)))
	{
		return false;
	}

	if (FAILED(device_->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_))))
	{
		return false;
	}

	// Create swap chain and describe windows properties
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = frameCount_;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Standard 32-bit color format
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Modern swap effect for better performance
	swapChainDesc.SampleDesc.Count = 1; // DX12 handles multi-sampling via separate textures, keep at 1

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	HRESULT hr = factory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1);

	if (FAILED(hr))
	{
		wchar_t buffer[256];

		swprintf_s(
			buffer,
			L"[VictoryEngine] CreateSwapChainForHwnd failed. HRESULT: 0x%08X\n",
			static_cast<unsigned int>(hr));

		OutputDebugStringW(buffer);

		return false;
	}


	// DEBUG
	HWND swapChainHwnd = nullptr;

	if (FAILED(swapChain1->GetHwnd(&swapChainHwnd)))
	{
		OutputDebugStringW(
			L"[VictoryEngine] Failed to get swap chain HWND.\n");

		return false;
	}

	if (swapChainHwnd != hwnd)
	{
		OutputDebugStringW(
			L"[VictoryEngine] ERROR: Swap chain HWND does not match viewport HWND.\n");

		return false;
	}

	OutputDebugStringW(
		L"[VictoryEngine] Swap chain HWND matches viewport HWND.\n");
	// DEBUG


	// Upgrade generic swap chain to IDXGISwapChain3 for better features and control
	if (FAILED(swapChain1.As(&swapChain_))) 
	{
		return false;
	}

	// Capture starting back buffer index
	frameIndex_ = swapChain_->GetCurrentBackBufferIndex();

	// Create descriptor heaps 
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = frameCount_; // One handle for each backbuffer
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(device_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap_)))) 
	{
		return false;
	}

	// Get exact memory address size for an rtv handle on this specific gpu device
	UINT rtvDescriptorSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Get handle pointing to the very beginning of our allocated memory block
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();

	// Create an RTV handle pointing to each backbuffer resource instance
	for (UINT n = 0; n < frameCount_; n++) 
	{
		if (FAILED(swapChain_->GetBuffer(n, IID_PPV_ARGS(&renderTargets_[n])))) 
		{
			return false;
		}

		// This links the back buffer memory allocation to our descriptor arraay handle index
		device_->CreateRenderTargetView(renderTargets_[n].Get(), nullptr, rtvHandle);

		// Step forward in memory to position the next handle allocation 
		rtvHandle.ptr += rtvDescriptorSize;
	}

	// Create direct graphics command list to record rendering sequences
	// Pass commandAllocators[0] as initial scratchpad, 0 initially
	for (UINT n = 0; n < frameCount_; n++) 
	{
		if(FAILED(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators_[n]))))
		{
			return false;
		}
	}

	if (FAILED(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators_[0].Get(), nullptr, IID_PPV_ARGS(&commandList_))))
	{
		return false;
	}
	// Command lsits are born in a recording state, close safely since we aren't rendering currently
	commandList_->Close();

	// Create the timeline synchronisation fence
	if (FAILED(device_->CreateFence(fenceValues_[frameIndex_], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_))))
	{
		return false;
	}

	// Increment the counter tracking where this frame's fence value should eventially hit
	fenceValues_[frameIndex_]++;

	// Create an OS event handle to let the CPU thread sleeep until GPU alerts us
	fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if(fenceEvent_ == nullptr) 
	{
		return false;
	}

	width_ = width;
	height_ = height;

	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
	viewport_.Width = static_cast<float>(width);
	viewport_.Height = static_cast<float>(height);
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = width;
	scissorRect_.bottom = height;

	// Stutter safety, force the GPU to fully process everything we just loaded
	FlushGPU();

	return true;
}

void VictoryRenderer::Resize(int width, int height) 
{
	if (width <= 0 || height <= 0) 
	{
		return;
	}
	if(width == width_ && height == height_) 
	{
		return;
	}

	FlushGPU();

	for (UINT i = 0; i < frameCount_; i++) 
	{
		renderTargets_[i].Reset();
	}

	HRESULT hr = swapChain_->ResizeBuffers(
		frameCount_,
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0);

	if(FAILED(hr)) 
	{
		return;
	}

	CreateRenderTargetViews();

	width_ = width;
	height_ = height;

	// Update viewport.
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
	viewport_.Width = static_cast<float>(width);
	viewport_.Height = static_cast<float>(height);
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	// Update scissor rectangle.
	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = width;
	scissorRect_.bottom = height;

	// The swap chain may now have a different current buffer.
	frameIndex_ = swapChain_->GetCurrentBackBufferIndex();
}

void VictoryRenderer::CreateRenderTargetViews() 
{
	UINT rtvDescriptorSize =
		device_->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
		rtvHeap_->GetCPUDescriptorHandleForHeapStart();

	for (UINT n = 0; n < frameCount_; n++)
	{
		if (FAILED(
			swapChain_->GetBuffer(
				n,
				IID_PPV_ARGS(&renderTargets_[n]))))
		{
			return;
		}

		device_->CreateRenderTargetView(
			renderTargets_[n].Get(),
			nullptr,
			rtvHandle);

		rtvHandle.ptr += rtvDescriptorSize;
	}
}
void VictoryRenderer::Shutdown() 
{
	if (device_ != nullptr)
	{
		FlushGPU();
	}

	if (fenceEvent_ != nullptr)
	{
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}

	fence_.Reset();
	commandList_.Reset();
	for (UINT n = 0; n < frameCount_; n++)
	{
		commandAllocators_[n].Reset();
		renderTargets_[n].Reset();
	}
	rtvHeap_.Reset();
	swapChain_.Reset();
	commandQueue_.Reset();
	device_.Reset();
	factory_.Reset();
}

void VictoryRenderer::RenderFrame(const float* clearColor)
{
	commandAllocators_[frameIndex_]->Reset();

	commandList_->Reset(
		commandAllocators_[frameIndex_].Get(),
		nullptr);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = renderTargets_[frameIndex_].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList_->ResourceBarrier(1, &barrier);

	UINT rtvDescriptorSize =
		device_->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
		rtvHeap_->GetCPUDescriptorHandleForHeapStart();

	rtvHandle.ptr += frameIndex_ * rtvDescriptorSize;

	commandList_->ClearRenderTargetView(
		rtvHandle,
		clearColor,
		0,
		nullptr);

	barrier.Transition.StateBefore =
		D3D12_RESOURCE_STATE_RENDER_TARGET;

	barrier.Transition.StateAfter =
		D3D12_RESOURCE_STATE_PRESENT;

	commandList_->ResourceBarrier(1, &barrier);


	if (SUCCEEDED(commandList_->Close()))
	{
		ID3D12CommandList* ppCommandLists[] =
		{
			commandList_.Get()
		};

		commandQueue_->ExecuteCommandLists(
			_countof(ppCommandLists),
			ppCommandLists);

	}

	swapChain_->Present(1, 0);

	MoveToNextFrame();
}
// Helpers

void VictoryRenderer::FlushGPU()
{
	fenceValue_++;

	HRESULT hr = commandQueue_->Signal(
		fence_.Get(),
		fenceValue_);

	if (FAILED(hr))
	{
		return;
	}

	if (fence_->GetCompletedValue() < fenceValue_)
	{

		hr = fence_->SetEventOnCompletion(
			fenceValue_,
			fenceEvent_);

		if (SUCCEEDED(hr))
		{
			WaitForSingleObject(
				fenceEvent_,
				INFINITE);
		}
	}

}

void VictoryRenderer::MoveToNextFrame() 
{
	const UINT64 currentFenceValue = fenceValues_[frameIndex_];

	if (SUCCEEDED(commandQueue_->Signal(fence_.Get(), currentFenceValue)))
	{
		frameIndex_ = swapChain_->GetCurrentBackBufferIndex();

		if (fence_->GetCompletedValue() < fenceValues_[frameIndex_]) 
		{
			if (SUCCEEDED(fence_->SetEventOnCompletion(fenceValues_[frameIndex_], fenceEvent_))) 
			{
				WaitForSingleObject(fenceEvent_, INFINITE);
			}
		}

		fenceValues_[frameIndex_] = currentFenceValue + 1;
	}
}

bool VictoryRenderer::CompileShader(const wchar_t* fileName, const wchar_t* entryPoint, const wchar_t* target, Microsoft::WRL::ComPtr<IDxcBlob>& shaderBlob)
{
	IDxcUtils* utils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> compiler;
	IDxcIncludeHandler* includeHandler;

	if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)))
		|| FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)))
		|| FAILED(utils->CreateDefaultIncludeHandler(&includeHandler)))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;


	if (FAILED(utils->LoadFile(fileName, nullptr, &sourceBlob)))
	{
		OutputDebugStringW(L"FAILED TO OPEN FILE: ");
		OutputDebugStringW(fileName);
		OutputDebugStringW(L"\n");
		return false;
	}

	DxcBuffer sourceBuffer = {};

	sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
	sourceBuffer.Size = sourceBlob->GetBufferSize();
	sourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR args[] =
	{
		fileName,
		L"-E", entryPoint,
		L"-T", target,
#ifdef _DEBUG
		L"-Zi",
		L"-Qembed_debug"
#endif
	};

	Microsoft::WRL::ComPtr<IDxcResult> result;

	if (FAILED(compiler->Compile(
		&sourceBuffer,
		args,
		_countof(args),
		includeHandler,
		IID_PPV_ARGS(&result))))
	{
		return false;
	}

	HRESULT hr;
	result->GetStatus(&hr);

	if (FAILED(hr)) 
	{
		Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors;

		if (SUCCEEDED(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr))) 
		{
			OutputDebugStringA(errors->GetStringPointer());
		}

		return false;
	}

	return SUCCEEDED(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr));
}
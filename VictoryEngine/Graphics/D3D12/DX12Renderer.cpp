#include "DX12Renderer.h"
bool VictoryRenderer::Initialize(HWND hwnd, int width, int height)
{
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
	if (FAILED(factory_->CreateSwapChainForHwnd(
		commandQueue_.Get(), // Swap chain needs the queue to handle presentations
		hwnd,
		&swapChainDesc,
		nullptr, // No fullscreen desc, we will handle that separately if needed
		nullptr, // No output restriction, allow it to choose the best output
		&swapChain1)))
	{
		return false;
	}

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
	return true;
}
void VictoryRenderer::Shutdown() 
{
	rtvHeap_.Reset();
	for (UINT n = 0; n < frameCount_; n++)
	{
		renderTargets_[n].Reset();
	}
	swapChain_.Reset();
	commandQueue_.Reset();
	device_.Reset();
	factory_.Reset();
}
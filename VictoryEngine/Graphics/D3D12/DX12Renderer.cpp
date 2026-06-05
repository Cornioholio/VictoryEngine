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
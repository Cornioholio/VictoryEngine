#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
// ^ Link essential DirectX 12 libraries

#ifdef ENGINE_BUILD_DLL
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

class ENGINE_API VictoryRenderer
{
public:
	bool Initialize(HWND hwnd, int width, int height); // Initializes the DirectX 12 renderer with the given window handle and dimensions
	void Shutdown(); // Shuts down the renderer and releases resources

private:
	static const UINT frameCount_ = 2; // Number of frames in the swap chain (double buffering)

	// Pipeline objects
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory_; // Factory for creating DXGI objects
	Microsoft::WRL::ComPtr<ID3D12Device> device_; // DXGI device interface
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_; // Command queue for submitting rendering commands
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain_; // Swap chain for presenting rendered frames
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_; // Descriptor heap for render target views
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets_[frameCount_]; // Render target resources for each frame

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_; // Fence for synchronizing CPU and GPU
	UINT64 fenceValue_ = 0; // Current value of the fence for synchronization
	HANDLE fenceEvent_ = nullptr; // Event handle for waiting on the fence
	UINT frameIndex_ = 0; // Current frame index in the swap chain

};


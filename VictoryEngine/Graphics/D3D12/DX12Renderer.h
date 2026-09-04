#pragma once
#include "Core/Tools/Logger.h"
#include "Core/Common/CoreMinimal.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxcapi.h>

#include <filesystem>

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
// ^ Link essential DirectX 12 libraries, move these to vcxproj config later on

struct Vertex 
{
	// X, Y, Z
	float position[3];
	// R, G, B, A
	float color[4];
};

class VICTORY_API VictoryRenderer
{
public:
	VictoryRenderer() = default;

	VictoryRenderer(const VictoryRenderer&) = delete; // Prevent copying
	VictoryRenderer& operator=(const VictoryRenderer&) = delete; // Prevent assignment

	bool Initialize(HWND hwnd, int width, int height); // Initializes the DirectX 12 renderer with the given window handle and dimensions
	void Shutdown(); // Shuts down the renderer and releases resources
	void Resize(int width, int height); // Resizes the swap chain and render targets to the new dimensions
	void CreateRenderTargetViews(); // Creates render target views for the swap chain buffers
	void RenderFrame(const float* clearColor);

private:
	// Viewport h & w
	int width_ = 0;
	int height_ = 0;

	D3D12_VIEWPORT viewport_ = {};
	D3D12_RECT scissorRect_ = {};

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
	UINT64 fenceValues_[frameCount_] = { 0 };
	HANDLE fenceEvent_ = nullptr; // Event handle for waiting on the fence
	UINT frameIndex_ = 0; // Current frame index in the swap chain
	UINT rtvDescriptorSize_ = 0; // Size of a single render target view descriptor

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators_[frameCount_];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	// Shader compilation and storage
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};

	bool CompileShader(const wchar_t* filePath,
		const wchar_t* entryPoint,
		const wchar_t* target,
		Microsoft::WRL::ComPtr<IDxcBlob>& shaderBlob);
	void UpdateViewport(int width, int height);
	void MoveToNextFrame();
	void FlushGPU();

};


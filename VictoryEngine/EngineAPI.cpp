#include "EngineAPI.h"
#include "Graphics/D3D12/DX12Renderer.h"
#include <thread>
#include <mutex>
#include <atomic>

namespace
{
	struct EngineState
	{
		VictoryRenderer* renderer = nullptr;

		std::thread renderThread;
		std::atomic<bool> isRunning = false;

		int pendingWidth = 0;
		int pendingHeight = 0;
		bool resizeRequested = false;

		std::mutex resizeMutex;
	};

	EngineState engineState;
}
bool VictoryEngine_Initialize(HWND hwnd, int width, int height) 
{
	if(engineState.renderer != nullptr)
	{
		return true; // Already initialized
	}

	engineState.renderer = new VictoryRenderer();

	if(!engineState.renderer->Initialize(hwnd, width, height))
	{
		delete engineState.renderer;
		engineState.renderer = nullptr;

		return false; // Initialization failed
	}
	
	engineState.isRunning = true;
	engineState.renderThread = std::thread([]()
	{
		const float clearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

		while(engineState.isRunning)
		{
			int width = 0;
			int height = 0;
			bool shouldResize = false;
			{
				std::lock_guard<std::mutex> lock(engineState.resizeMutex);
				if(engineState.resizeRequested)
				{
					width = engineState.pendingWidth;
					height = engineState.pendingHeight;

					engineState.resizeRequested = false;
					shouldResize = true;
				}
			}
			if(shouldResize)
			{
				engineState.renderer->Resize(width, height);
			}
			engineState.renderer->RenderFrame(clearColor); // Render with no clear color, just present the frame
		}
	});

	return true;
}

void VictoryEngine_RequestResize(int width, int height) 
{
	if(width <= 0 || height <= 0)
	{
		return; // Invalid dimensions
	}

	std::lock_guard<std::mutex> lock(engineState.resizeMutex);

	engineState.pendingWidth = width;
	engineState.pendingHeight = height;
	engineState.resizeRequested = true;
}

void VictoryEngine_Shutdown() 
{
	if(engineState.renderer == nullptr)
	{
		return; // Renderer not initialized
	}

	engineState.isRunning = false;
	if(engineState.renderThread.joinable()) 
	{
		engineState.renderThread.join();
	}

	engineState.renderer->Shutdown();

	delete engineState.renderer;
	engineState.renderer = nullptr;
}

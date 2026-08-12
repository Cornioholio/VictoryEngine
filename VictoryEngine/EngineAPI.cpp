#include "EngineAPI.h"
#include "Graphics/D3D12/DX12Renderer.h"

static VictoryRenderer* renderer = nullptr;

bool VictoryEngine_Test() 
{
	return true;
}

bool VictoryEngine_Initialize(HWND hwnd, int width, int height) 
{
	if(renderer != nullptr) 
	{
		return true; // Already initialized
	}

	renderer = new VictoryRenderer();

	if(!renderer->Initialize(hwnd, width, height)) 
	{
		delete renderer;
		renderer = nullptr;
		return false; // Initialization failed
	}

	return true;
}

void VictoryEngine_Resize(int width, int height) 
{
	if(renderer == nullptr) 
	{
		return;
	}

	renderer->Resize(width, height);
}

void VictoryEngine_RenderClearFrame(float r, float g, float b, float a) 
{
	if(renderer == nullptr) 
	{
		return; // Renderer not initialized
	}

	float clearColor[4] = { r, g, b, a };

	renderer->RenderFrame(clearColor);
}

void VictoryEngine_Shutdown() 
{
	if(renderer == nullptr) 
	{
		return; // Renderer not initialized
	}

	renderer->Shutdown();

	delete renderer;
	renderer = nullptr;
}

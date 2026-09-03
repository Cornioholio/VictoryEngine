# VictoryEngine 0.x

VictoryEngine is a game engine and editor written primarily in C++ and C#, currently developed as a personal engine development project. The native C++ core is responsible for the engine and rendering systems, and a WPF editor providing the primary user environment for development and tooling. The primary focus in this stage of development is building a robust DirectX12 rendering backend and developing core systems required to support modern game development workflows. This project serves as an exploration of engine development and a way to develop a deeper understanding of user experience, graphics programming and workflows in large codebases.


## Current State in Development

Engine components are limited in this stage of development in way of a game engine, but major strides have been taken in development despite one contributor (me!). The editor sports a rudimentary DirectX renderer viewport and an engine logger to write messages of different log levels to a console. And core engine systems including a timer for delta time and total execution time, and a mouse and keyboard input manager. Before the WPF editor was implemented a native hwnd window was used to prove each of these systems worked before the editor began construction.

## Engine Architecture

The C++ engine core holds all engine API functionality and systems. Where the entire engine lifecycle of initialisation, update loop (soon) and shutdown is owned by the engine API. Three main systems exist in the core engine in this stage of development, logger, timer and input. The logger is a static library of functions to push log entries holding a message and level of log (Error, Info, Debug and Warning), these are pushed back to a vector that is accessed by the engine API to interact with the editor for displaying and other uses. The timer implements Window's high resolution performance counter to measure elapsed time, tick calculates the time between updates and provides delta time, while total time provides the total elapsed time since the timer was started or reset. The timer also supports stopping and starting while accounting for time spent paused. The input system processes raw data from keyboard and mouse (gamepad not priority at the minute), and handles this data by calculating relative movements by the mouse and mouse clicks (functionality to be added) and checks for keystates.


## Renderer Architecture

The DirectX12 renderer itself is responsible for communicating with the GPU, and handles device creation by making a DXGI factory to create a D3D12 device. Command submission, where it creates a direct command queue, command allocators and command list for recording and submitting work to the device created by the DXGI factory. The renderer then creates a swap chain against the viewports native HWND window, meaning the renderer is tied to the editors viewport rather than the WPF UI. It then maintains multiple render targets and RTV descriptors for the previously mentioned swap chain's back buffers. Fences are then used to stop the CPU from overwriting resources while the GPU is still using them. HLSL shaders are compiled through DXC during initialisation (more to work on regarding shaders).

## Editor Architecture

The editor is built using C# and WPF, providing the editor's user interface and application layer. Separating the UI layer from the native C++ engine allows the core engine systems to remain independent of the editor's presentation. The editor is responsible solely for tools, panels, logging and user interaction. The dedicated VictoryEngineInterop layer provides an interface between C# frontend and C++ backend, currently handling engine lifecycle, viewport resizing and logging. The viewport host uses HwndHost to provice a native HWND that the DX12 renderer can render to while remaining part of the editor UI. Current editor architecture is designed for expansion, as of now it is just a viewport and renderer but an inspector, asset management and gizmo panels will be added as the project progresses.


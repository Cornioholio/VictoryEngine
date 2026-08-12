using System;
using System.Printing;
using System.Runtime.InteropServices;

namespace WPFApplication 
{
    public static class VictoryEngineInterop
    {
        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool VictoryEngine_Initialize(nint hwnd, int width, int height);

        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void VictoryEngine_RenderClearFrame(float r, float g, float b, float a);

        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void VictoryEngine_Shutdown();

        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void VictoryEngine_Resize(int width, int height);


        public static void Initialize(nint hwnd, int width, int height)
        {
            bool result = VictoryEngine_Initialize(hwnd, width, height);

            if (!result)
            {
                throw new Exception("VictoryEngine failed to initialize.");
            }
        }
        
        public static void Resize(int width, int height)
        {
            VictoryEngine_Resize(width, height);
        }
        public static void RenderClearFrame(float r, float g, float b, float a)
        {
            VictoryEngine_RenderClearFrame(r, g, b, a);
        }
        public static void Shutdown()
        {
            VictoryEngine_Shutdown();
        }
    }
}
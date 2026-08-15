using System;
using System.Runtime.InteropServices;

namespace WPFApplication 
{
    public static class VictoryEngineInterop
    {
        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool VictoryEngine_Initialize(nint hwnd, int width, int height);

        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void VictoryEngine_Shutdown();

        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void VictoryEngine_RequestResize(int width, int height);


        public static void Initialize(nint hwnd, int width, int height)
        {
            bool result = VictoryEngine_Initialize(hwnd, width, height);

            if (!result)
            {
                throw new Exception("VictoryEngine failed to initialize.");
            }
        }
        
        public static void RequestResize(int width, int height)
        {
            VictoryEngine_RequestResize(width, height);
        }
        public static void Shutdown()
        {
            VictoryEngine_Shutdown();
        }
    }
}
using System;
using System.Runtime.InteropServices;

namespace WPFApplication 
{
    public enum LogLevel 
    {
        Info,
        Warning,
        Error,
        Debug
    }
    public class ConsoleLogEntry
    { 
        public LogLevel Level { get; set; }
        public string Message { get; set; }
    }
    public static class VictoryEngineInterop
    {
        // Engine
        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool VictoryEngine_Initialize(nint hwnd, int width, int height);

        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void VictoryEngine_Shutdown();

        // Renderer
        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void VictoryEngine_RequestViewportResize(int width, int height);

        // Logger
        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern nint VictoryEngine_GetLogMessage(int index);

        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int VictoryEngine_GetLogLevel(int index);

        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int VictoryEngine_GetLogCount();


        public static void Initialize(nint hwnd, int width, int height)
        {
            bool result = VictoryEngine_Initialize(hwnd, width, height);

            if (!result)
            {
                throw new Exception("VictoryEngine failed to initialize.");
            }
        }
        
        public static void RequestViewportResize(int width, int height)
        {
            VictoryEngine_RequestViewportResize(width, height);
        }

        public static int GetLogCount()
        {
            return VictoryEngine_GetLogCount();
        }
        public static LogLevel GetLogLevel(int index)
        {
            return (LogLevel)VictoryEngine_GetLogLevel(index);
        }
        
        public static string GetLogMessage(int index)
        {
            nint messagePtr = VictoryEngine_GetLogMessage(index);

            if(messagePtr == nint.Zero)
            {
                return string.Empty; 
            }

            return Marshal.PtrToStringAnsi(messagePtr) ?? string.Empty;
        }

        public static void Shutdown()
        {
            VictoryEngine_Shutdown();
        }
    }
}
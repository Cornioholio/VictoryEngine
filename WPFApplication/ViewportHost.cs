using System;
using System.Windows;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Windows.Media;

namespace WPFApplication
{
    public class ViewportHost : HwndHost 
    {
        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        private static extern nint CreateWindowEx(
            int dwExStyle,
            string lpClassName,
            string lpWindowName,
            int dwStyle,
            int x,
            int y,
            int nWidth,
            int nHeight,
            nint hWndParent,
            nint hMenu,
            nint hInstance,
            nint lpParam);

        [DllImport("user32.dll")]
        private static extern bool DestroyWindow(nint hwnd);

        [DllImport("user32.dll")]
        private static extern bool SetWindowPos(
            nint hWnd,
            nint hWndInsertAfter,
            int X,
            int Y,
            int cx,
            int cy,
            uint uFlags);


        private const int WS_CHILD = 0x40000000;
        private const int WS_VISIBLE = 0x10000000;

        private int currentWidth = 1;
        private int currentHeight = 1;

        private nint handle;
        private void ResizeRenderer()
        {
            if (handle == 0)
            {
                return;
            }

            var dpi = VisualTreeHelper.GetDpi(this);

            int width = Math.Max(
                1,
                (int)Math.Round(ActualWidth * dpi.DpiScaleX));

            int height = Math.Max(
                1,
                (int)Math.Round(ActualHeight * dpi.DpiScaleY));

            if (width == currentWidth && height == currentHeight)
            {
                return;
            }

            currentWidth = width;
            currentHeight = height;

            System.Diagnostics.Debug.WriteLine(
                $"[ViewportHost] Resizing renderer: {width} x {height}");

            VictoryEngineInterop.Resize(width, height);

            System.Diagnostics.Debug.WriteLine(
                "[ViewportHost] Renderer resize returned");
        }
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);

            if (handle == 0)
            {
                return;
            }

            Dispatcher.BeginInvoke(
                System.Windows.Threading.DispatcherPriority.Render,
                new Action(ResizeRenderer));
        }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            nint hwnd = CreateWindowEx(
                0,
                "STATIC",
                "",
                WS_CHILD | WS_VISIBLE,
                0,
                0,
                1,
                1,
                hwndParent.Handle,
                0,
                0,
                0);

            if(hwnd == 0) 
            {
                throw new InvalidOperationException("Failed to create VictoryEngine viewport HWND.");
            }

            handle = hwnd;

            VictoryEngineInterop.Initialize(hwnd, 1, 1);

            return new HandleRef(this, hwnd);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            VictoryEngineInterop.Shutdown();

            DestroyWindow(hwnd.Handle);
        }

    }
}
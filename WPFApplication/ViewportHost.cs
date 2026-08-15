using System;
using System.Windows;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Windows.Media;

namespace WPFApplication
{
    public class ViewportHost : HwndHost 
    {
        private const int WS_CHILD = 0x40000000;
        private const int WS_VISIBLE = 0x10000000;

        private nint handle = nint.Zero;

        private int currentWidth = 1;
        private int currentHeight = 1;

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

        private void RequestViewportResize()
        {
            if (handle == nint.Zero)
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

            if (width == currentWidth &&
                height == currentHeight)
            {
                return;
            }

            currentWidth = width;
            currentHeight = height;

            VictoryEngineInterop.RequestViewportResize(
                width,
                height);
        }
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);

            if (handle == nint.Zero)
            {
                return;
            }

            Dispatcher.BeginInvoke(
                System.Windows.Threading.DispatcherPriority.Render,
                new Action(RequestViewportResize));
        }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            handle = CreateWindowEx(
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

            if(handle == nint.Zero) 
            {
                throw new InvalidOperationException("Failed to create VictoryEngine viewport HWND.");
            }

            VictoryEngineInterop.Initialize(handle, 1, 1);

            return new HandleRef(this, handle);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            handle = nint.Zero;

            DestroyWindow(hwnd.Handle);
        }

    }
}
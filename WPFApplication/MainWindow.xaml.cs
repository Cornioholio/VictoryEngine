using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;

namespace WPFApplication
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport("VictoryEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool VictoryEngine_Test();

        private const double BaseViewportWidth = 1280.0;
        private const double BaseViewportHeight = 720.0;

        public MainWindow()
        {
            InitializeComponent();

            MessageBox.Show("WPF UI thread is alive!");

            bool result = VictoryEngine_Test();

            Title = result ? "VictoryEngine Test Passed" : "VictoryEngine Test Failed";

            //CompositionTarget.Rendering += OnRendering;
        }

        //private void OnRendering(object? sender, EventArgs e) 
       // {
        //    VictoryEngineInterop.RenderClearFrame(0.1f, 0.2f, 0.3f, 1.0f);
        //}

       // protected override void OnClosed(EventArgs e)
       // {
       //     CompositionTarget.Rendering -= OnRendering;

      //      base.OnClosed(e);
      //  }

        private void ViewportArea_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            double availableWidth = ViewportArea.ActualWidth - 16;
            double availableHeight = ViewportArea.ActualHeight - 16;

            if (availableWidth <= 0 || availableHeight <= 0) 
            {
                return;
            }

            double scaleX = availableWidth / BaseViewportWidth;
            double scaleY = availableHeight / BaseViewportHeight;

            // Choose whichever dimension limits us first.
            double scale = Math.Min(scaleX, scaleY);

            // Don't make the viewport larger than 1280x720.
            scale = Math.Min(scale, 1.0);

            Viewport.Width = BaseViewportWidth * scale;
            Viewport.Height = BaseViewportHeight * scale;
        }
    }
}
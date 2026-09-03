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
using System.Collections.ObjectModel;
using System.Threading;

namespace WPFApplication
{
    public partial class MainWindow : Window
    {
        private const double BaseViewportWidth = 1920.0;
        private const double BaseViewportHeight = 1080.0;

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;
            Title = "VictoryEngine 0.x";
        }

        // Logger stuff
        public ObservableCollection<ConsoleLogEntry> LogEntries { get; } = new();
        private void LoadLogEntries() 
        {
            int logCount = VictoryEngineInterop.GetLogCount();

            for(int i = 0; i < logCount; i++)
            {
                LogLevel level = VictoryEngineInterop.GetLogLevel(i);
                string message = VictoryEngineInterop.GetLogMessage(i);

                LogEntries.Add(new ConsoleLogEntry
                {
                    Level = level,
                    Message = message
                });
                
            }
        }

        // Window and DX12 Renderer viewport logic
        private void ViewportArea_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            double availableWidth = ViewportArea.ActualWidth - 16;
            double availableHeight = ViewportArea.ActualHeight - 16;

            if(availableWidth <= 0 || availableHeight <= 0)
            {
                return;
            }

            double scaleX = availableWidth / BaseViewportWidth;
            double scaleY = availableHeight / BaseViewportHeight;

            // Choose whichever dimension limits us first.
            double scale = Math.Min(scaleX, scaleY);

            // Don't make the viewport larger than 1920x1080.
            scale = Math.Min(scale, 1.0);

            Viewport.Width = BaseViewportWidth * scale;
            Viewport.Height = BaseViewportHeight * scale;
        }
        protected override void OnClosed(EventArgs e)
        {
            try
            {
                VictoryEngineInterop.Shutdown();
            }
            finally 
            {
                base.OnClosed(e);
            }
 
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            LoadLogEntries();
        }
    }
}
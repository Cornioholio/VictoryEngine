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


        public MainWindow()
        {
            InitializeComponent();

            bool result = VictoryEngine_Test();

            Title = result ? "VictoryEngine Test Passed" : "VictoryEngine Test Failed";
        }
    }
}
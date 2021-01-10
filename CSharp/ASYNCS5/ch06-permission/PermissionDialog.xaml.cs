using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace FaviconBrowser
{
    /// <summary>
    /// Interaction logic for PermissionDialog.xaml
    /// </summary>
    public partial class PermissionDialog : Window
    {
        public PermissionDialog()
        {
            InitializeComponent();
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            PermissionGranted = true;
            Close();
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            PermissionGranted = false;
            Close();
        }

        public bool PermissionGranted { get; set; }
    }
}

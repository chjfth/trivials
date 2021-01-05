using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace FaviconBrowser
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private static readonly List<string> s_Domains = new List<string>
        {
            //"google.com",
            "www.apple.com",
            "bing.com",
            "oreilly.com",
            //"simple-talk.com",
            "microsoft.com",
            //"facebook.com",
            //"twitter.com",
            //"reddit.com",
            "baidu.com",
            //"bbc.co.uk",
            "www.amazon.com",
        };

        public MainWindow()
        {
            InitializeComponent();

            ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls
                       | SecurityProtocolType.Tls11 // require .NETFX 4.5
                       | SecurityProtocolType.Tls12 // require .NETFX 4.5
                       | SecurityProtocolType.Ssl3;
            // [2021-01-04] Chj:
            // http://www.oreilly.com/favicon.ico will be automatically redirected to
            // https://www.oreilly.com/favicon.ico , which requires Tls11, Tls12 .
        }

        private async void GetButton_OnClick(object sender, RoutedEventArgs e)
        {
            if (await GetUserPermission())
            {
                foreach (string domain in s_Domains)
                {
                    AddAFavicon(domain);
                }
            }
        }

        private async void AddAFavicon(string domain)
        {
            WebClient webClient = new WebClient();
            byte[] bytes = await webClient.DownloadDataTaskAsync("http://" + domain + "/favicon.ico");

            Image imageControl = MakeImageControl(bytes);

            m_WrapPanel.Children.Add(imageControl);
        }

        private Task<bool> GetUserPermission()
        {
            //
            // New from CH 6.3
            //

            // Make a TaskCompletionSource so we can return a puppet Task
            TaskCompletionSource<bool> tcs = new TaskCompletionSource<bool>();

            // Show the dialog
            PermissionDialog dialog = new PermissionDialog();
            dialog.Show();

            // When the user is finished with the dialog, complete the Task using SetResult
            dialog.Closed += delegate { tcs.SetResult(dialog.PermissionGranted); };

            // Return the puppet Task, which isn't completed yet
            return tcs.Task;
        }

        private static Image MakeImageControl(byte[] bytes)
        {
            Image imageControl = new Image();
            BitmapImage bitmapImage = new BitmapImage();
            bitmapImage.BeginInit();
            bitmapImage.StreamSource = new MemoryStream(bytes);
            bitmapImage.EndInit();
            imageControl.Source = bitmapImage;
            imageControl.Width = 16;
            imageControl.Height = 16;
            return imageControl;
        }
    }
}

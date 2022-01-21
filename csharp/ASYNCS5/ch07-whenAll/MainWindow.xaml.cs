using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
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
            //
            // CH 7.2 new
            //

            IEnumerable<Task<Image>> tasks = s_Domains.Select(GetFaviconAsync);
            Task<Image[]> allTask = Task.WhenAll(tasks);
            Image[] images = await allTask;
            foreach (Image eachImage in images)
            {
                AddAFavicon(eachImage);
            }
        }

        private async Task<Image> GetFaviconAsync(string domain)
        {
            string url = "http://" + domain + "/favicon.ico";
            WebClient webClient = new WebClient();

            Debug.WriteLine($"DownloadDataTaskAsync: {url}");

            byte[] bytes = await webClient.DownloadDataTaskAsync(url);
            return MakeImageControl(bytes);
        }

        private void AddAFavicon(Image imageControl)
        {
            m_WrapPanel.Children.Add(imageControl);
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

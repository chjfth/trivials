using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Reflection;
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

        private void GetButton_OnClick(object sender, RoutedEventArgs e)
        {
            foreach (string domain in s_Domains)
            {
                AddAFavicon(domain);
            }
        }

        private void AddAFavicon(string domain)
        {
            WebClient webClient = new WebClient();
            string url = "http://" + domain + "/favicon.ico";
            webClient.BaseAddress = url; // we have to set this ourselves, so to retrieve it later

            webClient.DownloadDataCompleted += OnWebClientOnDownloadDataCompleted;
            webClient.DownloadDataAsync(new Uri(url));

            //Debug.WriteLine("DownloadDataAsync() started: "+ url);
        }

        private void OnWebClientOnDownloadDataCompleted(object sender, DownloadDataCompletedEventArgs args)
        {
            // Chj Note: If web-request fails, accessing `args.Result` triggers TargetInvocationException,
            // and its InnerException tells the actual error reason. 

            try
            {
                Image imageControl = MakeImageControl(args.Result);
                m_WrapPanel.Children.Add(imageControl);
            }
            catch (Exception e)
            {
                WebClient webClient = (WebClient) sender;

                string errReason = e.Message;
                if (e is TargetInvocationException)
                    errReason = e.InnerException.Message;

                string info = $"HTTP error on: {webClient.BaseAddress}\nReason: {errReason}";
                Debug.WriteLine(info);
                MessageBox.Show(info, "HTTP Error",
                    MessageBoxButton.OK, MessageBoxImage.Exclamation);
            }
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

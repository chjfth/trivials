using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
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

            //ServicePointManager.DefaultConnectionLimit = 5;
            //
            // -- not useful for this program, bcz I find that this value(5) limits pending 
            // HTTP msg-exchange destined for a specific HOST. 
            // Because we access different HOSTs in this program, so that this program
            // can always launch 5 HTTP requests simultaneously (verified on Win7, with
            // the help of tcp-send-delay.py)
        }

        private void GetButton_OnClick(object sender, RoutedEventArgs e)
        {
            foreach (string domain in s_Domains)
            {
                AddAFavicon(domain);
            }
        }

        private async void AddAFavicon(string domain)
        {
            string url = "http://" + domain + "/favicon.ico";
            try
            {
                WebClient webClient = new WebClient();
                byte[] bytes = await webClient.DownloadDataTaskAsync(url);
                //
                // -- If web-request fails, WebException will be thrown from above await statement.

                Image imageControl = MakeImageControl(bytes);
                m_WrapPanel.Children.Add(imageControl);
            }
            catch (WebException e)
            {
                string errReason = e.Message;

                string info = $"HTTP error on: {url}\nReason: {errReason}";
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

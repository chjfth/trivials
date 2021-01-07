using System;
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
            "www.somebaddomain.com",
            @"what\the/heck?",
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


        private async void GetButton_OnClick(object sender, RoutedEventArgs evt)
        {
            //
            // CH 7.2 new with fine error checking and reporting, according to p58-59
            //

            List<Task<Image>> tasks = new List<Task<Image>>();
            foreach (string domain in s_Domains)
            {
                tasks.Add(GetFaviconAsync(domain));
            }

            Task<Image[]> allTask = Task.WhenAll(tasks);

            try
            {
                Image[] images = await allTask;
            }
            catch (Exception)
            {
                // do nothing here
            }
            finally
            {
                // Some URL may success, some may fail.
                // We'll show icons for the success ones, and report those fail ones.
                string info_err = "";

                for (int i = 0; i < tasks.Count; i++)
                {
                    var tsk = tasks[i];
                    string domain = s_Domains[i];
                    string info = $"{i + 1}. {domain}\r\n";

                    if (tsk.Exception == null) // success
                    {
                        AddAFavicon(tsk.Result);
                    }
                    else // fail
                    {
                        // tsk.Exception is an AggregateException

                        string excptmsg = "";
                        var inner = tsk.Exception.InnerException;
                        while (inner != null)
                        {
                            excptmsg += "  " + inner.Message + "\r\n";
                            inner = inner.InnerException;
                        }

                        info += $"  ERROR: {excptmsg}\r\n";
                    }

                    info_err += info;
                }

                if (info_err != "")
                {
                    Debug.WriteLine(info_err);
                    MessageBox.Show(info_err, "Some error occured",
                        MessageBoxButton.OK, MessageBoxImage.Exclamation);
                }
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

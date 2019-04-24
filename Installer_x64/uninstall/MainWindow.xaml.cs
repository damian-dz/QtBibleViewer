using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Threading;
using System.Windows;
using System.Windows.Controls;

namespace uninstall
{
    public partial class MainWindow : Window
    {
        private ResourceDictionary langDict;

        private List<UIElement> elements1 = new List<UIElement>();
        private List<UIElement> elements2 = new List<UIElement>();

        private Button frstButton;
        private Button scndButton;
        private Button thrdButton;

        private static Assembly crntAssembly = Assembly.GetExecutingAssembly();
        private string uninstPath = crntAssembly.Location;

        private bool dltRoot;
        private bool uninstCompleted;

        private void SetLanguageDictionary()
        {
            langDict = new ResourceDictionary();
            const string path = "Resources/";
            switch (Thread.CurrentThread.CurrentCulture.ToString())
            {
                case "en-US":
                    langDict.Source = new Uri(path + "en-US.xaml", UriKind.Relative);
                    break;
                case "es-ES":
                    langDict.Source = new Uri(path + "es-ES.xaml", UriKind.Relative);
                    break;
                case "pl-PL":
                    langDict.Source = new Uri(path + "pl-PL.xaml", UriKind.Relative);
                    break;
                default:
                    langDict.Source = new Uri(path + "en-US.xaml", UriKind.Relative);
                    break;
            }
            Resources.MergedDictionaries.Add(langDict);
        }

        private void DrawFirstWindow(bool forward)
        {
            if (!forward)
            {
                mainGrid.Children.Remove(frstButton);
                scndButton.Content = langDict["next"];
                scndButton.Click -= UninstallButton_Click;
                scndButton.Click += NextButton1_Click;
                scndButton.IsEnabled = true;
            }

            if (elements1.Count == 0)
            {
                var welcomeLabel = new Label
                {
                    Content = langDict["welcome"],
                    FontSize = 16,
                    FontWeight = FontWeights.Bold,
                    HorizontalAlignment = HorizontalAlignment.Center,
                    VerticalAlignment = VerticalAlignment.Top,
                    Margin = new Thickness(0, 10, 0, 0)
                };
                elements1.Add(welcomeLabel);

                var instructionBlock = new TextBlock
                {
                    Text = langDict["instruction"].ToString(),
                    FontSize = 12,
                    HorizontalAlignment = HorizontalAlignment.Center,
                    Margin = new Thickness(0, 100, 0, 0),
                    TextWrapping = TextWrapping.WrapWithOverflow,
                    VerticalAlignment = VerticalAlignment.Top
                };
                elements1.Add(instructionBlock);

                scndButton = new Button
                {
                    Content = langDict["next"],
                    Margin = new Thickness(0, 0, 120, 10),
                    HorizontalAlignment = HorizontalAlignment.Right,
                    VerticalAlignment = VerticalAlignment.Bottom,
                    Width = 100,
                    Height = 23
                };
                scndButton.Click += NextButton1_Click;

                thrdButton = new Button
                {
                    Content = langDict["close"],
                    Margin = new Thickness(0, 0, 10, 10),
                    HorizontalAlignment = HorizontalAlignment.Right,
                    VerticalAlignment = VerticalAlignment.Bottom,
                    Width = 100,
                    Height = 23,
                };
                thrdButton.Click += CloseButton_Click;
            }

            if (forward)
            {
                mainGrid.Children.Add(scndButton);
                mainGrid.Children.Add(thrdButton);
            }
            foreach (UIElement elem in elements1)
            {
                mainGrid.Children.Add(elem);
            }
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void DrawSecondWindow()
        {
            if (elements2.Count == 0)
            {
                var dirLabel = new Label
                {
                    Content = langDict["optnLbl"],
                    FontSize = 12,
                    HorizontalAlignment = HorizontalAlignment.Left,
                    VerticalAlignment = VerticalAlignment.Top,
                    Margin = new Thickness(10, 10, 0, 0),
                    Padding = new Thickness(0, 0, 0, 0)
                };
                elements2.Add(dirLabel);

                var eraseAllRadioButton = new RadioButton
                {
                    Content = langDict["eraseAll"],
                    GroupName = "Options",
                    Margin = new Thickness(10, 35, 10, 0),
                    VerticalAlignment = VerticalAlignment.Top,
                    IsChecked = true
                };
                elements2.Add(eraseAllRadioButton);

                var leaveFilesRadioButton = new RadioButton
                {
                    Content = langDict["leaveFiles"],
                    GroupName = "Options",
                    Margin = new Thickness(10, 55, 10, 0),
                    VerticalAlignment = VerticalAlignment.Top,
                };
                elements2.Add(leaveFilesRadioButton);

                var progressTextBox = new TextBox
                {
                    AcceptsReturn = true,
                    Margin = new Thickness(10, 75, 10, 40),
                    VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                    IsReadOnly = true,
                };
                elements2.Add(progressTextBox);

                frstButton = new Button
                {
                    Content = langDict["back"],
                    Margin = new Thickness(0, 0, 230, 10),
                    HorizontalAlignment = HorizontalAlignment.Right,
                    VerticalAlignment = VerticalAlignment.Bottom,
                    Width = 100,
                    Height = 23
                };
                frstButton.Click += BackButton2_Click;
            }

            scndButton.Content = langDict["uninstall"];
            scndButton.Click -= NextButton1_Click;
            scndButton.Click += UninstallButton_Click;

            mainGrid.Children.Add(frstButton);
            foreach (UIElement elem in elements2)
            {
                mainGrid.Children.Add(elem);
            }
        }

        public MainWindow()
        {
            SetLanguageDictionary();
            InitializeComponent();
            DrawFirstWindow(true);
            Closed += MainWindow_Closed;
        }

        private void MainWindow_Closed(object sender, EventArgs e)
        {
            if (uninstCompleted)
            {
                Stream stream = crntAssembly.GetManifestResourceStream("uninstall.Resources.uninstallDeleter.exe");
                var buffer = new byte[stream.Length];
                stream.Read(buffer, 0, buffer.Length);
                string tempFilePath = System.IO.Path.Combine(System.IO.Path.GetTempPath(),
                    "uninstallDeleter" + DateTime.Now.ToString("yyyyMMddHHmmss"));
                File.WriteAllBytes(tempFilePath, buffer);

                int uninstId = Process.GetCurrentProcess().Id;
                var startInfo = new ProcessStartInfo
                {
                    FileName = tempFilePath,
                    CreateNoWindow = true,
                    Arguments = uninstId + " " + uninstPath + " " + dltRoot,
                    UseShellExecute = false,
                    Verb = "runas"
                };
                Process.Start(startInfo);
            }
        }

        private void NextButton1_Click(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in elements1)
            {
                mainGrid.Children.Remove(child);
            }
            DrawSecondWindow();
        }

        private void BackButton2_Click(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in elements2)
            {
                mainGrid.Children.Remove(child);
            }
            DrawFirstWindow(false);
        }

        private void UninstallButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string appDir = System.IO.Path.GetDirectoryName(uninstPath);
                string[] files = Directory.GetFiles(appDir, "*", SearchOption.AllDirectories);
                dltRoot = (elements2[1] as RadioButton).IsChecked.Value;
                var progressBox = elements2[3] as TextBox;
                frstButton.IsEnabled = false;
                scndButton.IsEnabled = false;
                foreach (var file in files)
                {
                    if (file != uninstPath)
                    {
                        progressBox.AppendText(file + Environment.NewLine);
                        File.Delete(file);
                    }
                }
                scndButton.Content = langDict["finish"];
                scndButton.Click -= UninstallButton_Click;
                scndButton.Click += FinishButton_Click;
                uninstCompleted = true;
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString());
            }
        }

        private void FinishButton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }
    }
}

using IWshRuntimeLibrary;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows;
using WinForms = System.Windows.Forms;
using WPFCtrls = System.Windows.Controls;

namespace QtBibleViewerInstaller
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private ResourceDictionary langDict;
        private readonly string resPrefix = "QtBibleViewerInstaller.Resources.Program";
        private Assembly crntAssembly = Assembly.GetExecutingAssembly();
        private List<UIElement> elements1 = new List<UIElement>();
        private List<UIElement> elements2 = new List<UIElement>();
        private List<UIElement> elements3 = new List<UIElement>();
        private List<UIElement> elements4 = new List<UIElement>();
        private string instDir;
        private int totalBufferSize;
        private const int packedFilesSize = 19637284;
        private bool createDesktop;
        private bool createStartMenu;
        private bool useAppData;
        private bool startAppWhenClosed = false;

        private void SetLanguageDictionary()
        {
            langDict = new ResourceDictionary();
            const string path = "Resources/Installer/";
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

        private void DrawFirstWindow()
        {
            var welcomeLabel = new WPFCtrls.Label
            {
                Content = langDict["welcome"],
                FontSize = 16,
                FontWeight = FontWeights.Bold,
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Top,
                Margin = new Thickness(0, 10, 0, 0)
            };
            elements1.Add(welcomeLabel);
            var instructionBlock = new WPFCtrls.TextBlock
            {
                Text = (string)langDict["instruction"],
                FontSize = 12,
                HorizontalAlignment = HorizontalAlignment.Center,
                Margin = new Thickness(0, 100, 0, 0),
                TextWrapping = TextWrapping.WrapWithOverflow,
                VerticalAlignment = VerticalAlignment.Top
            };
            elements1.Add(instructionBlock);
            var nextButton = new WPFCtrls.Button
            {
                Content = langDict["next"],
                Margin = new Thickness(0, 0, 120, 10),
                HorizontalAlignment = HorizontalAlignment.Right,
                VerticalAlignment = VerticalAlignment.Bottom,
                Width = 100,
                Height = 23
            };
            nextButton.Click += NextButton1_Click;         
            var closeButton = new WPFCtrls.Button
            {
                Content = langDict["close"],
                Margin = new Thickness(0, 0, 10, 10),
                HorizontalAlignment = HorizontalAlignment.Right,
                VerticalAlignment = VerticalAlignment.Bottom,
                Width = 100,
                Height = 23
            };
            closeButton.Click += CloseButton_Click;
            foreach (UIElement elem in elements1)
            {
                mainGrid.Children.Add(elem);
            }
            mainGrid.Children.Add(nextButton);
            mainGrid.Children.Add(closeButton);
        }

        private void DrawSecondWindow()
        {
            var licenseLabel = new WPFCtrls.Label
            {
                Content = langDict["license"],
                FontSize = 16,
                FontWeight = FontWeights.Bold,
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Top,
                Margin = new Thickness(0, 10, 0, 0)
            };
            elements2.Add(licenseLabel);
            var licenseTextBox = new WPFCtrls.TextBox
            {
                AcceptsReturn = true,
                Margin = new Thickness(10, 50, 10, 60),
                VerticalScrollBarVisibility = WPFCtrls.ScrollBarVisibility.Auto,
                VerticalAlignment = VerticalAlignment.Top,
                IsReadOnly = true,
                Text = LoadLicenseText()
            };
            elements2.Add(licenseTextBox);
            var licenseCheckBox = new WPFCtrls.CheckBox
            {
                Content = langDict["accept"],
                Margin = new Thickness(10, 50, 10, 40),
                VerticalAlignment = VerticalAlignment.Bottom,
            };
            licenseCheckBox.Checked += LicenseCheckBox_Changed;
            licenseCheckBox.Unchecked += LicenseCheckBox_Changed;
            elements2.Add(licenseCheckBox);
            var nextButton = mainGrid.Children[0] as WPFCtrls.Button;
            nextButton.Click -= NextButton1_Click;
            nextButton.Click += NextButton2_Click;
            nextButton.IsEnabled = false;
            foreach (UIElement elem in elements2)
            {
                mainGrid.Children.Add(elem);
            }
        }

        private void LicenseCheckBox_Changed(object sender, RoutedEventArgs e)
        {
            var nextButton = mainGrid.Children[0] as WPFCtrls.Button;
            nextButton.IsEnabled = (bool)(sender as WPFCtrls.CheckBox).IsChecked;
        }

        private void DrawThirdWindow()
        {
            var dirLabel = new WPFCtrls.Label
            {
                Content = langDict["dirLbl"],
                FontSize = 12,
                HorizontalAlignment = HorizontalAlignment.Left,
                VerticalAlignment = VerticalAlignment.Top,
                Margin = new Thickness(10, 10, 0, 0),
                Padding = new Thickness(0, 0, 0, 0)
            };
            elements3.Add(dirLabel);
            var dirTextBox = new WPFCtrls.TextBox
            {
                HorizontalAlignment = HorizontalAlignment.Stretch,
                Margin = new Thickness(10, 35, 120, 0),
                Height = 23,
                VerticalAlignment = VerticalAlignment.Top,
                Text = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles) + @"\QtBibleViewer"
            };
            elements3.Add(dirTextBox);
            var changeButton = new WPFCtrls.Button
            {
                Content = langDict["chngBtn"],
                Margin = new Thickness(0, 35, 10, 10),
                HorizontalAlignment = HorizontalAlignment.Right,
                VerticalAlignment = VerticalAlignment.Top,
                Width = 100,
                Height = 23
            };
            changeButton.Click += ChangeButton_Click;
            elements3.Add(changeButton);
            var dsktCheckBox = new WPFCtrls.CheckBox
            {
                Content = langDict["dsktChk"],
                Margin = new Thickness(10, 65, 10, 0),
                VerticalAlignment = VerticalAlignment.Top,
                IsChecked = true
            };
            elements3.Add(dsktCheckBox);
            var strtCheckBox = new WPFCtrls.CheckBox
            {
                Content = langDict["strtChk"],
                Margin = new Thickness(10, 85, 10, 0),
                VerticalAlignment = VerticalAlignment.Top,
                IsChecked = true
            };
            elements3.Add(strtCheckBox);
            var apdtCheckBox = new WPFCtrls.CheckBox
            {
                Content = langDict["apdtChk"],
                Margin = new Thickness(250, 65, 10, 0),
                VerticalAlignment = VerticalAlignment.Top,
            };
            elements3.Add(apdtCheckBox);
            var progressTextBox = new WPFCtrls.TextBox
            {
                AcceptsReturn = true,
                Margin = new Thickness(10, 105, 10, 65),
                VerticalScrollBarVisibility = WPFCtrls.ScrollBarVisibility.Auto,
                IsReadOnly = true,
            };
            elements3.Add(progressTextBox);
            var progressBar = new WPFCtrls.ProgressBar
            {
                Margin = new Thickness(10, 70, 10, 40),
                VerticalAlignment = VerticalAlignment.Bottom,
                Height = 20,
                Maximum = packedFilesSize
            };
            elements3.Add(progressBar);
            var installButton = mainGrid.Children[0] as WPFCtrls.Button;
            installButton.Content = langDict["install"];
            installButton.Click -= NextButton2_Click;
            installButton.Click += InstallButton_Click;
            foreach (UIElement elem in elements3)
            {
                mainGrid.Children.Add(elem);
            }
        }

        private void DrawFourthWindow()
        {
            var successLabel = new WPFCtrls.Label
            {
                Content = langDict["success"],
                FontSize = 16,
                FontWeight = FontWeights.Bold,
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Top,
                Margin = new Thickness(0, 10, 0, 0)
            };
            elements4.Add(successLabel);
            var youMayBlock = new WPFCtrls.TextBlock
            {
                Text = langDict["youMay"].ToString(),
                FontSize = 12,
                HorizontalAlignment = HorizontalAlignment.Center,
                Margin = new Thickness(0, 100, 0, 0),
                TextWrapping = TextWrapping.WrapWithOverflow,
                VerticalAlignment = VerticalAlignment.Top
            };
            elements4.Add(youMayBlock);
            var runCheckBox = new WPFCtrls.CheckBox
            {
                Content = langDict["runQBV"],
                Margin = new Thickness(0, 100, 0, 0),
                VerticalAlignment = VerticalAlignment.Center,
                HorizontalAlignment = HorizontalAlignment.Center,
                IsChecked = true
            };
            elements4.Add(runCheckBox);
            var finishButton = mainGrid.Children[0] as WPFCtrls.Button;
            finishButton.Content = langDict["finish"];
            finishButton.Click -= NextButton3_Click;
            finishButton.Click += FinishButton_Click;
            foreach (UIElement elem in elements4)
            {
                mainGrid.Children.Add(elem);
            }
        }

        private string LoadLicenseText()
        {
            var stream = crntAssembly.GetManifestResourceStream(resPrefix + ".license.gpl-3.0.txt.gz");
            byte[] buffer = new byte[stream.Length];
            stream.Read(buffer, 0, buffer.Length);
            byte[] unpackedData = Decompress(buffer);
            return Encoding.Default.GetString(unpackedData);
        }

        public MainWindow()
        {
            SetLanguageDictionary();
            InitializeComponent();
            DrawFirstWindow();
            Closed += MainWindow_Closed;
            string[] result = crntAssembly.GetManifestResourceNames();
            foreach (var r in result)
            {
                Debug.WriteLine(r);
            }
        }

        [DllImport("user32.dll", SetLastError = true)]
        static extern void SwitchToThisWindow(IntPtr hWnd, bool fAltTab);

        private void MainWindow_Closed(object sender, EventArgs e)
        {
            if (startAppWhenClosed)
            {
                var procInfo = new ProcessStartInfo(instDir + @"\QtBibleViewer.exe")
                {
                    WindowStyle = ProcessWindowStyle.Normal
                };
                var proc = Process.Start(procInfo);
                IntPtr handle = proc.MainWindowHandle;
                SwitchToThisWindow(handle, true);
            }
        }

        private void ChangeButton_Click(object sender, RoutedEventArgs e)
        {
            using (var fbd = new WinForms.FolderBrowserDialog())
            {
                WinForms.DialogResult result = fbd.ShowDialog();
                if (result == WinForms.DialogResult.OK)
                {
                    (elements3[1] as WPFCtrls.TextBox).Text = fbd.SelectedPath;
                }
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

        private void NextButton2_Click(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in elements2)
            {
                mainGrid.Children.Remove(child);
            }
            DrawThirdWindow();
        }

        private void NextButton3_Click(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in elements3)
            {
                mainGrid.Children.Remove(child);
            }
            DrawFourthWindow();
        }

        private void FinishButton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
            startAppWhenClosed = (bool)(elements4[2] as WPFCtrls.CheckBox).IsChecked;
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private byte[] Decompress(byte[] data)
        {
            using (var compressedStream = new MemoryStream(data))
            using (var zipStream = new GZipStream(compressedStream, CompressionMode.Decompress))
            using (var resultStream = new MemoryStream())
            {
                zipStream.CopyTo(resultStream);
                return resultStream.ToArray();
            }
        }

        private void UnpackResourceFile(string resName, string subFolder = "")
        {
            string subPrefix = subFolder == "" ? "." : "." + subFolder + ".";
            var stream = crntAssembly.GetManifestResourceStream(resPrefix + subPrefix.Replace('\\', '.') + resName);
            string[] splitName = resName.Split('.');
            string saveName = splitName[0] + "." + splitName[1];
            if (splitName.Length > 3)
            {
                saveName += "." + splitName[2];
            }
            string saveSubPath = subFolder == "" ? @"\" + saveName : @"\" + subFolder + @"\" + saveName;
            var textToAppend = string.Format("{0} {1}.gz {2} {3}{4}",
                langDict["unpacking"], saveName, langDict["to"], instDir, saveSubPath);
            Dispatcher.Invoke(() =>
            {
                var progressBox = (elements3[6] as WPFCtrls.TextBox);
                progressBox.AppendText(textToAppend + Environment.NewLine);
                progressBox.ScrollToEnd();
            });
            byte[] buffer = new byte[stream.Length];
            stream.Read(buffer, 0, buffer.Length);
            totalBufferSize += buffer.Length;                
            byte[] unpackedData = Decompress(buffer);
            Dispatcher.Invoke(() =>
            {
                var progressBar = (elements3[7] as WPFCtrls.ProgressBar);
                progressBar.Value = totalBufferSize;
            });
            if (subFolder != "" && !Directory.Exists(Path.Combine(instDir, subFolder)))
            {
                Directory.CreateDirectory(Path.Combine(instDir, subFolder));
            }
            System.IO.File.WriteAllBytes(instDir + saveSubPath, unpackedData);
        }

        private void InstallButton_Click(object sender, RoutedEventArgs e)
        {
            instDir = (elements3[1] as WPFCtrls.TextBox).Text;
            createDesktop = (bool)(elements3[3] as WPFCtrls.CheckBox).IsChecked;
            createStartMenu = (bool)(elements3[4] as WPFCtrls.CheckBox).IsChecked;
            useAppData = (bool)(elements3[5] as WPFCtrls.CheckBox).IsChecked;
            var worker = new BackgroundWorker
            {
                WorkerSupportsCancellation = true
            };
            worker.DoWork += UnpackAllResourceFiles;
            worker.RunWorkerAsync();
        }

        private void UnpackAllResourceFiles(object sender, DoWorkEventArgs e)
        {
            if (!Directory.Exists(instDir))
            {
                Directory.CreateDirectory(instDir);
            }
            UnpackResourceFile("Qt5Charts.dll.gz");
            UnpackResourceFile("Qt5Core.dll.gz");
            UnpackResourceFile("Qt5Gui.dll.gz");
            UnpackResourceFile("Qt5Sql.dll.gz");
            UnpackResourceFile("Qt5Svg.dll.gz");
            UnpackResourceFile("Qt5Widgets.dll.gz");
            UnpackResourceFile("QtBibleViewer.exe.gz");
            UnpackResourceFile("uninstall.exe.gz");
            UnpackResourceFile("vcruntime140.dll.gz");
            if (!useAppData)
            {
                UnpackResourceFile("settings.ini.gz", @"App\config");
            }
            UnpackResourceFile("counters.bblv.gz", @"App\data");
            UnpackResourceFile("xref.bblv.gz", @"App\data");
            UnpackResourceFile("strong_lite.dct.mybible.gz", @"App\dictionaries");
            UnpackResourceFile("es.qm.gz", @"App\lang");
            UnpackResourceFile("pl.qm.gz", @"App\lang");
            UnpackResourceFile("kjv.bbl.mybible.gz", @"App\modules");
            UnpackResourceFile("kjvlite.bbl.mybible.gz", @"App\modules");
            UnpackResourceFile("pubg.bbl.mybible.gz", @"App\modules");
            UnpackResourceFile("qsvgicon.dll.gz", "iconengines");
            UnpackResourceFile("qgif.dll.gz", "imageformats");
            UnpackResourceFile("qicns.dll.gz", "imageformats");
            UnpackResourceFile("qico.dll.gz", "imageformats");
            UnpackResourceFile("qjpeg.dll.gz", "imageformats");
            UnpackResourceFile("qsvg.dll.gz", "imageformats");
            UnpackResourceFile("qtga.dll.gz", "imageformats");
            UnpackResourceFile("qtiff.dll.gz", "imageformats");
            UnpackResourceFile("qwbmp.dll.gz", "imageformats");
            UnpackResourceFile("qwebp.dll.gz", "imageformats");
            UnpackResourceFile("gpl-3.0.txt.gz", "license");
            UnpackResourceFile("qwindows.dll.gz", "platforms");
            UnpackResourceFile("qsqlite.dll.gz", "sqldrivers");
            UnpackResourceFile("qwindowsvistastyle.dll.gz", "styles");
            if (createDesktop)
            {
                CreateShortcut("dsktShort", Environment.GetFolderPath(Environment.SpecialFolder.Desktop));
            }
            if (createStartMenu)
            {
                CreateShortcut("strtShort", Environment.GetFolderPath(Environment.SpecialFolder.StartMenu));
            }
            RegisterUninstaller();
            Dispatcher.Invoke(() =>
            {
                var nextButton = mainGrid.Children[0] as WPFCtrls.Button;
                nextButton.Content = langDict["next"];
                nextButton.Click -= InstallButton_Click;
                nextButton.Click += NextButton3_Click;
            });
        }

        private void CreateShortcut(string id, string shortcutDir)
        {
            var shell = new WshShell();
            var shortcutFilePath = Path.Combine(shortcutDir, "QtBibleViewer.lnk");
            Dispatcher.Invoke(() =>
            {
                var progressBox = (elements3[6] as WPFCtrls.TextBox);
                progressBox.AppendText(langDict[id] + " " + shortcutFilePath + Environment.NewLine);
                progressBox.ScrollToEnd();
            });
            var shortcut = (IWshShortcut)shell.CreateShortcut(shortcutFilePath);
            shortcut.TargetPath = Path.Combine(instDir, "QtBibleViewer.exe");
            shortcut.WorkingDirectory = Path.Combine(instDir, "QtBibleViewer.exe");
            shortcut.Save();
        }

        private void RegisterUninstaller()
        {
            using (RegistryKey parentKey = Registry.LocalMachine.OpenSubKey(
                @"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall", true))
            {
                if (parentKey != null)
                {
                    Dispatcher.Invoke(() =>
                    {
                        var progressBox = (elements3[6] as WPFCtrls.TextBox);
                        progressBox.AppendText(langDict["regEntry"] +
                            @" HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\QtBibleViewer");
                        progressBox.ScrollToEnd();
                    });
                    RegistryKey subKey = parentKey.CreateSubKey("QtBibleViewer", true);
                    subKey.SetValue("DisplayName", "QtBibleViewer");
                    subKey.SetValue("DisplayVersion", "1.2");
                    subKey.SetValue("EstimatedSize", DirSizeInKB(new DirectoryInfo(instDir)), RegistryValueKind.DWord);
                    subKey.SetValue("InstallDate", DateTime.Now.ToString("yyyyMMdd"));
                    subKey.SetValue("InstallLocation", instDir);
                    subKey.SetValue("NoModify", 1, RegistryValueKind.DWord);
                    subKey.SetValue("NoRepair", 1, RegistryValueKind.DWord);
                    subKey.SetValue("Publisher", "Damian Dz.");
                    subKey.SetValue("UninstallString", instDir + @"\uninstall.exe");
                    subKey.SetValue("URLInfoAbout", "https://github.com/damian-dz/QtBibleViewer");
                    subKey.Close();
                }
            }
        }

        private long DirSizeInKB(DirectoryInfo dirInfo)
        {
            long size = 0;
            FileInfo[] fis = dirInfo.GetFiles();
            foreach (FileInfo fi in fis)
            {
                size += Convert.ToInt64(fi.Length / 1024d);
            }
            DirectoryInfo[] dis = dirInfo.GetDirectories();
            foreach (DirectoryInfo di in dis)
            {
                size += DirSizeInKB(di);
            }
            return size;
        }
    }
}

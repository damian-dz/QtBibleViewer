using IWshRuntimeLibrary;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows;
using WinForms = System.Windows.Forms;
using WPFCtrls = System.Windows.Controls;

namespace QtBibleViewerInstaller
{
    public partial class MainWindow : Window
    {
        private ResourceDictionary langDict;
        private const string resPrefix = "QtBibleViewerInstaller.Resources.Program";
        private Assembly crntAssembly = Assembly.GetExecutingAssembly();
        private List<UIElement> elements1 = new List<UIElement>();
        private List<UIElement> elements2 = new List<UIElement>();
        private List<UIElement> elements3 = new List<UIElement>();
        private List<UIElement> elements4 = new List<UIElement>();
        private string instDir;
        private int totalBufferSize;
        private const int packedFilesSize = 19785721;
        private bool createDesktop;
        private bool createStartMenu;
        private bool useAppData;
        private bool startAppWhenClosed = false;
        
        private WPFCtrls.Button frstButton;
        private WPFCtrls.Button scndButton;
        private WPFCtrls.Button thrdButton;

        private BackgroundWorker worker;
        private List<string> installedFiles = new List<string>();
        private List<string> createdDirs = new List<string>();

        private bool instDirExisted = true;
        private bool appDirExisted = true;

        private DateTime creationDate = new DateTime(2018, 11, 26);

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

        private void DrawFirstWindow(bool forward)
        {
            if (!forward)
            {
                mainGrid.Children.Remove(frstButton);
                scndButton.Click -= NextButton2_Click;
                scndButton.Click += NextButton1_Click;
                scndButton.IsEnabled = true;
            }

            if (elements1.Count == 0)
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
                    Text = langDict["instruction"].ToString(),
                    FontSize = 12,
                    HorizontalAlignment = HorizontalAlignment.Center,
                    Margin = new Thickness(0, 100, 0, 0),
                    TextWrapping = TextWrapping.WrapWithOverflow,
                    VerticalAlignment = VerticalAlignment.Top
                };
                elements1.Add(instructionBlock);

                scndButton = new WPFCtrls.Button
                {
                    Content = langDict["next"],
                    Margin = new Thickness(0, 0, 120, 10),
                    HorizontalAlignment = HorizontalAlignment.Right,
                    VerticalAlignment = VerticalAlignment.Bottom,
                    Width = 100,
                    Height = 23
                };
                scndButton.Click += NextButton1_Click;

                thrdButton = new WPFCtrls.Button
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

        private void DrawSecondWindow(bool forward)
        {
            if (!forward)
            {
                frstButton.Click -= BackButton3_Click;
                frstButton.Click += BackButton2_Click;
                scndButton.Content = langDict["next"];
                scndButton.Click -= InstallButton_Click;
                scndButton.Click += NextButton2_Click;
            }

            if (elements2.Count == 0)
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

                frstButton = new WPFCtrls.Button
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

            if (forward)
            {
                scndButton.Click -= NextButton1_Click;
                scndButton.Click += NextButton2_Click;
                scndButton.IsEnabled = (bool)(elements2[2] as WPFCtrls.CheckBox).IsChecked;
                mainGrid.Children.Add(frstButton);
            }

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
            if (elements3.Count == 0)
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
                    IsChecked = true
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
            }

            scndButton.Content = langDict["install"];
            scndButton.Click -= NextButton2_Click;
            scndButton.Click += InstallButton_Click;

            frstButton.Click -= BackButton2_Click;
            frstButton.Click += BackButton3_Click;

            foreach (UIElement elem in elements3)
            {
                mainGrid.Children.Add(elem);
            }
        }

        private void DrawFourthWindow()
        {
            mainGrid.Children.Remove(frstButton);

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
            Stream stream = crntAssembly.GetManifestResourceStream(resPrefix + ".license.gpl-3.0.txt.gz");
            var buffer = new byte[stream.Length];
            stream.Read(buffer, 0, buffer.Length);
            byte[] unpackedData = Decompress(buffer);
            return Encoding.Default.GetString(unpackedData);
        }

        public MainWindow()
        {
            SetLanguageDictionary();
            InitializeComponent();
            DrawFirstWindow(true);
            Closed += MainWindow_Closed;
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
            DrawSecondWindow(true);
        }

        private void BackButton2_Click(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in elements2)
            {
                mainGrid.Children.Remove(child);
            }
            DrawFirstWindow(false);
        }

        private void NextButton2_Click(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in elements2)
            {
                mainGrid.Children.Remove(child);
            }
            DrawThirdWindow();
        }

        private void BackButton3_Click(object sender, RoutedEventArgs e)
        {
            Debug.WriteLine("BackButton3_Click");
            foreach (UIElement child in elements3)
            {
                mainGrid.Children.Remove(child);
            }
            DrawSecondWindow(false);
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
            startAppWhenClosed = (bool)(elements4[2] as WPFCtrls.CheckBox).IsChecked;
            Application.Current.Shutdown();
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            if (worker.IsBusy)
            {
                worker.CancelAsync();
            }
            else
            {
                Application.Current.Shutdown();
            }         
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
                if (!string.IsNullOrWhiteSpace(progressBox.Text))
                {
                    progressBox.AppendText(Environment.NewLine);
                }
                progressBox.AppendText(textToAppend);
                progressBox.ScrollToEnd();
            });
            var buffer = new byte[stream.Length];
            stream.Read(buffer, 0, buffer.Length);
            totalBufferSize += buffer.Length;                
            byte[] unpackedData = Decompress(buffer);
            Dispatcher.Invoke(() =>
            {
                var progressBar = (elements3[7] as WPFCtrls.ProgressBar);
                progressBar.Value = totalBufferSize;
            });
            string subDir = Path.Combine(instDir, subFolder);
            if (subFolder != "" && !Directory.Exists(subDir))
            {
                Directory.CreateDirectory(subDir);
                createdDirs.Add(subDir);
                Directory.SetCreationTime(subDir, creationDate);
                Directory.SetLastAccessTime(subDir, creationDate);
                Directory.SetLastWriteTime(subDir, creationDate);
            }
            string filename = instDir + saveSubPath;
            System.IO.File.WriteAllBytes(filename, unpackedData);
            System.IO.File.SetCreationTime(filename, creationDate);
            System.IO.File.SetLastAccessTime(filename, creationDate);
            System.IO.File.SetLastWriteTime(filename, creationDate);
            installedFiles.Add(filename);
        }

        private void InstallButton_Click(object sender, RoutedEventArgs e)
        {
            frstButton.IsEnabled = false;
            scndButton.IsEnabled = false;
            instDir = (elements3[1] as WPFCtrls.TextBox).Text;
            createDesktop = (bool)(elements3[3] as WPFCtrls.CheckBox).IsChecked;
            createStartMenu = (bool)(elements3[4] as WPFCtrls.CheckBox).IsChecked;
            useAppData = (bool)(elements3[5] as WPFCtrls.CheckBox).IsChecked;
            worker = new BackgroundWorker
            {
                WorkerSupportsCancellation = true
            };
            worker.DoWork += UnpackAllResourceFiles;
            worker.RunWorkerCompleted += WorkCompleted;
            worker.RunWorkerAsync();
        }

        private void UnpackAllResourceFiles(object sender, DoWorkEventArgs e)
        {
            if (!Directory.Exists(instDir))
            {
                Directory.CreateDirectory(instDir);
                Directory.SetCreationTime(instDir, creationDate);
                Directory.SetLastAccessTime(instDir, creationDate);
                Directory.SetLastWriteTime(instDir, creationDate);
                appDirExisted = false;
            }
            else
            {
                if (!Directory.Exists(Path.Combine(instDir, "App")))
                {
                    appDirExisted = false;
                }
            }

            var paths = new Dictionary<string, string[]>
            {
                { "",
                    new string[]
                    {
                        "Qt5Charts.dll.gz",
                        "Qt5Core.dll.gz",
                        "Qt5Gui.dll.gz",
                        "Qt5Sql.dll.gz",
                        "Qt5Svg.dll.gz",
                        "Qt5Widgets.dll.gz",
                        "QtBibleViewer.exe.gz",
                        "uninstall.exe.gz",
                        "vcruntime140.dll.gz"
                    }
                },
                { @"App\data",
                    new string[]
                    {
                        "counters.bblv.gz",
                        "xref.bblv.gz"
                    }
                },
                { @"App\dictionaries",
                    new string[]
                    {
                        "strong_lite.dct.mybible.gz"
                    }
                },
                { @"App\lang",
                    new string[]
                    {
                        "es.qm.gz",
                        "pl.qm.gz",
                        "qt_es.qm.gz",
                        "qt_pl.qm.gz"
                    }
                },
                { @"App\modules",
                    new string[]
                    {
                        "kjv.bbl.mybible.gz",
                        "kjvlite.bbl.mybible.gz",
                        "pubg.bbl.mybible.gz"
                    }
                },
                { "iconengines",
                    new string[]
                    {
                        "qsvgicon.dll.gz"
                    }
                },
                { "imageformats",
                    new string[]
                    {
                        "qgif.dll.gz",
                        "qicns.dll.gz",
                        "qico.dll.gz",
                        "qjpeg.dll.gz",
                        "qsvg.dll.gz",
                        "qtga.dll.gz",
                        "qtiff.dll.gz",
                        "qwbmp.dll.gz",
                        "qwebp.dll.gz"
                    }
                },
                { "license",
                    new string[]
                    {
                        "gpl-3.0.txt.gz"
                    }
                },
                { "platforms",
                    new string[]
                    {
                        "qwindows.dll.gz"
                    }    
                },
                { "sqldrivers",
                    new string[]
                    {
                        "qsqlite.dll.gz"
                    }
                },
                { "styles",
                    new string[]
                    {
                        "qwindowsvistastyle.dll.gz"
                    }
                }
            };

            for (int i = 0; i < paths.Count; i++)
            {
                string folder = paths.ElementAt(i).Key;
                string[] files = paths.ElementAt(i).Value;
                for (int j = 0; j < files.Length; j++)
                {
                    if (worker.CancellationPending)
                    {
                        e.Cancel = true;
                        return;
                    }
                    UnpackResourceFile(files[j], folder);
                }
            }
            if (!useAppData)
            {
                UnpackResourceFile("settings.ini.gz", @"App\config");
            }

            if (createDesktop)
            {
                CreateShortcut("dsktShort", Environment.GetFolderPath(Environment.SpecialFolder.Desktop));
            }
            if (createStartMenu)
            {
                string mainFolder = Environment.GetFolderPath(Environment.SpecialFolder.StartMenu) + @"\Programs";
                string subFolder = Path.Combine(mainFolder, "QtBibleViewer");
                if (!Directory.Exists(subFolder))
                {
                    Directory.CreateDirectory(subFolder);
                }
                CreateShortcut("strtShort", subFolder);
            }

            RegisterUninstaller();

            GC.Collect();
        }


        private void WorkCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Cancelled)
            {
                MessageBox.Show(langDict["cancelled"].ToString());
                var progressBox = (elements3[6] as WPFCtrls.TextBox);
                for (int i = 0; i < installedFiles.Count; i++)
                {
                    progressBox.AppendText(langDict["deleting"] + " " + installedFiles[i]);
                    if (i < installedFiles.Count - 1 || createdDirs.Count > 0)
                    {
                        progressBox.AppendText(Environment.NewLine);
                    }
                    progressBox.ScrollToEnd();
                    System.IO.File.Delete(installedFiles[i]);
                }
                for (int i = 0; i < createdDirs.Count; i++)
                {
                    progressBox.AppendText(langDict["deleting"] + " " + createdDirs[i]);
                    if (i < createdDirs.Count - 1 || !appDirExisted)
                    {
                        progressBox.AppendText(Environment.NewLine);
                    }
                    progressBox.ScrollToEnd();
                    Directory.Delete(createdDirs[i]);
                }
                if (!appDirExisted && Directory.Exists(Path.Combine(instDir, "App")))
                {
                    progressBox.AppendText(langDict["deleting"] + " " + Path.Combine(instDir, "App"));
                    if (!instDirExisted)
                    {
                        progressBox.AppendText(Environment.NewLine);
                    }
                    Directory.Delete(Path.Combine(instDir, "App"));
                }
                if (!instDirExisted)
                {
                    progressBox.AppendText(langDict["deleting"] + " " + instDir);
                    Directory.Delete(instDir);
                }
            }
            else
            {
                scndButton.Content = langDict["next"];
                scndButton.Click -= InstallButton_Click;
                scndButton.Click += NextButton3_Click;
                scndButton.IsEnabled = true;
                if (!appDirExisted)
                {
                    string appDir = Path.Combine(instDir, "App");
                    Directory.SetCreationTime(appDir, creationDate);
                    Directory.SetLastAccessTime(appDir, creationDate);
                    Directory.SetLastWriteTime(appDir, creationDate);
                }
            }
        }

        private void CreateShortcut(string id, string shortcutDir)
        {
            var shell = new WshShell();
            var shortcutFilePath = Path.Combine(shortcutDir, "QtBibleViewer.lnk");
            Dispatcher.Invoke(() =>
            {
                var progressBox = (elements3[6] as WPFCtrls.TextBox);
                progressBox.AppendText(Environment.NewLine + langDict[id] + " " + shortcutFilePath);
                progressBox.ScrollToEnd();
            });
            var shortcut = (IWshShortcut)shell.CreateShortcut(shortcutFilePath);
            shortcut.TargetPath = Path.Combine(instDir, "QtBibleViewer.exe");
            shortcut.WorkingDirectory = Path.Combine(instDir, "QtBibleViewer.exe");
            shortcut.Save();
            using (FileStream fs = new FileStream(shortcutFilePath, FileMode.Open, FileAccess.ReadWrite))
            {
                fs.Seek(21, SeekOrigin.Begin);
                fs.WriteByte(0x22);
            }
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
                        progressBox.AppendText(Environment.NewLine + langDict["regEntry"] +
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

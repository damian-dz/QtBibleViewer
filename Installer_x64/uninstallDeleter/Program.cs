using System.Diagnostics;
using System.IO;

namespace uninstallDeleter
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 3)
            {
                int procId = int.Parse(args[0]);
                string uninstPath = args[1];

                try
                {
                    while (true)
                    {
                        Process.GetProcessById(procId);
                    }
                }
                catch
                {
                    File.Delete(uninstPath);
                    if (bool.Parse(args[2]))
                    {
                        string instDir = Path.GetDirectoryName(uninstPath);
                        Directory.Delete(instDir);
                    }
                }

            }
        }

    }
}

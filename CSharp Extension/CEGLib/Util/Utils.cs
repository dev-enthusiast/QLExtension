using System;
using System.Collections.Generic;
using System.Reflection;
using System.Linq;
using System.Text;

namespace CEGLib
{
    public class Utils
    {
        /// <summary>
        /// get path
        /// </summary>
        static public string AssemblyLoadDirectory
        {
            get
            {
                string codeBase = Assembly.GetCallingAssembly().CodeBase;
                UriBuilder uri = new UriBuilder(codeBase);
                string path = Uri.UnescapeDataString(uri.Path);
                return System.IO.Path.GetDirectoryName(path);
            }
        }
    }
}

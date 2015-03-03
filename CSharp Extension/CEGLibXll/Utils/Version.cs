using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using Xl = Microsoft.Office.Interop.Excel;
using ExcelDna.Integration;
using ExcelDna.Integration.CustomUI;
using ExcelDna.Logging;

namespace CEGLibXll
{
    public sealed class Version
    {
        [ExcelFunction(Description = "Display xll build time", IsMacroType = true, Category = "CEGLibXll - Operation")]
        public static DateTime cegOpLibXllBuildTime()
        {
            DateTime buildTime = new DateTime();
            try
            {
                buildTime = getXllBuildTime();
            }
            catch (Exception ex)
            {
                SystemUtil.logError("", "", ex.Message);
            }
            return buildTime;
        }

        [ExcelFunction(Description = "Display who built xll", IsMacroType = true, Category = "CEGLibXll - Operation")]
        public static string cegOpLibXllBuiltBy()
        {
            string user;
            try
            {
                user = getBuildUser();
            }
            catch (Exception ex)
            {
                SystemUtil.logError("", "", ex.Message);
                user = ex.Message;
            }
            return user;
        }

        [ExcelFunction(Description = "Display xll version", IsMacroType = true, Category = "CEGLibXll - Operation")]
        public static string cegOpVersion()
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            try
            {
                callerAddress = SystemUtil.getActiveCellAddress();
                OHRepository.Instance.removeErrorMessage(callerAddress);
            }
            catch (Exception)
            {
            }

            try
            {
                return getVerStr();
            }
            catch (Exception exception_)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), exception_.Message);
                return "";
            }
        }

        public static string getVerStr()
        {
            string readme = System.Environment.GetEnvironmentVariable("CEGLibXllInstallDir") + "/documents/README.txt";
            System.IO.StreamReader objReader;
            objReader = new System.IO.StreamReader(readme);
            string text = objReader.ReadLine();
            if (!(text.Contains("version") || text.Contains("Version")))
            {
                text = objReader.ReadLine();
            }
            string ver = text.Substring(text.IndexOf("ersion") + 7, 6);

            return ver;
        }

        public static string getXllPath()
        {
            string appName = new Uri(Assembly.GetCallingAssembly().CodeBase).LocalPath;
            string path = System.IO.Path.GetDirectoryName(appName);
            appName = System.IO.Path.Combine(path, "CEGLibXll.xll");
            return appName;
        }

        public static string getBuildUser()
        {
            var attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(BuildInfoAttribute), false);
            BuildInfoAttribute attribute = (BuildInfoAttribute)attributes[0];
            return attribute.UserName;
        }

        public static DateTime getXllBuildTime()
        {
            DateTime buildDate = new System.IO.FileInfo(getXllPath()).LastWriteTime;
            return buildDate;
        }
    }
}

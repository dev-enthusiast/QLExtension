using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Data.OleDb;
using ExcelDna.Integration;
using System.Data.SqlClient;
using System.Reflection;
using System.Diagnostics;

namespace QLXLL
{
    class QLOperation
    {
        public static bool CallFromWizardFlag = false;
        public static bool CallerAddressFlag = false;

        [ExcelFunction(Description = "toggle callFromWizard control", Category = "QLXLL - Operation")]
        public static bool qlCheckCallFromWizard(
            [ExcelArgument(Description = "true/false")] bool control)
        {
            CallFromWizardFlag = control;
            return CallFromWizardFlag;
        }

        [ExcelFunction(Description = "toggle callerAddress control", Category = "QLXLL - Operation")]
        public static bool qlCallerAddressControl(
            [ExcelArgument(Description = "true/false")] bool control)
        {
            CallerAddressFlag = control;
            return CallerAddressFlag;
        }

        [ExcelFunction(Description = "Display xll build time", IsMacroType = true, Category = "QLXLL - Operation")]
        public static DateTime qlXllBuildTime()
        {
            DateTime buildTime = new DateTime();
            try
            {
                buildTime = getXllBuildTime();
            }
            catch (Exception ex)
            {
                QLAddin.logError("", "", ex.Message);
            }
            return buildTime;
        }

        [ExcelFunction(Description = "Display who built xll", IsMacroType = true, Category = "QLXLL - Operation")]
        public static string qlXllBuiltBy()
        {
            string user;
            try
            {
                user = getBuildUser();
            }
            catch (Exception ex)
            {
                QLAddin.logError("", "", ex.Message);
                user = ex.Message;
            }
            return user;
        }

        [ExcelFunction(Description = "Display xll path", IsMacroType = true, Category = "QLXLL - Operation")]
        public static string qlXllPath()
        {
            string callerAddress = "";
            try
            {
                callerAddress = QLAddin.getActiveCellAddress();
                OHRepository.Instance.removeErrorMessage(callerAddress);
            }
            catch (Exception)
            {
            }

            string appName = null;
            try
            {
                appName = getXllPath();
            }
            catch (Exception exception_)
            {
                QLAddin.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), exception_.Message);
                appName = exception_.Message;
            }
            return appName;
        }

        [ExcelFunction(Description = "Display xll version", IsMacroType = true, Category = "QLXLL - Operation")]
        public static string qlVersion()
        {
            if (QLAddin.CallFromWizard())
                return "";

            string callerAddress = "";
            try
            {
                callerAddress = QLAddin.getActiveCellAddress();
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
                QLAddin.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), exception_.Message);
                return "";
            }
        }


        public static string getVerStr()
        {
            string readme = System.Environment.GetEnvironmentVariable("QLXLLInstallDir") + "/documents/README.txt";
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
            appName = System.IO.Path.Combine(path, "QLXLL.xll");
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

        /*
        [ExcelFunction(Description = "Reload Default File", Category = "QLXLL - Operation")]
        public static string qlMarketDataConvention(
            [ExcelArgument(Description = "default value query string")]String queryString)
        {
            if (QLAddin.CallFromWizard())
                return "";

            string callerAddress = QLAddin.getActiveCellAddress();

            OHRepository.Instance.removeErrorMessage(callerAddress);

            try
            {
                return QLExpansion.ConfigurationRegistry.Instance.get_child(queryString);
            }
            catch (Exception ex)
            {
                QLAddin.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), ex.Message);
                return "#QL_ERR!";
            }
        }

        [ExcelFunction(Description = "Reload Default File", Category = "QLXLL - Operation")]
        public static string qlMarketDataConventionDictionary(
            [ExcelArgument(Description = "default path query string")]String path,
            [ExcelArgument(Description = "default key query string")]String key,
            [ExcelArgument(Description = "set to capital")]bool capital)
        {
            if (QLAddin.CallFromWizard())
                return "";

            string callerAddress = QLAddin.getActiveCellAddress();

            OHRepository.Instance.removeErrorMessage(callerAddress);

            try
            {
                Dictionary<String, String> dict = QLExpansion.ConfigurationRegistry.Instance.get_grand_child(path, key);
                Object[,] ret = new Object[dict.Count, 2];
                int i = 0;
                foreach (var kvp in dict)
                {
                    ret[i, 0] = capital ? kvp.Key.ToUpper() : kvp.Key;
                    ret[i, 1] = capital ? kvp.Value.ToUpper() : kvp.Value;
                    i++;
                }
                return ret;
            }
            catch (Exception e)
            {
                QLAddin.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "#QL_ERR!";
            }
        }

        [ExcelFunction(Description = "Turn Logger on/off", Category = "QLXLL - Operation")]
        public static object qlLoggerControl(
            [ExcelArgument(Description = "turn on ql logger")]bool onOff)
        {
            if (QLAddin.CallFromWizard())
                return 0.0;

            string callerAddress = QLAddin.getActiveCellAddress();

            OHRepository.Instance.removeErrorMessage(callerAddress);

            try
            {
                return QLExpansion.MLogger.Instance.EnableLog(onOff);
            }
            catch (Exception ex)
            {
                QLAddin.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), ex.Message);
                return "#QL_ERR!";
            }
        }
        */

        #region Enum
        public static Dictionary<string, string> EnumDictionary = new Dictionary<string, string>();

        public static void addEnumDictionary()
        {
            EnumDictionary.Add("MF", "ModifiedFollowing");
            EnumDictionary.Add("Modified Following", "ModifiedFollowing");
            EnumDictionary.Add("F", "Following");
            EnumDictionary.Add("P", "Preceding");
            EnumDictionary.Add("MP", "ModifiedPreceding");
            EnumDictionary.Add("Modified Preceding", "ModifiedPreceding");
        }

        [ExcelFunction(Description = "Display enumeration type", Category = "QLXLL - Operation")]
        public static object qlDisplayAllEnum()
        {
            Dictionary<string, List<String>> enumdic = QLExpansion.EnumRepository.Instance.getEnumMap();

            int col = 0;
            string[,] ret = new string[100, enumdic.Count];
            foreach (KeyValuePair<string, List<string>> kvp in enumdic)
            {
                int row = 0;
                ret[row++, col] = kvp.Key;
                foreach (string data in kvp.Value)
                {
                    ret[row++,col] = data;
                }
                while (row < 100)
                {
                    ret[row++, col] = "";
                }
                col++;
            }
            
            return ret;
        }

        [ExcelFunction(Description = "List the IDs of objects in repository", Category = "QLXLL - Operation")]
        public static object qlListObjects(
            [ExcelArgument(Description = "pattern ")] string pattern)
        {
            if (QLAddin.CallFromWizard())
                return new string[0, 0];

            List<String> objids = OHRepository.Instance.listObjects(pattern);

            object[,] ret = new object[objids.Count, 1];
            int i = 0;
            foreach (string str in objids)
            {
                ret[i++, 0] = str;
            }

            return ret;
        }

        [ExcelFunction(Description = "Get object class name", Category = "QLXLL - Operation")]
        public static string qlObjectClassName(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (QLAddin.CallFromWizard())
                return "";

            return OHRepository.Instance.getObjectType(objID).Name;
        }

        [ExcelFunction(Description = "Get object caller address", Category = "QLXLL - Operation")]
        public static string qlObjectCallerAddress(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (QLAddin.CallFromWizard())
                return "";

            return OHRepository.Instance.getCallerAddress(objID);
        }

        [ExcelFunction(Description = "Get object creation time", Category = "QLXLL - Operation")]
        public static DateTime qlObjectCreationTime(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (QLAddin.CallFromWizard())
                return DateTime.MinValue;

            return OHRepository.Instance.getObjectCreationTime(objID);
        }

        [ExcelFunction(Description = "Get object update time", Category = "QLXLL - Operation")]
        public static DateTime qlObjectUpdateTime(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (QLAddin.CallFromWizard())
                return DateTime.MinValue;

            return OHRepository.Instance.getObjectUpdateTime(objID);
        }
        #endregion
    }
}

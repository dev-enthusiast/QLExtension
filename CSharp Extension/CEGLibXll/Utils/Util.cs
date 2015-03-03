using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xl = Microsoft.Office.Interop.Excel;
using ExcelDna.Integration;
using ExcelDna.Integration.CustomUI;
using ExcelDna.Logging;

namespace CEGLibXll
{
    public class Util
    {
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

        /*
        [ExcelFunction(Description = "Display enumeration type", Category = "CEGLibXll - Operation")]
        public static object cegDisplayAllEnum()
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
                    ret[row++, col] = data;
                }
                while (row < 100)
                {
                    ret[row++, col] = "";
                }
                col++;
            }

            return ret;
        }
        */
        /// <summary>
        /// See conversion for details
        /// </summary>
        /// <param name="enumclassname"></param>
        /// <returns></returns>
        [ExcelFunction(Description = "retrieve enum list", Category = "CEGLibXll - Operation")]
        public static object cegOpGetEnumerationList(
            [ExcelArgument(Description = "Enum Class Name (Daycounter, Calendar, BusinessDayConvention)")] string enumclassname)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = SystemUtil.getActiveCellAddress();

            try
            {
                // by default ExcelDna is horizontal; make vertical
                object[,] ret = new object[10,1];
                for (int i = 0; i < 10; i ++ )
                {
                    ret[i,0] = "";
                }
                switch (enumclassname.ToUpper())
                {
                    case "DAYCOUNTER":
                        ret[0,0] = "ACTUAL360";
                        ret[1,0] = "ACTUAL365";
                        ret[2,0] = "ACTUALACTUAL";
                        break;
                    case "CALENDAR":
                        ret[0,0] = "NYC";
                        ret[1,0] = "LON";
                        ret[2,0] = "NYC|LON";
                        break;
                    case "BUSINESSDAYCONVENTION":
                        ret[0,0] = "F";
                        ret[1,0] = "MF";
                        ret[2,0] = "P";
                        ret[3,0] = "MP";
                        ret[4,0] = "NONE";
                        break;
                    case "DGRULE":      // Date Generation Rule
                        ret[0, 0] = "Backward";
                        ret[1, 0] = "Forward";
                        ret[2, 0] = "Zero";
                        ret[3, 0] = "ThirdWednesday";
                        ret[4, 0] = "Twentieth";
                        ret[5, 0] = "TwentiethIMM";
                        ret[6, 0] = "CDS";
                        break;
                    default:
                        ret[0,0] = "unkown enum type";
                        break;
                }

                return ret;
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return e.Message;
            }
        }

        [ExcelFunction(Description = "List the IDs of objects in repository", Category = "CEGLibXll - Operation")]
        public static object cegOpListObjects(
            [ExcelArgument(Description = "pattern ")] string pattern)
        {
            if (SystemUtil.CallFromWizard())
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

        [ExcelFunction(Description = "Get object class name", Category = "CEGLibXll - Operation")]
        public static string cegOpObjectClassName(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            return OHRepository.Instance.getObjectType(objID).Name;
        }

        [ExcelFunction(Description = "Get object caller address", Category = "CEGLibXll - Operation")]
        public static string cegOpObjectCallerAddress(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            return OHRepository.Instance.getCallerAddress(objID);
        }

        [ExcelFunction(Description = "Get object creation time", Category = "CEGLibXll - Operation")]
        public static DateTime cegOpObjectCreationTime(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (SystemUtil.CallFromWizard())
                return DateTime.MinValue;

            return OHRepository.Instance.getObjectCreationTime(objID);
        }

        [ExcelFunction(Description = "Get object update time", Category = "CEGLibXll - Operation")]
        public static DateTime cegOpObjectUpdateTime(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (SystemUtil.CallFromWizard())
                return DateTime.MinValue;

            return OHRepository.Instance.getObjectUpdateTime(objID);
        }
    }
}

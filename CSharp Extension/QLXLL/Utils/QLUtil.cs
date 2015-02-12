using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Diagnostics;
using Xl = Microsoft.Office.Interop.Excel;
using ExcelDna.Integration;
using ExcelDna.Integration.CustomUI;
using ExcelDna.Logging;


namespace QLXLL
{
    sealed public class QLUtil
    {
        public static bool CallFromWizardFlag = true;
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
                QLUtil.logError("", "", ex.Message);
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
                QLUtil.logError("", "", ex.Message);
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
                callerAddress = QLUtil.getActiveCellAddress();
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
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), exception_.Message);
                appName = exception_.Message;
            }
            return appName;
        }

        [ExcelFunction(Description = "Display xll version", IsMacroType = true, Category = "QLXLL - Operation")]
        public static string qlVersion()
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            try
            {
                callerAddress = QLUtil.getActiveCellAddress();
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
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), exception_.Message);
                return "";
            }
        }

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

        /*
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

        [ExcelFunction(Description = "List the IDs of objects in repository", Category = "QLXLL - Operation")]
        public static object qlListObjects(
            [ExcelArgument(Description = "pattern ")] string pattern)
        {
            if (QLUtil.CallFromWizard())
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
            if (QLUtil.CallFromWizard())
                return "";

            return OHRepository.Instance.getObjectType(objID).Name;
        }

        [ExcelFunction(Description = "Get object caller address", Category = "QLXLL - Operation")]
        public static string qlObjectCallerAddress(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (QLUtil.CallFromWizard())
                return "";

            return OHRepository.Instance.getCallerAddress(objID);
        }

        [ExcelFunction(Description = "Get object creation time", Category = "QLXLL - Operation")]
        public static DateTime qlObjectCreationTime(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (QLUtil.CallFromWizard())
                return DateTime.MinValue;

            return OHRepository.Instance.getObjectCreationTime(objID);
        }

        [ExcelFunction(Description = "Get object update time", Category = "QLXLL - Operation")]
        public static DateTime qlObjectUpdateTime(
            [ExcelArgument(Description = "id of object ")] string objID)
        {
            if (QLUtil.CallFromWizard())
                return DateTime.MinValue;

            return OHRepository.Instance.getObjectUpdateTime(objID);
        }
        #endregion

        //////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////       Auxiliary functions          ///////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        #region Conversion
        public enum ConversionDefaultLevel
        {
            Strict = 0,
            AllowMissing = 1,
            Quiet = 2
        }

        public static T DefaultValue<T>(Object defaultValue)
        {
            bool isNull = QLUtil.isNull(defaultValue);

            if (!isNull && typeof(T) == defaultValue.GetType())
            {
                return (T)defaultValue;
            }
            else
            {
                if (typeof(T) == typeof(QuantLib.Period))
                {
                    if (isNull) return (T)(Object)(new QuantLib.Period(0, QuantLib.TimeUnit.Days));
                    return (T)(Object)defaultValue;
                }
                else if (typeof(T) == typeof(DateTime))
                {
                    if (isNull) return (T)(Object)(DateTime.MinValue);
                    return (T)(Object)defaultValue;
                }
                else if (typeof(T) == typeof(double[]))
                {
                    if (isNull) return (T)(Object)(new double[0] { });
                    return (T)(Object)(new double[1] { (double)defaultValue });
                }
                else if (typeof(T) == typeof(int[]))
                {
                    if (isNull) return (T)(Object)(new int[0] { });
                    return (T)(Object)(new int[1] { (int)defaultValue });
                }
                else if (typeof(T) == typeof(DateTime[]))
                {
                    if (isNull) return (T)(Object)(new DateTime[0] { });
                    return (T)(Object)(new DateTime[1] { (DateTime)defaultValue });
                }
                else if (typeof(T) == typeof(QuantLib.Period[]))
                {
                    if (isNull) return (T)(Object)(new double[0] { });
                    return (T)(Object)(new QuantLib.Period[1] { (QuantLib.Period)defaultValue });
                }
                else if (typeof(T) == typeof(StringCollection))
                {
                    if (isNull) return (T)(Object)(new StringCollection());
                    StringCollection ret = new StringCollection();
                    ret.Add((String)defaultValue);
                    return (T)(Object)ret;
                }
            }

            throw new Exception("unexpected default value. ");
        }

        public static T ConvertObject<T>(Object obj, String name)
        {
            return ConvertObject<T>(obj, ConversionDefaultLevel.Strict, name, null);
        }

        public static T ConvertObject<T>(Object obj, String name, Object defaultValue)
        {
            return ConvertObject<T>(obj, ConversionDefaultLevel.AllowMissing, name, defaultValue);
        }

        public static T ConvertObject<T>(Object obj, ConversionDefaultLevel level, String name)
        {
            return ConvertObject<T>(obj, level, name, null);
        }

        public static T ConvertObject<T>(Object obj, ConversionDefaultLevel level, String name, Object defaultValue)
        {
            try
            {
                // pass through
                if (!QLUtil.isNull(obj) && obj.GetType() == typeof(T))
                {
                    return (T)(Object)obj;
                }
                if (typeof(T) == typeof(DateTime))          // To DateTime
                {
                    if (obj is QuantLib.Date)
                        return (T)(Object)DateTime.FromOADate(((QuantLib.Date)obj).serialNumber());
                    else if (obj is String)
                        return (T)(Object)DateTime.ParseExact((String)obj, "M/d/yyyy", null);
                    else if (obj is double)
                        return (T)(Object)DateTime.FromOADate((double)obj);
                    else if (obj is object[,])
                        return (T)(Object)QLUtil.ConvertObject2Array(obj, level, name, DateTime.MinValue)[0];
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to System.DateTime");
                }  
                else if (typeof(T) ==  typeof(QuantLib.Date))       // to QuanLib.Date
                {
                    if (obj is DateTime)
                    {
                        QuantLib.Date d = new QuantLib.Date(Convert.ToInt32(((DateTime)obj).ToOADate()));
                        return (T)(Object)d;
                    }
                    else if (obj is double)
                    {
                        QuantLib.Date d = new QuantLib.Date(Convert.ToInt32((double)obj));
                        return (T)(Object)d;
                    }
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to QuantLib.Date");
                        
                }
                else if (typeof(T) == typeof(QuantLib.Calendar))        // to QuantLib.Calendar
                {
                    if (obj is String)
                    {
                        string c = (string)obj;
                        switch (c.ToUpper())
                        {
                            case "NYC":
                                return (T)(Object)(new QuantLib.UnitedStates(QuantLib.UnitedStates.Market.Settlement));
                            case "LON":
                                return (T)(Object)(new QuantLib.UnitedKingdom(QuantLib.UnitedKingdom.Market.Settlement));
                            case "NYC|LON":
                            case "LON|NYC":
                                return (T)(Object)(new QuantLib.JointCalendar(
                                    new QuantLib.UnitedStates(QuantLib.UnitedStates.Market.Settlement),
                                    new QuantLib.UnitedKingdom(QuantLib.UnitedKingdom.Market.Settlement),
                                    QuantLib.JointCalendarRule.JoinBusinessDays));
                            default:
                                throw new ArgumentOutOfRangeException("Financial Center not supported yet.");
                        }
                    }
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to Calendar");
                }
                else if (typeof(T) == typeof(QuantLib.DayCounter))              // to DayCounter
                {
                    if (obj is string)
                    {
                        string c = (string)obj;
                        switch (c.ToUpper())
                        {
                            case "ACTUAL360":
                                return (T)(object)(new QuantLib.Actual360());
                            case "ACTUAL365":
                                return (T)(object)(new QuantLib.Actual365Fixed());
                            case "ACTUALACTUAL":
                                return (T)(object)(new QuantLib.ActualActual());
                            default:
                                throw new ArgumentOutOfRangeException("Day Counter not supported yet.");
                        }
                    }
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to DayCounter");
                }
                else if (typeof(T) == typeof(QuantLib.Period))
                {
                    if (obj is String)      // String (2D, 3M, 10Y)
                    {
                        return (T)(Object)new QuantLib.Period((String)obj);
                    }
                    else if (obj is QuantLib.Frequency)             // Monthly, Annual
                    {
                        string[] freqNames = Enum.GetNames(typeof(QuantLib.Frequency));
                        StringCollection sc = new StringCollection();
                        sc.AddRange(freqNames);
                        if (sc.Contains(obj as string))
                        {
                            QuantLib.Frequency frq = (QuantLib.Frequency)Enum.Parse(typeof(QuantLib.Frequency), obj as string);
                            return (T)(Object)new QuantLib.Period(frq);
                        }
                        else
                            return (T)(Object)new QuantLib.Period((QuantLib.Frequency)obj);
                    }
                    else if (obj is int)
                        return (T)(Object)new QuantLib.Period((int)obj, QuantLib.TimeUnit.Years);
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to QuantLib.Period");
                }
                else if (typeof(T) == typeof(DateTime[]))
                {
                    if (obj is object[,] || obj is DateTime || obj is double)
                        return (T)(Object)QLUtil.ConvertObject2Array(obj, level, name, DateTime.MinValue);
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to DateTime[]");
                }
                else if (typeof(T) == typeof(QuantLib.Period[]))
                {
                    if (obj is object[,] || obj is String)
                        return (T)(Object)QLUtil.ConvertObject2Array(obj, level, name, new QuantLib.Period());
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to QuantLib.Period[]");
                }
                else if (typeof(T) == typeof(double))
                {
                    if (obj is String)
                    {
                        double amount = 0.0;
                        double.TryParse((String)obj, out amount);
                        return (T)(Object)amount;
                    }
                }
                else if (typeof(T) == typeof(double[]))
                {
                    if (obj is object[,] || obj is double)
                    {
                        double def = (defaultValue is double) ? (double)defaultValue : 0.0;
                        return (T)(Object)QLUtil.ConvertObject2Array<double>(obj, def);
                    }
                }
                else if (typeof(T).IsEnum)
                {
                    if (obj is String)
                    {
                        if (QLUtil.EnumDictionary.ContainsKey(obj as string))
                            obj = QLUtil.EnumDictionary[obj as string];
                        return (T)Enum.Parse(typeof(T), (string)obj, true);
                    }
                }
                else if (typeof(T) == typeof(String[]))
                {
                    if (obj is object[,] || obj is string)
                    {
                        StringCollection tmp = QLUtil.ConvertObject2StringCollection(obj);
                        String[] ret = new String[tmp.Count];
                        tmp.CopyTo(ret, 0);
                        return (T)(Object)ret;
                    }
                }
                else if (typeof(T) == typeof(StringCollection))
                {
                    if (obj is object[,] || obj is string || obj is double)
                        return (T)(Object)QLUtil.ConvertObject2StringCollection(obj);
                }
                else if (typeof(T) == typeof(bool[]))
                {
                    if (obj is object[,] || obj is bool)
                    {
                        bool def = (defaultValue is bool) ? (bool)defaultValue : false;
                        return (T)(Object)QLUtil.ConvertObject2Array<bool>(obj, def);
                    }
                }
                else if (typeof(T) == typeof(int[]))
                {
                    if (obj is object[,] || obj is double)
                    {
                        double def = (defaultValue is int) ? Convert.ToDouble(defaultValue) : 0.0;
                        double[] tmp = QLUtil.ConvertObject2Array<double>(obj, def);
                        int[] ret = new int[tmp.Length];
                        for (int i = 0; i < ret.Length; i++) ret[i] = (int)tmp[i];
                        return (T)(Object)ret;
                    }
                }
            }
            catch (System.Exception e)
            {
                if (level == ConversionDefaultLevel.Quiet)
                    return DefaultValue<T>(defaultValue);
                else
                    throw new Exception(name + " must be valid -- . " + e.Message);
            }
            if (level == ConversionDefaultLevel.Strict)
            {
                throw new Exception(name + " type is invalid. ");
            }
            else if (!(QLUtil.isNull(obj)) && (level == ConversionDefaultLevel.AllowMissing))
            {
                throw new Exception(name + " is invalid. ");
            }
            return DefaultValue<T>(defaultValue);
        }

        public static StringCollection ConvertObject2StringCollection(Object obj0)
        {
            if (QLUtil.isNull(obj0))
                throw new Exception("string list object is null. ");

            StringCollection result = new StringCollection();
            if (obj0 is string)
            {
                result.Add((string)obj0);
                return result;
            }
            else if (obj0 is double)
            {
                result.Add(System.Convert.ToString(System.Convert.ToInt32((double)obj0)));
                return result;
            }
            Object[,] obj = (Object[,])obj0;
            int rows = obj.GetLength(0);
            int cols = obj.GetLength(1);
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    if (QLUtil.isError(obj[i, j]))
                    {
                        throw new Exception("Invalid string list inputs -- please verify. ");
                    }
                    if (obj[i, j] is string)
                    {
                        result.Add((String)obj[i, j]);
                    }
                    else if (obj[i, j] is double)
                    {
                        result.Add(System.Convert.ToString(System.Convert.ToInt32((double)obj[i, j])));
                    }
                }
            }

            return result;
        }

        public static T[] ConvertObject2Array<T>(object arg, ConversionDefaultLevel level, string name, T def_value) where T : new()
        {
            try
            {
                if (arg is int || arg is double || arg is string || arg is T)
                {
                    T[] ret = new T[1];
                    ret[0] = QLUtil.ConvertObject<T>(arg, level, name, def_value);
                    return ret;
                }
                else if (arg is object[,])
                {
                    int row = ((object[,])arg).GetLength(0);
                    int col = ((object[,])arg).GetLength(1);

                    List<T> list = new List<T>();
                    for (int i = 0; i < row; i++)
                    {
                        for (int j = 0; j < col; j++)
                        {
                            T t = QLUtil.ConvertObject<T>(((object[,])arg)[i, j], level, name, def_value);
                            if (!t.Equals(def_value)) list.Add(t);
                        }
                    }
                    T[] ret = new T[list.Count];
                    for (int i = 0; i < ret.Length; i++)
                        ret[i] = list[i];

                    return ret;
                }
            }
            catch (Exception e)
            {
                throw e;
            }

            T[] retArr = new T[1];
            retArr[0] = def_value;
            return retArr;
        }

        public static T[] ConvertObject2Array<T>(object arg, T p)
        {
            try
            {
                if (arg is int || arg is double || arg is T)
                {
                    T[] ret = new T[1];
                    ret[0] = (T)arg;
                    return ret;
                }
                else if (arg is object[,])
                {
                    int row = ((object[,])arg).GetLength(0);
                    int col = ((object[,])arg).GetLength(1);

                    T[] ret = new T[row * col];
                    for (int i = 0; i < row; i++)
                    {
                        for (int j = 0; j < col; j++)
                        {
                            ret[i * col + j] = (T)((object[,])arg).GetValue(i, j);
                        }
                    }
                    return ret;
                }
            }
            catch (Exception e)
            {
                throw e;
            }

            T[] retArr = new T[1];
            retArr[0] = p;
            return retArr;
        }

        public double[,] ConvertObject2Matrix(object arg)
        {
            try
            {
                if (arg is int || arg is double)
                {
                    double[,] ret = new double[1, 1];
                    ret[0, 0] = (double)arg;
                    return ret;
                }
                else if (arg is object[,])
                {
                    int row = ((object[,])arg).GetLength(0);
                    int col = ((object[,])arg).GetLength(1);

                    double[,] ret = new double[row, col];
                    for (int i = 0; i < row; i++)
                    {
                        for (int j = 0; j < col; j++)
                        {
                            ret[i, j] = (double)((object[,])arg).GetValue(i, j);
                        }
                    }
                    return ret;
                }
            }
            catch (Exception e)
            {
                throw e;
            }

            double[,] retarr = new double[1, 1];
            return retarr;
        }

        public double[,] ConvertObject2Matrix(object arg, int row_offset)
        {
            try
            {
                if (arg is int || arg is double)
                {
                    double[,] ret = new double[1, 1];
                    ret[0, 0] = (double)arg;
                    return ret;
                }
                else if (arg is object[,])
                {
                    int row = ((object[,])arg).GetLength(0);
                    int col = ((object[,])arg).GetLength(1);

                    double[,] ret = new double[row - row_offset, col];
                    for (int i = 0; i < row - row_offset; i++)
                    {
                        for (int j = 0; j < col; j++)
                        {
                            if (((object[,])arg).GetValue(i + row_offset, j).GetType() == typeof(DateTime))
                            {
                                DateTime d = (DateTime)((object[,])arg).GetValue(i + row_offset, j);
                                ret[i, j] = d.ToOADate();
                            }
                            else
                            {
                                ret[i, j] = (double)((object[,])arg).GetValue(i + row_offset, j);
                            }
                        }
                    }
                    return ret;
                }
            }
            catch (Exception e)
            {
                throw e;
            }

            double[,] retarr = new double[1, 1];
            return retarr;
        }

        public double[][] Convert2Matrix(double[,] v)
        {
            double[][] ret = new double[v.GetLength(0)][];
            for (int i = 0; i < v.GetLength(0); i++)
            {
                ret[i] = new double[v.GetLength(1)];
                for (int j = 0; j < v.GetLength(1); j++)
                    ret[i][j] = v[i, j];
            }
            return ret;
        }

        public Object[][] Convert2ObjectMatrix(Object[,] v, bool checkError)
        {
            Object[][] ret = new Object[v.GetLength(0)][];
            for (int i = 0; i < v.GetLength(0); i++)
            {
                ret[i] = new Object[v.GetLength(1)];
                for (int j = 0; j < v.GetLength(1); j++)
                {
                    if (checkError && QLUtil.isError(v[i, j]))
                        throw new Exception("Matrix contains error element(s)");
                    ret[i][j] = v[i, j];
                }
            }
            return ret;
        }

        public Object[][] Convert2ObjectMatrixTransposed(Object[,] v, bool checkError)
        {
            Object[][] ret = new Object[v.GetLength(1)][];
            for (int i = 0; i < v.GetLength(1); i++)
            {
                ret[i] = new Object[v.GetLength(0)];
                for (int j = 0; j < v.GetLength(0); j++)
                {
                    if (checkError && QLUtil.isError(v[j, i]))
                        throw new Exception("Matrix contains error element(s)");
                    ret[i][j] = v[j, i];
                }
            }
            return ret;
        }

        public Object Transpose<T>(T[] arr)
        {
            if (arr == null) return null;

            var ret = new T[arr.Length, 1];
            for (int i = 0; i < arr.Length; i++)
                ret[i, 0] = arr[i];
            return ret;
        }

        public Dictionary<String, String> Convert2Dictionary(StringCollection key, StringCollection value)
        {
            if (key.Count != value.Count)
                throw new Exception("key & value pair size mismatching....");

            Dictionary<String, String> ret = new Dictionary<string, string>();
            for (int i = 0; i < key.Count; i++)
                ret.Add(key[i], value[i]);
            return ret;
        }
        #endregion

        #region ExcelDNA related functions
        public static bool isNull(Object o)
        {
            if (o == null || o is ExcelMissing || o is ExcelEmpty || o is ExcelError)
                return true;
            if (o is string && (o as string == string.Empty || o as string == ""))
                return true;
            return false;
        }

        public static bool isError(Object o)
        {
            return (o is ExcelError || (o is String && ((String)o == "#QL_ERR!" || (String)o == "#QL_TRG!")));
        }

        public static Xl.Range getActiveCellRange()
        {
            Xl.Application xlApp = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            Xl.Range range;
            try
            {
                range = (Xl.Range)xlApp.get_Caller(System.Type.Missing);
            }
            catch (Exception)
            {
                range = xlApp.ActiveCell;
            }
            return range;
        }

        public static ExcelReference getActiveReference()
        {
            ExcelReference reference = null;

            // first check if calling from Excel
            Xl.Application xlApp = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            Xl.Range range;
            try
            {
                range = (Xl.Range)xlApp.get_Caller(System.Type.Missing);
            }
            catch (System.Exception)
            {
                return null;
            }
            try
            {
                reference = (ExcelReference)XlCall.Excel(XlCall.xlfCaller);
            }
            catch (System.Exception)
            {
                reference = (ExcelReference)XlCall.Excel(XlCall.xlfSelection);
            }
            return reference;
        }

        public static string getActiveCellAddressLocal(ExcelReference reference)
        {
            string cellReference = (string)XlCall.Excel(XlCall.xlfAddress, 1 + reference.RowFirst, 1 + reference.ColumnFirst);
            if (reference.RowFirst != reference.RowLast || reference.ColumnFirst != reference.ColumnLast)
            {
                string bottomRight = (string)XlCall.Excel(XlCall.xlfAddress, 1 + reference.RowLast, 1 + reference.ColumnLast);
                cellReference += ":" + bottomRight;
            }
            return cellReference;
        }

        public static string getActiveCellAddressLocal()
        {
            return getActiveCellAddressLocal(getActiveReference());
        }

        public static string getActiveCellAddress()
        {
            if (QLUtil.CallerAddressFlag == false)
                return "getActiveCellAddress";

            ExcelReference reference = getActiveReference();
            if (isNull(reference))
                return "getActiveCellAddress";

            string sheetName = (string)XlCall.Excel(XlCall.xlSheetNm, reference);
            return sheetName + "!" + getActiveCellAddressLocal(reference);
        }

        public static string getSelectCellAddress()
        {
            ExcelReference reference = (ExcelReference)XlCall.Excel(XlCall.xlfSelection);
            string sheetName = (string)XlCall.Excel(XlCall.xlSheetNm, reference);

            return sheetName + "!" + getActiveCellAddressLocal(reference);
        }

        public static string getAddress(Xl.Range range)
        {
            int c = range.Column;
            int r = range.Row;
            Microsoft.Office.Interop.Excel.Worksheet sheet = (Microsoft.Office.Interop.Excel.Worksheet)range.Parent;
            string s = sheet.Name;
            string ps = ((Microsoft.Office.Interop.Excel.Workbook)sheet.Parent).Name;

            return string.Format("[{0}]{1}${2}${3}", ps, s, ExcelColumnIndexToName(c), r);
        }

        public static string ExcelColumnIndexToName(int columnNumber)
        {
            int dividend = columnNumber;
            string columnName = string.Empty;
            int modulo;

            while (dividend > 0)
            {
                modulo = (dividend - 1) % 26;
                columnName = Convert.ToChar(65 + modulo).ToString() + columnName;
                dividend = (int)((dividend - modulo) / 26);
            }

            return columnName;
        }

        public static Xl.Range ConvertExcelReferenceToRange(ExcelReference excelRef)
        {
            Xl.Application app = (Xl.Application)ExcelDnaUtil.Application;
            string sheetFullName = (string)XlCall.Excel(XlCall.xlSheetNm, excelRef);   // something like [book1]sheet1, we need to remove [book1]
            string sheetName = sheetFullName.Substring(sheetFullName.IndexOf(']') + 1);
            Xl.Worksheet sheet = app.Worksheets[sheetName];
            string topAddress = ExcelColumnIndexToName(excelRef.ColumnFirst + 1) + (excelRef.RowFirst + 1);
            string bottomAddress = ExcelColumnIndexToName(excelRef.ColumnLast + 1) + (excelRef.RowLast + 1);
            return sheet.get_Range(topAddress, bottomAddress);
        }

        public static Type GetTypeForComObject(Object comObject)
        {
            IntPtr iunknown = System.Runtime.InteropServices.Marshal.GetIUnknownForObject(comObject);
            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetAssembly(typeof(Xl.Range));

            Type[] excelTypes = assembly.GetTypes();

            foreach (Type currType in excelTypes)
            {
                Guid iid = currType.GUID;
                if (!currType.IsInterface || iid == Guid.Empty)
                    continue;

                IntPtr ipointer = IntPtr.Zero;
                System.Runtime.InteropServices.Marshal.QueryInterface(iunknown, ref iid, out ipointer);
                if (ipointer != IntPtr.Zero)
                    return currType;
            }

            return null;
        }

        public static bool TestExcelType(Object obj)
        {
            if (!(obj is object[,]))
            {
                if (obj is ExcelError || QLUtil.isQLError(obj))
                    return false;
            }
            else
            {
                foreach (Object o in (object[,])obj)
                {
                    if (o is ExcelError || QLUtil.isQLError(o))
                        return false;
                }
            }

            return true;
        }

        public static void logError(string callerAddress, string callerName, string Message)
        {
            OHRepository.Instance.storeErrorMsg(callerAddress, Message);
            LogDisplay.RecordLine("{0}\t{1}\t{2}", callerAddress, callerName, Message);
        }
        #endregion

        #region Version functions
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

        public static bool isQLError(string v)
        {
            if (v == "#QL_ERR!" || v == "#QL_TRG!")
                return true;
            else
                return false;
        }

        public static bool isQLError(Object obj)
        {
            try
            {
                string v = (string)obj;
                return isQLError(v);
            }
            catch (Exception)
            {
                return false;
            }
        }
        #endregion


        #region Call from Wizard
        public delegate bool WindowEnumDelegate(IntPtr hwnd, IntPtr lParam);

        // declare the API function the enumerate child windows
        [DllImport("user32.dll")]
        public static extern int EnumChildWindows(IntPtr hwnd, WindowEnumDelegate del, IntPtr lParma);

        [DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern int GetClassName(IntPtr hwnd, StringBuilder lpClassName, int nMaxCount);

        [DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern int GetWindowThreadProcessId(IntPtr hwnd, out ulong lpdwProcessId);

        public static bool WindowEnumProc(IntPtr hwnd, IntPtr lParam)
        {
            int nRet;
            StringBuilder ClassName = new StringBuilder(100);
            // Get the window class name
            nRet = GetClassName(hwnd, ClassName, ClassName.Capacity);
            if (nRet == 0)
                return true;

            if (string.Compare(ClassName.ToString(), 0, "bosa_sdm_xl", 0, 11, true, System.Globalization.CultureInfo.InvariantCulture) == 0)
            {
                GCHandle gch = GCHandle.FromIntPtr(lParam);
                List<IntPtr> list = gch.Target as List<IntPtr>;
                if (list == null)
                {
                    throw new InvalidCastException("GCHandle Target could not be cast as List<IntPtr>");
                }

                ulong lpdwProcessId;
                GetWindowThreadProcessId(hwnd, out lpdwProcessId);
                list.Add(new IntPtr((long)lpdwProcessId));
                return true;
            }

            return true;
        }

        public static bool CallFromWizard()
        {
            if (QLUtil.CallFromWizardFlag == false)
                return false;

            Process currentProcess = Process.GetCurrentProcess();
            IntPtr currPtr = new IntPtr(currentProcess.Id);

            List<IntPtr> result = new List<IntPtr>();
            GCHandle listHandle = GCHandle.Alloc(result);

            try
            {
                QLUtil.WindowEnumDelegate childProc = new QLUtil.WindowEnumDelegate(QLUtil.WindowEnumProc);
                QLUtil.EnumChildWindows(IntPtr.Zero, childProc, GCHandle.ToIntPtr(listHandle));

                foreach (IntPtr ptr in result)
                {
                    if (ptr == currPtr)
                        return true;
                }
            }
            finally
            {
                if (listHandle.IsAllocated)
                    listHandle.Free();
            }
            return false;
        }
        #endregion
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////    Supporting Classes             ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    #region Supporting classes
    class FunctionWizardThread
    {
        private Xl.Range range_ = null;
        private System.Threading.SynchronizationContext xlcontext_ = null;

        public FunctionWizardThread(Xl.Range range, System.Threading.SynchronizationContext xlcontext)
        {
            range_ = range;
            xlcontext_ = xlcontext;
        }

        public void functionargumentsPopup()
        {
            xlcontext_.Post(
                delegate(object state)
                {
                    range_.FunctionWizard();
                }, null);
        }
    }

    class WinFucntion
    {
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SendMessageTimeout(
            IntPtr hWnd,
            int Msg,
            int wParam,
            string lParam,
            int fuFlags,
            int uTimeout,
            out int lpdwResult
        );

        public const int HWND_BROADCAST = 0xffff;
        public const int WM_SETTINGCHAGE = 0x001A;
        public const int SMTO_NORMAL = 0x0000;
        public const int SMTO_BLOCK = 0x0001;
        public const int SMTO_ABORTIFHUNG = 0x0002;
        public const int SMTO_NOTIMEOUTIFNOTHUNG = 0x0008;

        public static void BroadcastEnvironment()
        {
            int result;
            SendMessageTimeout((IntPtr)HWND_BROADCAST, WM_SETTINGCHAGE, 0, "Environment",
                SMTO_BLOCK | SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG, 5000, out result);
        }
    }
    #endregion
}

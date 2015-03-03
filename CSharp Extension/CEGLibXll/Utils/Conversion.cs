using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CEGLibXll
{
    sealed public class Conversion
    {
        public enum ConversionDefaultLevel
        {
            Strict = 0,
            AllowMissing = 1,
            Quiet = 2
        }

        public static T DefaultValue<T>(Object defaultValue)
        {
            bool isNull = SystemUtil.isNull(defaultValue);

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
                if (!SystemUtil.isNull(obj) && obj.GetType() == typeof(T))
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
                        return (T)(Object)Conversion.ConvertObject2Array(obj, level, name, DateTime.MinValue)[0];
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to System.DateTime");
                }
                else if (typeof(T) == typeof(QuantLib.Date))       // to QuanLib.Date
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
                                throw new ArgumentOutOfRangeException("Unknown Financial Center.");
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
                                throw new ArgumentOutOfRangeException("Unknow day counter.");
                        }
                    }
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to DayCounter");
                }
                else if (typeof(T) == typeof(QuantLib.BusinessDayConvention))              // to bdc
                {
                    if (obj is string)
                    {
                        string c = (string)obj;
                        switch (c.ToUpper())
                        {
                            case "F":
                                return (T)(object)(QuantLib.BusinessDayConvention.Following);
                            case "MF":
                                return (T)(object)(QuantLib.BusinessDayConvention.ModifiedFollowing);
                            case "P":
                                return (T)(object)(QuantLib.BusinessDayConvention.Preceding);
                            case "MP":
                                return (T)(object)(QuantLib.BusinessDayConvention.ModifiedPreceding);
                            case "NONE":
                                return (T)(object)(QuantLib.BusinessDayConvention.Unadjusted);
                            default:
                                throw new ArgumentOutOfRangeException("unknow business day convention.");
                        }
                    }
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to BusinessDayConvention");
                }
                else if (typeof(T) == typeof(QuantLib.DateGeneration.Rule))              // to rule
                {
                    if (obj is string)
                    {
                        string c = (string)obj;
                        switch (c.ToUpper())
                        {
                            case "BACKWARD":
                                return (T)(object)(QuantLib.DateGeneration.Rule.Backward);
                            case "FORWARD":
                                return (T)(object)(QuantLib.DateGeneration.Rule.Forward);
                            case "ZERO":
                                return (T)(object)(QuantLib.DateGeneration.Rule.Zero);
                            case "THIRDWEDNESDAY":
                                return (T)(object)(QuantLib.DateGeneration.Rule.ThirdWednesday);
                            case "TWENTIETH":
                                return (T)(object)(QuantLib.DateGeneration.Rule.Twentieth);
                            case "TWENTIETHIMM":
                                return (T)(object)(QuantLib.DateGeneration.Rule.TwentiethIMM);
                            case "CDS":
                                return (T)(object)(QuantLib.DateGeneration.Rule.CDS);
                            default:
                                throw new ArgumentOutOfRangeException("unknow date generation rule.");
                        }
                    }
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to DateGenerationRule");
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
                        return (T)(Object)Conversion.ConvertObject2Array(obj, level, name, DateTime.MinValue);
                    else
                        throw new ArgumentOutOfRangeException("Unknown type to convert to DateTime[]");
                }
                else if (typeof(T) == typeof(QuantLib.Period[]))
                {
                    if (obj is object[,] || obj is String)
                        return (T)(Object)Conversion.ConvertObject2Array(obj, level, name, new QuantLib.Period());
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
                        return (T)(Object)Conversion.ConvertObject2Array<double>(obj, def);
                    }
                }
                else if (typeof(T).IsEnum)
                {
                    if (obj is String)
                    {
                        if (Util.EnumDictionary.ContainsKey(obj as string))
                            obj = Util.EnumDictionary[obj as string];
                        return (T)Enum.Parse(typeof(T), (string)obj, true);
                    }
                }
                else if (typeof(T) == typeof(String[]))
                {
                    if (obj is object[,] || obj is string)
                    {
                        StringCollection tmp = Conversion.ConvertObject2StringCollection(obj);
                        String[] ret = new String[tmp.Count];
                        tmp.CopyTo(ret, 0);
                        return (T)(Object)ret;
                    }
                }
                else if (typeof(T) == typeof(StringCollection))
                {
                    if (obj is object[,] || obj is string || obj is double)
                        return (T)(Object)Conversion.ConvertObject2StringCollection(obj);
                }
                else if (typeof(T) == typeof(bool[]))
                {
                    if (obj is object[,] || obj is bool)
                    {
                        bool def = (defaultValue is bool) ? (bool)defaultValue : false;
                        return (T)(Object)Conversion.ConvertObject2Array<bool>(obj, def);
                    }
                }
                else if (typeof(T) == typeof(int[]))
                {
                    if (obj is object[,] || obj is double)
                    {
                        double def = (defaultValue is int) ? Convert.ToDouble(defaultValue) : 0.0;
                        double[] tmp = Conversion.ConvertObject2Array<double>(obj, def);
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
            else if (!(SystemUtil.isNull(obj)) && (level == ConversionDefaultLevel.AllowMissing))
            {
                throw new Exception(name + " is invalid. ");
            }
            return DefaultValue<T>(defaultValue);
        }

        public static StringCollection ConvertObject2StringCollection(Object obj0)
        {
            if (SystemUtil.isNull(obj0))
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
                    if (SystemUtil.isError(obj[i, j]))
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
                    ret[0] = Conversion.ConvertObject<T>(arg, level, name, def_value);
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
                            T t = Conversion.ConvertObject<T>(((object[,])arg)[i, j], level, name, def_value);
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
                    if (checkError && SystemUtil.isError(v[i, j]))
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
                    if (checkError && SystemUtil.isError(v[j, i]))
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
    }
}

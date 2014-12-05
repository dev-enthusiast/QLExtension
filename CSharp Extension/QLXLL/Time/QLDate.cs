using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ExcelDna.Integration;
using ExcelDna.Integration.Rtd;
using Xl = Microsoft.Office.Interop.Excel;

namespace QLXLL
{
    public class qlDate
    {
        [ExcelFunction(Description = "RTD time", Category = "QLXLL - Time")]
        public static Object qlNow(
            [ExcelArgument(Description = "(INT)interval in seconds ")]double interval)
        {
            string[] param = { interval.ToString(), "NOW" };
            object ret = XlCall.RTD("QLXLL.RTDTimerServer", null, param);
            return new object[,] { { ret } };
        }

        [ExcelFunction(Description = "today's date (non volatile function)", Category = "QLXLL - Time")]
        public static Object qlToday(bool withTime)
        {
            if (withTime)
                return DateTime.Now;
            else
                return DateTime.Today;
        }

        [ExcelFunction(Description = "check if the given date is a business day", Category = "QLXLL - Time")]
        public static bool qlIsBusinessDay(
            [ExcelArgument(Description = "Date ")]DateTime date,
            [ExcelArgument(Description = "Calendar ")]string calendar)
        {
            if (QLUtil.CallFromWizard())
                return false;

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
                if (date == DateTime.MinValue)
                    throw new Exception("Date must not be empty. ");

                QuantLib.Calendar can = QLUtil.ConvertObject<QuantLib.Calendar>(calendar,"calendar");
                QuantLib.Date d = QLUtil.ConvertObject<QuantLib.Date>(date,"date");
                
                return can.isBusinessDay(d);
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return false;
            }
        }
    }
}

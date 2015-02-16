using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ExcelDna.Integration;
using ExcelDna.Integration.Rtd;
using Xl = Microsoft.Office.Interop.Excel;
using QuantLib;

namespace CEGLibXll
{
    public class cegDate
    {
        [ExcelFunction(Description = "RTD time", Category = "CEGLibXll - Time")]
        public static Object cegNow(
            [ExcelArgument(Description = "(INT)interval in seconds ")]double interval)
        {
            string[] param = { interval.ToString(), "NOW" };
            object ret = XlCall.RTD("CEGLibXll.RTDTimerServer", null, param);
            return new object[,] { { ret } };
        }

        [ExcelFunction(Description = "today's date (non volatile function)", Category = "CEGLibXll - Time")]
        public static Object cegToday(bool withTime)
        {
            if (withTime)
                return DateTime.Now;
            else
                return DateTime.Today;
        }

        [ExcelFunction(Description = "set the evaluation date of the whole spreadsheet", Category = "CEGLibXll - Time")]
        public static object cegSetEvaluationDate(
            [ExcelArgument(Description = "Evaluation Date ")]DateTime date)
        {
            if (QLUtil.CallFromWizard())
                return false;

            string callerAddress = "";
            callerAddress = QLUtil.getActiveCellAddress();

            QuantLib.Date todaysDate; 
            try
            {
                if (date == DateTime.MinValue)
                    todaysDate = QLUtil.ConvertObject<QuantLib.Date>(DateTime.Today,"date");
                else
                    todaysDate = QLUtil.ConvertObject<QuantLib.Date>(date,"date");

                QuantLib.Settings.instance().setEvaluationDate(todaysDate);

                return QLUtil.ConvertObject<DateTime>(todaysDate, "date");
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return false;
            }
        }

        [ExcelFunction(Description = "calculate year fraction between two dates", Category = "CEGLibXll - Time")]
        public static object cegYearFraction(
            [ExcelArgument(Description = "Start Date ")]DateTime date1,
            [ExcelArgument(Description = "End Date ")]DateTime date2,
            [ExcelArgument(Description = "Day Counter ")]string dc)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = QLUtil.getActiveCellAddress();
            OHRepository.Instance.removeErrorMessage(callerAddress);

            try
            {
                if ((date1 == DateTime.MinValue) || (date2 == DateTime.MinValue))
                    throw new Exception("Date must not be empty. ");

                Date start = QLUtil.ConvertObject<Date>(date1, "date");
                Date end = QLUtil.ConvertObject<Date>(date2, "date");
                DayCounter daycounter = QLUtil.ConvertObject<DayCounter>(dc, "daycounter");
                return daycounter.yearFraction(start, end);
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }

        [ExcelFunction(Description = "check if the given date is a business day", Category = "CEGLibXll - Time")]
        public static bool cegIsBusinessDay(
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

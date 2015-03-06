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
        public static Object cegTimeNow(
            [ExcelArgument(Description = "(INT)interval in seconds ")]double interval)
        {
            string[] param = { interval.ToString(), "NOW" };
            object ret = XlCall.RTD("CEGLibXll.RTDTimerServer", null, param);
            return new object[,] { { ret } };
        }

        [ExcelFunction(Description = "today's date (non volatile function)", Category = "CEGLibXll - Time")]
        public static Object cegTimeToday(bool withTime)
        {
            if (withTime)
                return DateTime.Now;
            else
                return DateTime.Today;
        }

        [ExcelFunction(Description = "set the evaluation date of the whole spreadsheet", Category = "CEGLibXll - Time")]
        public static object cegTimeSetEvaluationDate(
            [ExcelArgument(Description = "Evaluation Date ")]DateTime date)
        {
            if (SystemUtil.CallFromWizard())
                return false;

            string callerAddress = "";
            callerAddress = SystemUtil.getActiveCellAddress();

            QuantLib.Date todaysDate; 
            try
            {
                if (date == DateTime.MinValue)
                    todaysDate = Conversion.ConvertObject<QuantLib.Date>(DateTime.Today,"date");
                else
                    todaysDate = Conversion.ConvertObject<QuantLib.Date>(date, "date");

                QuantLib.Settings.instance().setEvaluationDate(todaysDate);

                return Conversion.ConvertObject<DateTime>(todaysDate, "date");
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return false;
            }
        }

        [ExcelFunction(Description = "calculate year fraction between two dates", Category = "CEGLibXll - Time")]
        public static object cegTimeYearFraction(
            [ExcelArgument(Description = "Start Date ")]DateTime date1,
            [ExcelArgument(Description = "End Date ")]DateTime date2,
            [ExcelArgument(Description = "Day Counter (default ActualActual) ")]string dc = "ActualActual")
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = SystemUtil.getActiveCellAddress();
            OHRepository.Instance.removeErrorMessage(callerAddress);

            try
            {
                if ((date1 == DateTime.MinValue) || (date2 == DateTime.MinValue))
                    throw new Exception("Date must not be empty. ");

                Date start = Conversion.ConvertObject<Date>(date1, "date");
                Date end = Conversion.ConvertObject<Date>(date2, "date");
                DayCounter daycounter = Conversion.ConvertObject<DayCounter>(dc, "daycounter");
                return daycounter.yearFraction(start, end);
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }

        [ExcelFunction(Description = "business days between two dates (doesn't include these two days)", Category = "CEGLibXll - Time")]
        public static object cegTimeBusinessDaysBetween(
            [ExcelArgument(Description = "Start Date ")]DateTime date1,
            [ExcelArgument(Description = "End Date ")]DateTime date2,
            [ExcelArgument(Description = "Calendar (default NYC) ")]string calendar)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = SystemUtil.getActiveCellAddress();
            OHRepository.Instance.removeErrorMessage(callerAddress);

            try
            {
                if ((date1 == DateTime.MinValue) || (date2 == DateTime.MinValue))
                    throw new Exception("Date must not be empty. ");
                Date start = Conversion.ConvertObject<Date>(date1, "date");
                Date end = Conversion.ConvertObject<Date>(date2, "date");

                if (string.IsNullOrEmpty(calendar)) calendar = "NYC";
                QuantLib.Calendar can = Conversion.ConvertObject<QuantLib.Calendar>(calendar, "calendar");
                
                return can.businessDaysBetween(start, end, false, false);
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }

        [ExcelFunction(Description = "check if the given date is a business day", Category = "CEGLibXll - Time")]
        public static bool cegTimeIsBusinessDay(
            [ExcelArgument(Description = "Date ")]DateTime date,
            [ExcelArgument(Description = "Calendar (default NYC) ")]string calendar)
        {
            if (SystemUtil.CallFromWizard())
                return false;

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
                if (date == DateTime.MinValue)
                    throw new Exception("Date must not be empty. ");
                QuantLib.Date d = Conversion.ConvertObject<QuantLib.Date>(date, "date");

                if (string.IsNullOrEmpty(calendar)) calendar = "NYC";
                QuantLib.Calendar can = Conversion.ConvertObject<QuantLib.Calendar>(calendar, "calendar");
                
                return can.isBusinessDay(d);
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return false;
            }
        }

        [ExcelFunction(Description = "adjust a date to business day", Category = "CEGLibXll - Time")]
        public static object cegTimeAdjustDate(
            [ExcelArgument(Description = "Date ")]DateTime date,
            [ExcelArgument(Description = "Calendar (default NYC) ")]string calendar,
            [ExcelArgument(Description = "BusinessDayConvention (default ModifiedFollowing) ")]string bdc)
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
                if (date == DateTime.MinValue)
                    throw new Exception("Date must not be empty. ");
                QuantLib.Date d = Conversion.ConvertObject<QuantLib.Date>(date, "date");

                if (string.IsNullOrEmpty(calendar)) calendar = "NYC";
                QuantLib.Calendar can = Conversion.ConvertObject<QuantLib.Calendar>(calendar, "calendar");

                if (string.IsNullOrEmpty(bdc)) bdc = "MF";                
                BusinessDayConvention bdc2 = Conversion.ConvertObject<BusinessDayConvention>(bdc, "businessdayconvention");

                Date newday = can.adjust(d, bdc2);
                return newday.serialNumber();
            }
            catch(TypeInitializationException e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                object[,] ret = new object[5,1];
                ret[0,1] = e.ToString();
                ret[1,1] = e.Message;
                ret[2,1] = e.StackTrace;
                ret[3,3] = e.Source;
                ret[4,1] = e.InnerException.Message;
                return ret;
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return e.Message;
            }
        }

        [ExcelFunction(Description = "advance forward a date acording to tenor given", Category = "CEGLibXll - Time")]
        public static object cegTimeAdvanceDate(
            [ExcelArgument(Description = "Date ")]DateTime date,
            [ExcelArgument(Description = "Calendar (default NYC) ")]string calendar,
            [ExcelArgument(Description = "Tenor (e.g. '3D' or '2Y') ")]string tenor,
            [ExcelArgument(Description = "BusinessDayConvention (default ModifiedFollowing) ")]string bdc,
            [ExcelArgument(Description = "is endofmonth ")]bool eom)
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
                if (date == DateTime.MinValue)
                    throw new Exception("Date must not be empty. ");
                QuantLib.Date d = Conversion.ConvertObject<QuantLib.Date>(date, "date");

                if (string.IsNullOrEmpty(calendar)) calendar = "NYC";
                QuantLib.Calendar can = Conversion.ConvertObject<QuantLib.Calendar>(calendar, "calendar");

                if (string.IsNullOrEmpty(tenor))
                    tenor = "1D";
                QuantLib.Period period = Conversion.ConvertObject<QuantLib.Period>(tenor, "period");

                if (string.IsNullOrEmpty(bdc)) bdc = "MF";
                BusinessDayConvention bdc2 = Conversion.ConvertObject<BusinessDayConvention>(bdc, "businessdayconvention");

                Date newday = can.advance(d, period, bdc2, eom);
                return newday.serialNumber();
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return e.Message;
            }
        }

        [ExcelFunction(Description = "time schedule betwwen two days according to tenor", Category = "CEGLibXll - Time")]
        public static object cegTimeSchedule(
            [ExcelArgument(Description = "Start Date ")]DateTime date1,
            [ExcelArgument(Description = "End Date ")]DateTime date2,
            [ExcelArgument(Description = "Tenor (e.g. '3D' or '2Y') ")]string tenor,
            [ExcelArgument(Description = "Calendar (default NYC) ")]string calendar,
            [ExcelArgument(Description = "BusinessDayConvention (default ModifiedFollowing) ")]string bdc,
            [ExcelArgument(Description = "DateGenerationRule (default Backward) ")]string rule,
            [ExcelArgument(Description = "is endofmonth ")]bool eom)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = SystemUtil.getActiveCellAddress();
            OHRepository.Instance.removeErrorMessage(callerAddress);

            try
            {
                if ((date1 == DateTime.MinValue) || (date2 == DateTime.MinValue))
                    throw new Exception("Date must not be empty. ");
                Date start = Conversion.ConvertObject<Date>(date1, "date");
                Date end = Conversion.ConvertObject<Date>(date2, "date");

                QuantLib.Period period = Conversion.ConvertObject<QuantLib.Period>(tenor, "period");

                if (string.IsNullOrEmpty(calendar)) calendar = "NYC";
                QuantLib.Calendar can = Conversion.ConvertObject<QuantLib.Calendar>(calendar, "calendar");

                if (string.IsNullOrEmpty(bdc)) bdc = "MF";
                BusinessDayConvention bdc2 = Conversion.ConvertObject<BusinessDayConvention>(bdc, "businessdayconvention");

                if (string.IsNullOrEmpty(rule)) rule = "BACKWARD";
                DateGeneration.Rule rule2 = Conversion.ConvertObject<DateGeneration.Rule>(rule, "dategenerationrule");

                Schedule sch = new Schedule(start, end, period, can, bdc2, bdc2, rule2, eom);

                object[,] ret = new object[sch.size(), 1];
                for (uint i = 0; i < sch.size(); i++)
                {
                    ret[i, 0] = sch.date(i).serialNumber();
                }

                return ret;
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }
    }
}

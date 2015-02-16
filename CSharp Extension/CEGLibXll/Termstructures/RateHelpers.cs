using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ExcelDna.Integration;
using ExcelDna.Integration.Rtd;
using Xl = Microsoft.Office.Interop.Excel;

namespace CEGLibXll
{
    
    public class RateHelpers
    {
        /*
        [ExcelFunction(Description = "create deposit rate helper", Category = "CEGLibXll - TermStructure")]
        public static string cegDepositRateHelper(
            [ExcelArgument(Description = "(String) id of rate helper object ")] String ObjectId,
            [ExcelArgument(Description = "(double) quote of deposit rate ")] double Quote,
            [ExcelArgument(Description = "(String) forward start month, e.g. 3M ")] String Tenor,
            [ExcelArgument(Description = "(String) id of index object ")]String IndexId)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = QLUtil.getActiveCellAddress();

            try
            {
                // To do: use default value
                QuantLib.QuoteHandle quote_ = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(Quote));
                QuantLib.Period tenor_ = QLUtil.ConvertObject<QuantLib.Period>(Tenor, "tenor");
                QuantLib.Index index = OHRepository.Instance.getObject<QuantLib.Index>(IndexId);
                if (FixingDays is ExcelMissing || FixingDays is ExcelEmpty)
                    FixingDays = 2;
                QuantLib.RateHelper rh = new QuantLib.DepositRateHelper(Quote, tenor_, fixingDays, calendar, convention, eom, dayCounter);

                string id = "RHDEP#" + ObjectId;
                OHRepository.Instance.storeObject(id, rh, callerAddress);
                return id + "@" + DateTime.Now.ToString("HH:mm:ss");
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "#QL_ERR!";
            }
        }

        [ExcelFunction(Description = "create future rate helper", Category = "CEGLibXll - TermStructure")]
        public static string cegFutureRateHelper(
            [ExcelArgument(Description = "(String) id of rate helper object ")] String ObjectId,
            [ExcelArgument(Description = "(double) quote of deposit rate ")] double Quote,
            [ExcelArgument(Description = "(String) forward start month, e.g. 3M ")] String Tenor,
            [ExcelArgument(Description = "(String) id of index object ")]String IndexId)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = QLUtil.getActiveCellAddress();

            try
            {
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "#QL_ERR!";
            }
        }
        */
    }
}

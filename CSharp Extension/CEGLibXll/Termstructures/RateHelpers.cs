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
        [ExcelFunction(Description = "create deposit rate helper", Category = "CEGLibXll - Rates")]
        public static string cegDepositRateHelper(
            [ExcelArgument(Description = "(String) id of rate helper object ")] String ObjectId,
            [ExcelArgument(Description = "(double) quote of deposit rate ")] double Quote,
            [ExcelArgument(Description = "(String) forward start month, e.g. 3M ")] String Tenor,
            [ExcelArgument(Description = "(String) id of index object ")]String IndexId,
            [ExcelArgument(Description = "Calendar (default NYC) ")]string calendar = "LON|NYC",
            [ExcelArgument(Description = "Day Counter (default Actual360) ")]string dc = "Actual360",
            [ExcelArgument(Description = "Convention (default MF) ")]string convention = "ModifiedFollowing",
            [ExcelArgument(Description = "End of Month (default true) ")]bool endofmonth = true,
            [ExcelArgument(Description = "FixingDays (default 2) ")]int fixingDays = 2)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = SystemUtil.getActiveCellAddress();

            try
            {
                // To do: use default value
                QuantLib.QuoteHandle quote_ = new QuantLib.QuoteHandle(new QuantLib.SimpleQuote(Quote));
                QuantLib.Period tenor_ = Conversion.ConvertObject<QuantLib.Period>(Tenor, "tenor");
                QuantLib.Index index = OHRepository.Instance.getObject<QuantLib.Index>(IndexId);
                QuantLib.Index id = new QuantLib.USDLibor(tenor, curvehandle);
                if ((object)fixingDays is ExcelMissing || (object)fixingDays is ExcelEmpty)
                    fixingDays = 2;
                QuantLib.RateHelper rh = new QuantLib.DepositRateHelper(Quote, tenor_, fixingDays, calendar, convention, eom, dayCounter);

                string id = "RHDEP#" + ObjectId;
                OHRepository.Instance.storeObject(id, rh, callerAddress);
                return id + "@" + DateTime.Now.ToString("HH:mm:ss");
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "#CEG_ERR!";
            }
        }

        [ExcelFunction(Description = "create future rate helper", Category = "CEGLibXll - Rates")]
        public static string cegFutureRateHelper(
            [ExcelArgument(Description = "(String) id of rate helper object ")] String ObjectId,
            [ExcelArgument(Description = "(double) quote of deposit rate ")] double Quote,
            [ExcelArgument(Description = "(String) forward start month, e.g. 3M ")] String Tenor,
            [ExcelArgument(Description = "(String) id of index object ")]String IndexId)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = SystemUtil.getActiveCellAddress();

            try
            {
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "#CEG_ERR!";
            }
        }
    }
}

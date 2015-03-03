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
    public class EuropeanOption
    {
        [ExcelFunction(Description = "European Option with Black Scholes Pricer", Category = "CEGLibXll - Instruments")]
        public static string cegEuropeanOptionBlackScholes(
            [ExcelArgument(Description = "id of option to be constructed ")] string ObjectId,
            [ExcelArgument(Description = "Option type ")]string optype,
            [ExcelArgument(Description = "Spot price ")]double underlyingprice,
            [ExcelArgument(Description = "Strike price ")]double stirkeprice,
            [ExcelArgument(Description = "Expiry Date ")]DateTime date,
            [ExcelArgument(Description = "Risk free rate ")]double riskfreerate,
            [ExcelArgument(Description = "dividend/convenience rate ")]double dividendrate,
            [ExcelArgument(Description = "Black-Scholes Vol ")]double volatility,
            [ExcelArgument(Description = "DayCounter ")]string daycounter,
            [ExcelArgument(Description = "Calendar ")]string calendar)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = "";            
            callerAddress = SystemUtil.getActiveCellAddress();

            try
            {
                if (date == DateTime.MinValue)
                    throw new Exception("Date must not be empty. ");

                if (string.IsNullOrEmpty(daycounter))
                    daycounter = "ACTUAL365";
                if (string.IsNullOrEmpty(calendar))
                    calendar = "NYC";

                Option.Type optiontype;
                if (optype.ToUpper() == "CALL")
                    optiontype = Option.Type.Call;
                else if (optype.ToUpper() == "PUT")
                    optiontype = Option.Type.Put;
                else
                    throw new Exception("Unknow option type");

                QuantLib.Calendar cal = Conversion.ConvertObject<QuantLib.Calendar>(calendar, "calendar");
                QuantLib.DayCounter dc = Conversion.ConvertObject<QuantLib.DayCounter>(daycounter, "daycounter");
                QuantLib.Date maturitydate = Conversion.ConvertObject<QuantLib.Date>(date, "date");

                QuantLib.Date today = QuantLib.Settings.instance().getEvaluationDate();
                QuantLib.Date settlementdate = today;           // T+2
                if (maturitydate.serialNumber() <= today.serialNumber())
                    throw new Exception("Option already expired.");

                EuropeanExercise europeanExercise = new EuropeanExercise(maturitydate);

                QuoteHandle underlyingQuoteH = new QuoteHandle(new QuantLib.SimpleQuote(underlyingprice));

                YieldTermStructureHandle flatRateTSH = new YieldTermStructureHandle(
                    new FlatForward(settlementdate, riskfreerate, dc));
                YieldTermStructureHandle flatDividendTSH = new YieldTermStructureHandle(
                    new FlatForward(settlementdate, dividendrate, dc));

                BlackVolTermStructureHandle flatVolTSH = new BlackVolTermStructureHandle(
                    new BlackConstantVol(settlementdate, cal, volatility, dc));

                BlackScholesMertonProcess stochasticProcess = new BlackScholesMertonProcess(underlyingQuoteH,
                    flatDividendTSH, flatRateTSH, flatVolTSH);

                PlainVanillaPayoff payoff = new PlainVanillaPayoff(optiontype, stirkeprice);

                VanillaOption europeanOption = new VanillaOption(payoff, europeanExercise);

                europeanOption.setPricingEngine(new AnalyticEuropeanEngine(stochasticProcess));

                // Store the option and return its id
                string id = "OPTION@" + ObjectId;
                OHRepository.Instance.storeObject(id, europeanOption, callerAddress);
                id += "#" + (String)DateTime.Now.ToString(@"HH:mm:ss");
                return id;
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }

        [ExcelFunction(Description = "European Option with Black Scholes Pricer", Category = "CEGLibXll - Instruments")]
        public static object cegGetOptionBSMGreek(
            [ExcelArgument(Description = "id of option ")] string ObjectId,
            [ExcelArgument(Description = "Greek type ")]string gtype)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = "";            
            callerAddress = SystemUtil.getActiveCellAddress();

            try
            {
                Xl.Range rng = SystemUtil.getActiveCellRange();
                VanillaOption option = OHRepository.Instance.getObject<VanillaOption>(ObjectId);
                switch(gtype.ToUpper())
                {
                    case "NPV":
                        return option.NPV();
                    case "DELTA":
                        return option.delta();
                    case "GAMMA":
                        return option.gamma();
                    case "VEGA":
                        return option.vega();
                    case "THETA":
                        return option.theta();
                    case "RHO":
                        return option.rho();
                    default:
                        return 0;
                }
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }
    }
}

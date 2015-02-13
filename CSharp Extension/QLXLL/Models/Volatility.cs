using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ExcelDna.Integration;
using ExcelDna.Integration.Rtd;
using Xl = Microsoft.Office.Interop.Excel;
using QuantLib;

namespace QLXLL
{
    public class Volatility
    {
        #region SABR
        [ExcelFunction(Description = "SABR section calibration", Category = "QLXLL - Models")]
        public static string qlSABRModel(
            [ExcelArgument(Description = "id of option to be constructed ")] string ObjectId,
            [ExcelArgument(Description = "ATM forward ")]double forward,
            [ExcelArgument(Description = "expiry (years of fraction ")]double tenor,
            [ExcelArgument(Description = "Strikes ")]double[] strikes,
            [ExcelArgument(Description = "implied vols ")]double[] volatilities,
            [ExcelArgument(Description = "initial parameters ")]double[] initials,
            [ExcelArgument(Description = "is parameters fixed? ")]object[] isfixed)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = QLUtil.getActiveCellAddress();

            try
            {
                if (strikes.Length != volatilities.Length)
                    throw new Exception("SABR input lengths don't match.");

                if ((initials.Length !=4) || (isfixed.Length != 4))
                    throw new Exception("SABR has four parameters");

                QlArray xx = new QlArray((uint)strikes.Length); 
                QlArray yy = new QlArray((uint)volatilities.Length);
                for (uint i = 0; i < strikes.Length; i++)
                {
                    xx.set(i, strikes[i]);
                    yy.set(i, volatilities[i]);
                }

                EndCriteria endcriteria = new EndCriteria(100000, 100, 1e-8, 1e-8, 1e-8);
                OptimizationMethod opmodel = new Simplex(0.01);
                // Change the parameter sequence
                // alpha ~ sigma0(3), beta ~ beta(1), nu ~ alpha(0), rho ~ rho(2)
                SABRInterpolation sabr = new SABRInterpolation(xx, yy, tenor, forward, 
                    initials[3], initials[1], initials[0], initials[2],
                    endcriteria, opmodel,
                    (bool)isfixed[3], (bool)isfixed[1], (bool)isfixed[0], (bool)isfixed[2], true);
                
                double err = 0;
                // only update if there exist free parameters
                if (!((bool)isfixed[0] && (bool)isfixed[1] && (bool)isfixed[2] && (bool)isfixed[3]))
                    err = sabr.update();

                // Store the option and return its id
                string id = "MODEL@" + ObjectId;
                OHRepository.Instance.storeObject(id, sabr, callerAddress);
                id += "#" + (String)DateTime.Now.ToString(@"HH:mm:ss");
                return id;
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }


        [ExcelFunction(Description = "Get SABR interpolated value", Category = "QLXLL - Models")]
        public static object qlGetSABRCalibratedParameters(
            [ExcelArgument(Description = "id of SABR model ")] string ObjectId)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = QLUtil.getActiveCellAddress();

            try
            {
                Xl.Range rng = QLUtil.getActiveCellRange();
                SABRInterpolation option = OHRepository.Instance.getObject<SABRInterpolation>(ObjectId);

                // Change the parameter sequence
                // alpha ~ sigma0(3), beta ~ beta(1), nu ~ alpha(0), rho ~ rho(2)
                object[,] ret = new object[6,2];
                ret[0, 0] = "alpha:";  ret[0, 1] = option.nu();
                ret[1, 0] = "beta:"; ret[1, 1] = option.beta();
                ret[2, 0] = "rho:"; ret[2, 1] = option.rho();
                ret[3, 0] = "atmvol:"; ret[3, 1] = option.alpha();
                ret[4, 0] = "rmsError:"; ret[4, 1] = option.rmsError();
                ret[5, 0] = "maxError:"; ret[5, 1] = option.maxError();

                return ret;
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }

        [ExcelFunction(Description = "Get SABR interpolated value", Category = "QLXLL - Models")]
        public static object qlGetSABRInterpolatedValue(
            [ExcelArgument(Description = "id of SABR model ")] string ObjectId,
            [ExcelArgument(Description = "x value ")]double x)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = QLUtil.getActiveCellAddress();

            try
            {
                Xl.Range rng = QLUtil.getActiveCellRange();
                SABRInterpolation option = OHRepository.Instance.getObject<SABRInterpolation>(ObjectId);
                return option.call(x);
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }
        #endregion
    }
}

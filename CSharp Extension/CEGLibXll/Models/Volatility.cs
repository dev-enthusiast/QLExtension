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
    public class Volatility
    {
        #region SABR
        [ExcelFunction(Description = "SABR section calibration", Category = "CEGLibXll - Models")]
        public static string cegSABRModel(
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

                int[] idx = volatilities.Select((v, Index) => new { V = v, idx = Index })
                    .Where(x => x.V == 0)
                    .Select(x => x.idx)
                    .ToArray();
                
                QlArray xx = new QlArray((uint)strikes.Length - (uint)idx.Length); 
                QlArray yy = new QlArray((uint)volatilities.Length - (uint)idx.Length);
                for (uint i = 0, j = 0; i < strikes.Length; i++)
                {
                    if (volatilities[i] == 0)       // empty
                        continue;
                    xx.set(j, strikes[i]);
                    yy.set(j, volatilities[i]);
                    j++;
                }

                EndCriteria endcriteria = new EndCriteria(100000, 100, 1e-8, 1e-8, 1e-8);
                OptimizationMethod opmodel = new Simplex(0.01);
                // alpha: ATM Vol, beta: CEV param, rho: underlying/vol correlation, nu: vol of vol
                SABRInterpolation sabr = new SABRInterpolation(xx, yy, tenor, forward, 
                    initials[0], initials[1], initials[2], initials[3],
                    endcriteria, opmodel,
                    (bool)isfixed[0], (bool)isfixed[1], (bool)isfixed[2], (bool)isfixed[3], true);
                
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


        [ExcelFunction(Description = "Get SABR interpolated value", Category = "CEGLibXll - Models")]
        public static object cegGetSABRCalibratedParameters(
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

                object[,] ret = new object[6,2];
                ret[0, 0] = "alpha:";  ret[0, 1] = option.alpha();
                ret[1, 0] = "beta:"; ret[1, 1] = option.beta();
                ret[2, 0] = "nu:"; ret[2, 1] = option.nu();
                ret[3, 0] = "rho:"; ret[3, 1] = option.rho();
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

        [ExcelFunction(Description = "Get SABR interpolated value", Category = "CEGLibXll - Models")]
        public static object cegGetSABRInterpolatedValue(
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

        #region SVI
        [ExcelFunction(Description = "SVI section calibration", Category = "CEGLibXll - Models")]
        public static string cegSVIModel(
            [ExcelArgument(Description = "id of option to be constructed ")] string ObjectId,
            [ExcelArgument(Description = "ATM forward ")]double forward,
            [ExcelArgument(Description = "expiry (years of fraction ")]double tenor,
            [ExcelArgument(Description = "Strikes ")]double[] strikes,
            [ExcelArgument(Description = "implied vols ")]double[] volatilities,
            [ExcelArgument(Description = "initial parameters ")]double[] initials)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = QLUtil.getActiveCellAddress();

            try
            {
                if (strikes.Length != volatilities.Length)
                    throw new Exception("SABR input lengths don't match.");

                if (initials.Length != 5)
                    throw new Exception("SVI has five parameters.");

                int[] idx = volatilities.Select((v, Index) => new { V = v, idx = Index })
                    .Where(x => x.V == 0)
                    .Select(x => x.idx)
                    .ToArray();
                
                QlArray xx = new QlArray((uint)strikes.Length - (uint)idx.Length); 
                QlArray yy = new QlArray((uint)volatilities.Length - (uint)idx.Length);
                for (uint i = 0, j = 0; i < strikes.Length; i++)
                {
                    if (volatilities[i] == 0)       // empty
                        continue;
                    xx.set(j, strikes[i]);
                    yy.set(j, volatilities[i]);
                    j++;
                }

                EndCriteria endcriteria = new EndCriteria(100000, 100, 1e-8, 1e-8, 1e-8);
                OptimizationMethod opmodel = new Simplex(0.01);
                // alpha: ATM Vol, beta: CEV param, rho: underlying/vol correlation, nu: vol of vol
                SVIInterpolation svi = new SVIInterpolation(xx, yy, tenor, forward, 
                    initials[0], initials[1], initials[2], initials[3], initials[4],
                    endcriteria, opmodel, true);
                
                double err = 0;
                err = svi.update();

                // Store the option and return its id
                string id = "MODEL@" + ObjectId;
                OHRepository.Instance.storeObject(id, svi, callerAddress);
                id += "#" + (String)DateTime.Now.ToString(@"HH:mm:ss");
                return id;
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }


        [ExcelFunction(Description = "Get SVI interpolated value", Category = "CEGLibXll - Models")]
        public static object cegGetSVICalibratedParameters(
            [ExcelArgument(Description = "id of SVI model ")] string ObjectId)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = QLUtil.getActiveCellAddress();

            try
            {
                Xl.Range rng = QLUtil.getActiveCellRange();
                SVIInterpolation option = OHRepository.Instance.getObject<SVIInterpolation>(ObjectId);

                object[,] ret = new object[7,2];
                ret[0, 0] = "a:";  ret[0, 1] = option.a();
                ret[1, 0] = "b:"; ret[1, 1] = option.b();
                ret[2, 0] = "rho:"; ret[2, 1] = option.rho();
                ret[3, 0] = "m:"; ret[3, 1] = option.m();
                ret[4, 0] = "sigma:"; ret[3, 1] = option.sigma();
                ret[5, 0] = "rmsError:"; ret[4, 1] = option.rmsError();
                ret[6, 0] = "maxError:"; ret[5, 1] = option.maxError();

                return ret;
            }
            catch (Exception e)
            {
                QLUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return "";
            }
        }

        [ExcelFunction(Description = "Get SVI interpolated value", Category = "CEGLibXll - Models")]
        public static object cegGetSVIInterpolatedValue(
            [ExcelArgument(Description = "id of SVI model ")] string ObjectId,
            [ExcelArgument(Description = "strik value ")]double x)
        {
            if (QLUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            callerAddress = QLUtil.getActiveCellAddress();

            try
            {
                Xl.Range rng = QLUtil.getActiveCellRange();
                SVIInterpolation option = OHRepository.Instance.getObject<SVIInterpolation>(ObjectId);
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

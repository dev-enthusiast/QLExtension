using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xl = Microsoft.Office.Interop.Excel;
using ExcelDna.Integration;
using ExcelDna.Integration.CustomUI;
using ExcelDna.Logging;

namespace CEGLibXll
{
    sealed public class Operation
    {
        public static bool CallFromWizardFlag = true;
        public static bool CallerAddressFlag = false;

        [ExcelFunction(Description = "toggle callFromWizard control", Category = "CEGLibXll - Operation")]
        public static bool cegOpCheckCallFromWizard(
            [ExcelArgument(Description = "true/false")] bool control)
        {
            CallFromWizardFlag = control;
            return CallFromWizardFlag;
        }

        [ExcelFunction(Description = "toggle callerAddress control", Category = "CEGLibXll - Operation")]
        public static bool cegOpCallerAddressControl(
            [ExcelArgument(Description = "true/false")] bool control)
        {
            CallerAddressFlag = control;
            return CallerAddressFlag;
        }

        [ExcelFunction(Description = "Display xll path", IsMacroType = true, Category = "CEGLibXll - Operation")]
        public static string cegOpLibXllPath()
        {
            string callerAddress = "";
            try
            {
                callerAddress = SystemUtil.getActiveCellAddress();
                OHRepository.Instance.removeErrorMessage(callerAddress);
            }
            catch (Exception)
            {
            }

            string appName = null;
            try
            {
                appName = Version.getXllPath();
            }
            catch (Exception exception_)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), exception_.Message);
                appName = exception_.Message;
            }
            return appName;
        }
    }
}

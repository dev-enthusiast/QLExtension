using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ExcelDna.Integration;
using ExcelDna.Integration.Rtd;
using Xl = Microsoft.Office.Interop.Excel;

namespace QLXLL
{
    public class QLEnum
    {
        [ExcelFunction(Description = "retrieve enum list", Category = "QLXLL - Operation")]
        public static bool qlGetEnumerationList(
            [ExcelArgument(Description = "Enum Class Name")] string enumclassname)
        {
            return false;
        }
    }
}

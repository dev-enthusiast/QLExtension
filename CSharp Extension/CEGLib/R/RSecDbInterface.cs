using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace CEGLib.R
{
    class RSecDbInterface
    {

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern SEXPREC SetSecDbDatabase(SEXPREC dbString);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern SEXPREC TSDBRead(SEXPREC curveName, SEXPREC startDate, SEXPREC endDate);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern SEXPREC GetSymbolInfo(SEXPREC symbolName);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern SEXPREC GetValueType(SEXPREC securityName, SEXPREC valueTypeName);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern SEXPREC EvaluateSlang(SEXPREC slangCode);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern SEXPREC CallFunction(SEXPREC libraryName, SEXPREC functionName, SEXPREC argumentList);
    }
}

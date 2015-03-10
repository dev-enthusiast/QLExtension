using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

using RDotNet;
using RDotNet.Internals;

namespace CEGLib.R
{
    public class RSecDbInterface
    {
        private static REngine rengine_;

        public RSecDbInterface()
        {
            REngine.SetEnvironmentVariables();
            rengine_ = REngine.GetInstance();
        }
        ~RSecDbInterface()
        {
            if (rengine_ != null)
            {
                rengine_.Close();
                rengine_.Dispose();
            }
        }

        public static void SetDatabase(string dbname = "!Bal Prod 1;ReadOnly{!Bal Prod 2}")
        {
            try
            {
                CharacterVector name = rengine_.CreateCharacter(dbname);
                SEXPREC sexp = name.GetInternalStructure();
                IntPtr retp = SetSecDbDatabase(ref sexp);
                SEXPREC ret = (SEXPREC)Marshal.PtrToStructure(retp, typeof(SEXPREC));
            }
            catch(Exception e)
            {
                ConfigManager.Debug("unable to connect to db: " + e.Message);
            }
        }

        public static void GetValueType(string securityname, string valuetype)
        {
            try
            {
                CharacterVector security = rengine_.CreateCharacter(securityname);
                SEXPREC sexpsec = security.GetInternalStructure();
                CharacterVector type = rengine_.CreateCharacter(valuetype);
                SEXPREC sexpsectype = type.GetInternalStructure();

                IntPtr retp = GetValueType(ref sexpsec, ref sexpsectype);
                SEXPREC ret = (SEXPREC)Marshal.PtrToStructure(retp, typeof(SEXPREC));
                
            }
            catch(Exception e)
            {
                ConfigManager.Debug("unable to connect to db: " + e.Message);
            }
        }


        #region dllimport
        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr SetSecDbDatabase(ref SEXPREC dbString);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr TSDBRead(ref SEXPREC curveName, ref SEXPREC startDate, ref SEXPREC endDate);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr GetSymbolInfo(ref SEXPREC symbolName);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr GetValueType(ref SEXPREC securityName, ref SEXPREC valueTypeName);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr EvaluateSlang(ref SEXPREC slangCode);

        [DllImport("RSecDbInterface.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr CallFunction(ref SEXPREC libraryName, ref SEXPREC functionName, ref SEXPREC argumentList);
        #endregion
    }
}

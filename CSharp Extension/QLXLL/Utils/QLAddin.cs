using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using Xl = Microsoft.Office.Interop.Excel;
using ExcelDna.Integration;
using ExcelDna.ComInterop;
using ExcelDna.Integration.CustomUI;
using ExcelDna.Logging;


namespace QLXLL
{
    public class QLAddin : IExcelAddIn
    {
        public void AutoOpen()
        {
            System.Windows.Forms.MessageBox.Show("Welcome to QLXLL");
            ComServer.DllRegisterServer();
        }

        public void AutoClose()
        {
            ComServer.DllUnregisterServer();
            System.Windows.Forms.MessageBox.Show("Thanks for using QLXLL");
        }
    }
}

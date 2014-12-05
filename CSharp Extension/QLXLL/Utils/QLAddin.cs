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
    [ComVisible(true)]
    public class QLAddin : IExcelAddIn
    {
        public void AutoOpen()
        {
            string xllName = (string)XlCall.Excel(XlCall.xlGetName);
            string rootPath = System.IO.Path.GetDirectoryName(xllName);
            rootPath = System.IO.Path.Combine(rootPath, @"..\..\QLExpansion-master\");
            rootPath = System.IO.Path.GetFullPath(rootPath);

            System.Windows.Forms.MessageBox.Show("Welcome to QLXLL");
            System.Windows.Forms.MessageBox.Show("QLXLL Loaded from " + xllName);
            System.Windows.Forms.MessageBox.Show("Root Path is " + rootPath);

            ComServer.DllRegisterServer();
        }

        public void AutoClose()
        {
            ComServer.DllUnregisterServer();
            System.Windows.Forms.MessageBox.Show("Thanks for using QLXLL");
        }
    }
}

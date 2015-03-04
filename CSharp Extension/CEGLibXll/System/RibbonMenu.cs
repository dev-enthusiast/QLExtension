using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Threading;
using System.Reflection;
using Xl = Microsoft.Office.Interop.Excel;
using System.Data;
using System.Runtime.InteropServices;
using ExcelDna.Integration;
using ExcelDna.Integration.CustomUI;
using ExcelDna.Logging;


namespace CEGLibXll
{
    [ComVisible(true)]
    //[ClassInterface(ClassInterfaceType.AutoDispatch)]
    public class RibbonMenu : ExcelRibbon
    {
        public static SynchronizationContext syncContext_;
        #region Event Handler
        public static void Login_Click()
        {
            System.Windows.Forms.MessageBox.Show("No Login needed");
        }

        public void Error_Click(IRibbonControl control_)
        {
            LogDisplay.Show();
        }

        public void Help_Click(IRibbonControl control_)
        {
            //System.Diagnostics.Process.Start(xllDir + @"documents\CEGLibXll.chm");
            System.Windows.Forms.MessageBox.Show("Please contact RiskAnalytics for help.");
        }

        public void About_Click(IRibbonControl control_)
        {
            // About abt = new About();
            // abt.ShowDialog();
            System.Windows.Forms.MessageBox.Show("CEGLib v1.0");
        }

        public void Function_Click(IRibbonControl control_)
        {
            Xl.Application xlApp = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            String fname = control_.Id;
            fname = "=" + fname + "()";

            Xl.Range rg = xlApp.ActiveCell;

            String cellName = SystemUtil.ExcelColumnIndexToName(rg.Column) + rg.Row;

            Xl._Worksheet sheet = (Xl.Worksheet)xlApp.ActiveSheet;
            Xl.Range range = sheet.get_Range(cellName, System.Type.Missing);
            string previousFormula = range.FormulaR1C1.ToString();
            range.Value2 = fname;
            range.Select();

            syncContext_ = SynchronizationContext.Current;
            if (syncContext_ == null)
            {
                syncContext_ = new System.Windows.Forms.WindowsFormsSynchronizationContext();
            }

            FunctionWizardThread othread = new FunctionWizardThread(range, syncContext_);
            Thread thread = new Thread(new ThreadStart(othread.functionargumentsPopup));
            thread.Start();
        }

        public void excelFile_Click(IRibbonControl control_)
        {
            Xl.Application xlApp = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            String fname = control_.Id;

            string file = "";
            switch (fname)
            {
                case "OptionPricer":
                    file = "OptionPricer.xlsx";
                    break;
                default:
                    break;
            }

            file = @"C:\Workspace\Output\Debug\OptionPricer.xlsx";
            xlApp.Workbooks.Open(file);
        }
        #endregion
    }
}

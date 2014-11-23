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


namespace QLXLL
{
    [ComVisible(true)]
    public partial class QLRibbon : ExcelRibbon
    {
        // inner class
        public class DisabledItemCheck
        {
            // Constructor
            public DisabledItemCheck()
            {
                string disabledItemsKey = @"Software\Microsoft\Office\14.0\Excel\Resiliency\DisabledItems";
                Microsoft.Win32.RegistryKey key = Microsoft.Win32.Registry.CurrentUser.OpenSubKey(disabledItemsKey, true);

                if (key != null && key.ValueCount != 0)
                {
                    string[] strlist = key.GetValueNames();
                    foreach (string str in strlist)
                    {
                        Object vname = Microsoft.Win32.Registry.GetValue(@"HKEY_CURRENT_USER\" + disabledItemsKey, str, null);
                        if (vname != null)
                        {
                            String decode = System.Text.Encoding.Default.GetString((byte[])vname).Replace("\0","").ToLower();
                            if (decode.Contains("qlxll"))
                            {
                                key.DeleteValue(str);
                                System.Windows.Forms.DialogResult ret = System.Windows.Forms.MessageBox.Show(
                                    "Qlxll has been disabled. Restart to fix it?", "Excel warning -- QLXLL start up failed.",
                                    System.Windows.Forms.MessageBoxButtons.OKCancel, System.Windows.Forms.MessageBoxIcon.Warning);
                                if (ret == System.Windows.Forms.DialogResult.OK)
                                {
                                    Xl.Application excelApp = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
                                    excelApp.Quit();
                                }
                            }
                        }
                    }
                }
            }
        }


        public static DisabledItemCheck disabledItemCheck_ = new DisabledItemCheck();
        public static string xllDir = System.IO.Path.GetDirectoryName((string)XlCall.Excel(XlCall.xlGetName)) + @"\..\";
        public static string excelNamespaceUrl = @"http://schemas.microsoft.com/office/2006/01/customui";
        public static SynchronizationContext syncContext_;

            
#region Custom UI XML
        private string xmlHeader = @"<customUI xmlns='http://schemas.microsoft.com/office/2006/01/customui' loadImage='LoadImage'>
          <ribbon>
            <tabs>
              <tab id='FarmTab' label='Farm'>
                <!--
                <group id='templateGroup' labe='Template Group' >
                    <comboBox id='comboTemplate' label='Template' onChange='Template_Click' image='M'>
                        <item id='template1' label='CurveSwap_Template.xls' />
                        <item id='template2' label='bucketConversion_Template.xls' />
                      </comboBox>
                </group>
                <group id='SampleGroup' label='My Dynamic Sample Group'>
                  <button id='Button1' label='My Button Label' image='M' size='large' onAction='RunTagMacro' tag='ShowHelloMessage' />
                  <button id='Button2' label='My Second Button' image='M' size='normal' onAction='OnButtonPressed'/>
                   <menu id='MyMenu' label='Test Menu' itemSize='normal'> 
                       <toggleButton idMso='Bold'/> 
                       <button id='MenuButton1' label='Button1' /> 
                       <menuSeparator id='separator1'/>
                       <toggleButton id='MenuToggleButton1' label='ToggleButton' /> 
                       <button idMso='FileExit'/> 
                       <menu id='Nested1' label='Advanced' itemSize='normal'> 
                          <button idMso='Cut'/> 
                          <button idMso='Copy'/> 
                          <button idMso='Paste'/> 
                          <menuSeparator id='separator2' title='Large Buttons Below'/>
                            <menu id='Nested2' label='Large' itemSize='large'> 
                            <button idMso='Cut'/> 
                             <button idMso='Copy'/> 
                            <button idMso='Paste'/> 
                          </menu> 
                       </menu> 
                    </menu>
                </group >
                -->
                <group id='eToolkitGroup' label='Login'>
                    <button id='buttonEToolkit' label='Summit' image='Tool' size='large' onAction='EToolkit_Click' />
                    <button id='buttonEIdeal' label='Ideal' image='Book' size='large' onAction='Ideal_Click' />
                </group>
                <group id='templateGroup' label='Templates' >
                    <button id='buttonTemplate1' label='Market' tag='Market.xlsm' image='Magnify'  onAction='Template_Click' />
                    <button id='buttonTemplate2' label='CurveSwap' tag='CurveSwap_Template.xlsm' image='Swap'  onAction='Template_Click' />
                    <button id='buttonTemplate3' label='BucketConversion' tag='bucketConversion_Template.xls' image='Bucket' onAction='Template_Click' />
                    <button id='buttonTemplate4' label='MultiCCY Curve' tag='Curve_Template.xlsm' image='Curve' onAction='Template_Click' />
                    <button id='buttonTemplate5' label='Credit' tag='differential_discounting.xlsm' image='Discount' onAction='Template_Click' />
                </group>
                <group id='errorGroup' label='Error'>
                    <button id='buttonError' label='Display Error Details' imageMso='GroupShowHideListView' size='large' onAction='Error_Click'/>
                </group>
                <group id='functionGroup' label='FARM Functions'>
                </group>
                <group id='helpGroup' label='Help'>
                    <button id='buttonHelper' label='Help Context' imageMso='ShapeBasicsHelp' onAction='Help_Click'/>
                    <button id='buttonAbout' label='About Farm' imageMso='OfficeDiagnostics' onAction='About_Click'/>
                </group>
              </tab>
            </tabs>
          </ribbon>
        </customUI>";
#endregion

        public override string GetCustomUI(string RibbonID)
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.LoadXml(xmlHeader);
                XmlNamespaceManager nsmgr = new XmlNamespaceManager(doc.NameTable);
                nsmgr.AddNamespace("df", @"http://schemas.microsoft.com/office/2006/01/customui");
                XmlElement tab = (XmlElement)doc.DocumentElement.SelectSingleNode("df:ribbon/df:tabs/df:tab[@id='QLTab']", nsmgr);
                XmlElement groupEle = (XmlElement)tab.SelectSingleNode("df:group[@id='functionGroup']", nsmgr);

                // add function group
                Dictionary<String, StringCollection> functionList = getFunctionList();
                int index = 0;
                foreach (KeyValuePair<String, StringCollection> kvp in functionList)
                {
                    string category = kvp.Key;
                    // create a menu in ribbon for each category
                    XmlElement menuEle = doc.CreateElement("menu", excelNamespaceUrl);
                    groupEle.AppendChild(menuEle);
                    menuEle.SetAttribute("id", "QLMenu" + index);
                    string menuLabel = category.Replace("QLXLL - ", "");
                    menuEle.SetAttribute("label", menuLabel);
                    menuEle.SetAttribute("itemSize", "normal");

                    // add menu items
                    foreach (string functionName in kvp.Value)
                    {
                        XmlElement menuItem = doc.CreateElement("button", excelNamespaceUrl);
                        menuEle.AppendChild(menuItem);
                        menuItem.SetAttribute("id", functionName);
                        menuItem.SetAttribute("label", functionName);
                        menuItem.SetAttribute("onAction", "Function_Click");
                    }
                    ++index;
                }

                pctrl.Caption = "QLXLL Action ... ";
                pctrl.Tag = "QLXLL Action";
                pctrl.OnAction = "QLXLL_Action_Click";

                Menu_Fwd = pctrl.Controls.Add(MsoControlType.msoControlButton, Type.Missing, Type.Missing, 1, true);
                Menu_Fwd.Caption = "Plot Fwd Curve";
                Menu_Fwd.OnAction = "Cell_PlotFwdCurve_Click";

                Menu_Disc = pctrl.Controls.Add(MsoControlType.msoControlButton, Type.Missing, Type.Missing, 1, true);
                Menu_Disc.Caption = "Plot Disc Curve";
                Menu_Disc.OnAction = "Cell_PlotDiscCurve_Click";

                Menu_PV01 = pctrl.Controls.Add(MsoControlType.msoControlButton, Type.Missing, Type.Missing, 1, true);
                Menu_PV01.Caption = "Show PV01";
                Menu_PV01.OnAction = "Cell_ShowPV01_Click";

                Menu_Leg = pctrl.Controls.Add(MsoControlType.msoControlButton, Type.Missing, Type.Missing, 1, true);
                Menu_Leg.Caption = "Display Leg";
                Menu_Leg.OnAction = "Cell_DispLeg_Click";

                set_Menu_Invisible();

                CommandBarControl MenuError = popUp.Controls.Add(MsoControlType.msoControlButton, Type.Missing, Type.Missing, 1, true);
                MenuError.Caption = "Show Error ...";
                MenuError.OnAction = "Cell_Error_Click";

                // QLExpansion.EnumRepository.Instance.registerAllEnum();
                // QLExpansion.Calendar.SetEvaluationDate(DateTime.Now.Date);

                if (File.Exists(xllDir + @"\data\config\QL_Default.xml"))
                {
                    System.Environment.SetEnvironmentVariable("QLInstallDir", xllDir, EnvironmentVariableTarget.Process);
                    System.Environment.SetEnvironmentVariable("QLInstallDir", xllDir, EnvironmentVariableTarget.User);
                }

                QLOperation.addEnumDictionary();

                QLExpansion.ConfigRepository.Instance.setDefaultValues();
                // QLExpansion.CalendarRepository.Instance.loadAllHolidays();
                string tmpPath = System.Environment.GetEnvironmentVariable("TMP") + @"\amort\";
                if (!System.IO.Directory.Exists(tmpPath))
                {
                    System.IO.Directory.CreateDirectory(tmpPath);
                }
                else
                {
                    DirectoryInfo di = new DirectoryInfo(tmpPath);
                    FileInfo[] outfiles = di.GetFiles("*.xml");
                    foreach (FileInfo amortfile in outfiles)
                    {
                        Dictionary<String, double[]> AmortTable = new Dictionary<string, double[]>();
                        XmlDocument xmldoc = new XmlDocument();
                        xmldoc.Load(amortfile.FullName);

                        XmlNodeList elements = xmldoc.SelectNodes("/AmortTable/");
                        foreach(XmlElement element in elements)
                        {
                            XmlNodeList valElements = element.SelectNodes("value");
                            double[] values = new double[valElements.Count];
                            int i = 0;
                            foreach (XmlElement valelem in valElements)
                            {
                                values[i++] = double.Parse(valelem.InnerText);
                            }
                            AmortTable.Add(element.Name, values);
                        }
                        OHRepository.Instance.storeObject(amortfile.Name.Substring(0,amortfile.Name.Length-4),AmortTable, "QLRibbonMenu");
                    }
                }

                return doc.OuterXml;
            }
            catch (Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.Message);
                return "";
            }
        }

        public override void OnConnection(object Application, ExcelDna.Integration.Extensibility.ext_ConnectMode ConnectMode, object AddInInst, ref Array custom)
        {
            ExcelAsyncUtil.Initialize();                // required to perform async macro call

            Xl.Application excelApp = (Xl.Application)Application;
            ((Xl.AppEvents_Event)excelApp).WorkbookOpen += new Xl.AppEvents_WorkbookOpenEventHandler(QLRibbon_WorkbookOpen);
            ((Xl.AppEvents_Event)excelApp).WorkbookBeforeClose +=new Xl.AppEvents_WorkbookBeforeCloseEventHandler(QLRibbon_WorkbookBeforeClose);

            foreach (var addin in excelApp.COMAddIns)
            {
                Microsoft.Office.Core.COMAddIn a = addin as Microsoft.Office.Core.COMAddIn;
                if (a != null)
                {
                    if (a.ProgId == "vstoFARM" && a.Object != null)
                    {
                        /*
                        QLAddin.Instance.vstoAddin = (IvstoAddin)a.Object;
                        long ticks = DateTime.Now.Ticks;
                        QLAddin.Instance.volParamUpdateReceiver = new VolParamUpdateReceiver(ticks);
                        QLAddin.Instance.volParamUpdateReceiver.onVolParamMatrixChangeService.handler += new Action<QLExpansion.VolParamSerializedMatrix, bool>(this.processVolParamUpdate);

                        excelApp.OnKey("{Enter}", "qlSetKeyPressedFlag");       // enter key on numerical pad
                        excelApp.OnKey("~", "qlSetKeyPressedFlag");     // regular enter key

                        QLAddin.Instance.vstoAddin.createVolParamUpdateSender(ticks); */

                    }
                    else if (a.Description.ToUpper().Contains("QLXLL") && a.Object != null)
                    {
                        throw new Exception("Cannot load QLXLL twice. ");
                    }
                }
            }

            base.OnConnection(Application, ConnectMode, AddInInst, ref custom);
        }
        /*
        void processVolParamUpdate(QLExpansion.VolParamSerializedMatrix e, bool isPossibleManualUpdate)
        {
            try
            {
                String id = "params_" + e.symbol;

                if (isPossibleManualUpdate && OHRepository.Instance.containsObject(id))
                {
                    QLExpansion.VolParamSerializedMatrix previous = OHRepository.Instance.getObject<QLExpansion.VolParamSerializedMatrix>(id);
                    if (e.quote_time <= previous.quote_time)
                    {
                        e.quote_time = DateTime.Now;
                    }
                }

                // insert into repository
                OHRepository.Instance.storeObject(id, e, "");

                qlInstruments.createEqVol(e, e.symbol);
            }
            catch (Exception e_)
            {
                QLAddin.logError("QLRibbonMenu", "processVolParamUpdate", e_.Message);
            }
        }*/

        /// <summary>
        /// namespace ExcelDna.Integration
        /// {
        ///     pubic interface IExcelAddIn
        ///     {
        ///         void AutoClose();
        ///         void AutoOpen();
        ///     }
        /// }
        /// </summary>
        void QLRibbon_WorkbookOpen(Xl.Workbook wb)
        {
        }

        void QLRibbon_WorkbookBeforeClose(Xl.Workbook wb, ref bool Cancel)
        {
        }

        private Dictionary<String, StringCollection> getFunctionList()
        {
            List<_MethodInfo> methods = new List<_MethodInfo>();// = ExcelDna.Integration.DnaLibrary.CurrentLibrary.Methods;

            Dictionary<String, StringCollection> functionList = new Dictionary<string, StringCollection>();

            foreach (MethodInfo mi in methods)
            {
                object[] atts = mi.GetCustomAttributes(false);
                foreach (object att in atts)
                {
                    ExcelDna.Integration.ExcelFunctionAttribute a = (ExcelDna.Integration.ExcelFunctionAttribute)att;
                    if (!functionList.ContainsKey(a.Category))
                    {
                        functionList.Add(a.Category, new StringCollection());
                    }
                    functionList[a.Category].Add(mi.Name);
                }
            }
            return functionList;
        }

        public void AutoOpen()
        {
        }

        #region Click Event Handler
        public void EToolkit_Click(IRibbonControl control_)
        {
            //EToolkitWSApp.Instance.Show();
        }

        public void Ideal_Click(IRibbonControl control_)
        {
            //EToolkitWSApp.Instance.ShowIDeal();
        }

        public void Error_Click(IRibbonControl control_)
        {
            LogDisplay.Show();
        }

        public void Help_Click(IRibbonControl control_)
        {
            System.Diagnostics.Process.Start(xllDir + @"documents\QLXLL.chm");
        }

        public void About_Click(IRibbonControl control_)
        {
            // About abt = new About();
            // abt.ShowDialog();
        }

        public void Function_Click(IRibbonControl control_)
        {
            Xl.Application xlApp = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            String fname = control_.Id;
            fname = "=" + fname + "()";

            Xl.Range rg = xlApp.ActiveCell;

            String cellName = QLAddin.ExcelColumnIndexToName(rg.Column) + rg.Row;

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

        public void Template_Click(IRibbonControl control_)
        {
            Xl.Application xlApp = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            if (xlApp.Workbooks.Count == 0)
                xlApp.Workbooks.Add();

            // xlApp.Calculation = Xl.XlCalculation.xlCalculationManual;
            // xlApp.CalculateBeforeSave = false;

            xlApp.Workbooks.Application.Calculation = Microsoft.Office.Interop.Excel.XlCalculation.xlCalculationManual;
            xlApp.Workbooks.Application.CalculateBeforeSave = false;

            String fname = control_.Tag;
            fname = xllDir + @"Templates\" + fname;
            xlApp.Workbooks.Open(fname, false, false, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
                Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);

        }

        public static void PostLegAmortTableImpl(String LegAmortTable, String LegAnalysis)
        {
            Xl.Application oXl = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            Xl.Workbook oWB = (Xl.Workbook)oXl.ActiveWorkbook;
            Xl.Worksheet oWS = (Xl.Worksheet)oWB.ActiveSheet;
            Xl.Range oWR = (Xl.Range)oWS.Application.Selection;

            Xl.Range ATable;
            Xl.Range LTable;
            Xl.Range PTable;
            int i, j, k;

            Xl.Range RangeLegAmortTable;
            Xl.Range RangeLegAnalysis;
            Xl.Range tmpCell_1;
            Xl.Range tmpCell_2;

            Xl.Range RangeLegAnalysisOffset;
            Xl.Range ATableOffset;
            Xl.Range PTableOffset;
            Xl.Range LTableOffset;

            oWB.Sheets["Leg"].Select();

            RangeLegAmortTable = oWS.get_Range(LegAmortTable);
            tmpCell_1 = oWS.get_Range(QLAddin.ExcelColumnIndexToName(RangeLegAmortTable.Column + 2) + (RangeLegAmortTable.Row + 1).ToString());
            tmpCell_2 = oWS.get_Range(QLAddin.ExcelColumnIndexToName(RangeLegAmortTable.Column + 5) + (RangeLegAmortTable.Row + 10000).ToString());
            oWS.get_Range(tmpCell_1, tmpCell_2).ClearContents();

            RangeLegAnalysis = oWS.get_Range(LegAnalysis);
            RangeLegAnalysisOffset = oWS.get_Range(QLAddin.ExcelColumnIndexToName(RangeLegAnalysis.Offset.Column + 20) + (RangeLegAnalysis.Offset.Row + 0).ToString());


            PTable = oWS.get_Range(RangeLegAnalysis, RangeLegAnalysisOffset).Find("PaymentDates", RangeLegAnalysis, Xl.XlFindLookIn.xlValues, Xl.XlLookAt.xlPart, Xl.XlSearchOrder.xlByColumns, Xl.XlSearchDirection.xlNext, true);

            for (i = 1; i <= 4; i++)
            {
                ATable = oWS.get_Range(QLAddin.ExcelColumnIndexToName(RangeLegAmortTable.Column + 1 + i) + (RangeLegAmortTable.Row).ToString());
                if (ATable != null)
                {
                    LTable = oWS.get_Range(RangeLegAnalysis, RangeLegAnalysisOffset).Find(ATable.Text, RangeLegAnalysis, Xl.XlFindLookIn.xlValues, Xl.XlLookAt.xlPart, Xl.XlSearchOrder.xlByColumns, Xl.XlSearchDirection.xlNext, true);
                    j = 1;
                    k = 1;

                    try
                    {
                        if (LTable != null)
                        {
                            // Tricky: each time while condition is checked, need updated j
                            while ((oWS.get_Range(QLAddin.ExcelColumnIndexToName(LTable.Column) + (LTable.Row + j).ToString()).Text != "") && (oWS.get_Range(QLAddin.ExcelColumnIndexToName(LTable.Column) + (LTable.Row + j).ToString()).Text != "#N/A"))
                            {
                                PTableOffset = oWS.get_Range(QLAddin.ExcelColumnIndexToName(PTable.Column) + (PTable.Row + j).ToString());
                                if (PTableOffset != null)
                                {
                                    if (PTableOffset.Text != "")
                                    {
                                        ATableOffset = oWS.get_Range(QLAddin.ExcelColumnIndexToName(ATable.Column) + (ATable.Row + k).ToString());
                                        LTableOffset = oWS.get_Range(QLAddin.ExcelColumnIndexToName(LTable.Column) + (LTable.Row + j).ToString());
                                        ATableOffset.Value = LTableOffset.Value;
                                        k++;
                                    }
                                }
                                j++;
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        QLAddin.logError("PostLegAmortTable", "PostLegAmortTable", e.Message);
                    }
                }
            }
        }

        public static void PostLegAmortTable()
        {
            PostLegAmortTableImpl("Leg_AmortTable", "Leg_Analysis");
        }

        private static void legdisplayImpl(ExcelReference selection, String LegIDrngName, String targetaddr, String lastaddr, String legccy, String discaddr)
        {
            /*
            object[,] result;
            // Get the value of the selection
            object selectionContent = selection.GetValue();
            string DiscId = "USDLIB3M";
            try
            {
                string legnamedrange = "";
                string legid = string.Empty;

                Xl._Application xlApp = (Xl._Application)ExcelDna.Integration.ExcelDnaUtil.Application;
                Xl._Worksheet sheet = (Xl._Worksheet)xlApp.ActiveSheet;
                Xl.Range range = sheet.get_Range("IndexFrame", System.Type.Missing);
                Xl.Range range2 = sheet.get_Range("EODLIV", System.Type.Missing);

                String selectionval = (String)selection.GetValue();
                if (selectionval.ToUpper().Contains("LEG1"))
                {
                    legid = selectionval;
                    legnamedrange = "SwapDiscount1";
                }
                else
                {
                    legid = selectionval;
                    legnamedrange = "SwapDiscount2";
                }
                Xl.Range range3 = sheet.get_Range(legnamedrange, System.Type.Missing);
                ExcelReference swapdisc = new ExcelReference(range3.Row - 1, selection.ColumnFirst);
                DiscId = (string)swapdisc.GetValue() + "_" + range.Value2.ToString() + "_" + range2.Value2.ToString();

                Xl.Worksheet legsheet = (Xl.Worksheet)xlApp.Sheets["leg"];
                Xl.Range Leg_Analysis = legsheet.get_Range(targetaddr, System.Type.Missing);
                Xl.Range LastRange = legsheet.get_Range(lastaddr, System.Type.Missing);
                Xl.Range retRange = legsheet.get_Range(Leg_Analysis, LastRange);
                retRange.ClearContents();

                if (lastaddr != "LastRange2")
                {
                    Xl.Range Leg2_Analysis = legsheet.get_Range("leg2Clear", System.Type.Missing);
                    Xl.Range LastRange2 = legsheet.get_Range("LastRange2", System.Type.Missing);
                    Xl.Range retRange2 = legsheet.get_Range(Leg2_Analysis, LastRange2);
                    retRange2.ClearContents();

                    Xl.Range Leg2IDrng = legsheet.get_Range("leg2ID", System.Type.Missing);
                    Leg2IDrng.ClearContents();
                }

                object[,] ccy = new object[1, 1];
                object[,] legidval = new object[1, 1];
                legidval[0, 0] = legid;

                if (selectionContent is string)
                {
                    string LegId = (string)selectionContent;

                    QLExpansion.BLeg leg = OHRepository.Instance.getObject<QLExpansion.BLeg>(LegId);
                    QLExpansion..BIborIndex curve = null;
                    try
                    {
                        curve = OHRepository.Instance.getObject<QLExpansion.BIborIndex>(DiscId);
                    }
                    catch (Exception)
                    {
                        curve = null;
                    }

                    StringCollection itemList = new StringCollection();
                    itemList.AddRange(QLExpansion.BLeg.getLegHeader(leg.getLegType()));

                    ccy[0, 0] = leg.getLegCurrency();
                    int count = leg.getLegSize();
                    QLExpansion.LegHeader[] lh = new QLExpansion.LegHeader[itemList.Count];
                    int i = 0;
                    DateTime runDate = QLExpansion.FARMCalendar.getEvaluationDate();
                    List<object[]> retList = new List<object[]>();
                    int PaymentDatesPos = -1;
                    int iColCount = itemList.Count;
                    foreach (String str in itemList)
                    {
                        if ("PaymentDates" == str)
                        {
                            PaymentDatesPos = i;
                        }
                        lh[i++] = (QLExpansion..LegHeader)Enum.Parse(typeof(QLExpansion.LegHeader), str, true);
                    }

                    object[,] rr = leg.getLegInfo(curve, lh, true, QLExpansion.PayerReceiver.Receiver);

                    for (int x = 1; x < rr.GetLength(0); x++)
                    {
                        object[] obj = new object[iColCount];
                        for (int y = 0; y < iColCount; y++)
                        {
                            obj[y] = rr[x, y];
                        }
                        retList.Add(obj);
                    }


                    result = new object[retList.Count + 1, iColCount];
                    int j = 0;
                    for (i = 0; i < iColCount; i++)
                    {
                        result[j, i] = itemList[i];
                    }
                    foreach (object[] dr in retList)
                    {
                        j++;
                        for (int y = 0; y < iColCount; y++)
                        {
                            result[j, y] = dr[y];
                        }
                    }

                }
                else
                {
                    result = new object[,] { { "Selection was not a range or a number, but " + selectionContent.ToString() } };
                }
                // Now create the target reference that will refer to leg, getting a reference that contains the SheetId first  
                ExcelReference sheet2 = (ExcelReference)XlCall.Excel(XlCall.xlSheetId, "leg");

                Xl.Range Legidrng = legsheet.get_Range(LegIDrngName, System.Type.Missing);
                ExcelReference legidtarget = new ExcelReference(Legidrng.Row - 1, Legidrng.Row - 1, Legidrng.Column - 1, Legidrng.Column - 1, sheet2.SheetId);
                legidtarget.SetValue(legidval);

                Xl.Range Legcurrency = legsheet.get_Range(legccy, System.Type.Missing);
                ExcelReference currencytarget = new ExcelReference(Legcurrency.Row - 1, Legcurrency.Row - 1, Legcurrency.Column - 1, Legcurrency.Column - 1, sheet2.SheetId);
                currencytarget.SetValue(ccy);

                Xl.Range disctargetrange = legsheet.get_Range(discaddr, System.Type.Missing);
                ExcelReference disctarget = new ExcelReference(disctargetrange.Row - 1, disctargetrange.Row - 1, disctargetrange.Column - 1, disctargetrange.Column - 1, sheet2.SheetId);
                disctarget.SetValue(DiscId);

                int resultRows = result.GetLength(0);
                int resultCols = result.GetLength(1);
                ExcelReference target = new ExcelReference(Leg_Analysis.Row - 1, resultRows + Leg_Analysis.Row - 2, Leg_Analysis.Column - 1, resultCols + Leg_Analysis.Column - 2, sheet2.SheetId);

                // Finally setting the result into the target range.     
                target.SetValue(result);

            }
            catch (Exception e)
            {
                QLAddin.logError(selectionContent as string, "LegAnalysisImpl", e.Message);
            }
            */
        }

        public static void LegDisplay()
        {
            ExcelReference selection = (ExcelReference)XlCall.Excel(XlCall.xlfSelection);

            legdisplayImpl(selection, "LegID", "Leg_Analysis", "LastRange", "legCCY", "DISCIDX");
        }

        public static void SwapDisplayRowBased()
        {
            Xl._Application xlApp = (Xl._Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            Xl._Workbook oWB = (Xl.Workbook)xlApp.ActiveWorkbook;
            Xl._Worksheet sheet = (Xl._Worksheet)xlApp.ActiveSheet;
            Xl.Range range1 = sheet.get_Range("swapleg1", System.Type.Missing);

            ExcelReference selection = (ExcelReference)XlCall.Excel(XlCall.xlfSelection);

            ExcelReference leg1selection = new ExcelReference(selection.RowFirst, range1.Column-1);
            legdisplayImpl(leg1selection, "LegID", "Leg_Analysis", "LastRange", "legCCY", "DISCIDX");

            ExcelReference leg2selection = new ExcelReference(selection.RowFirst, range1.Column);
            legdisplayImpl(leg2selection, "Leg2Id", "Leg2_Analysis", "LastRange2", "leg2CCY", "leg2_DISCIDX");
        }

        public static void SwapDisplay()
        {
            Xl._Application xlApp = (Xl._Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            Xl._Workbook oWB = (Xl.Workbook)xlApp.ActiveWorkbook;
            Xl._Worksheet sheet = (Xl._Worksheet)xlApp.ActiveSheet;
            Xl.Range range1 = sheet.get_Range("swapleg1", System.Type.Missing);

            ExcelReference selection = (ExcelReference)XlCall.Excel(XlCall.xlfSelection);

            ExcelReference leg1selection = new ExcelReference(range1.Row - 1, selection.ColumnFirst);
            legdisplayImpl(leg1selection, "LegID", "Leg_Analysis", "LastRange", "legCCY", "DISCIDX");

            Xl.Range range2 = sheet.get_Range("swapleg2", System.Type.Missing);
            ExcelReference leg2selection = new ExcelReference(range2.Row - 1, selection.ColumnFirst);
            legdisplayImpl(leg2selection, "Leg2Id", "Leg2_Analysis", "LastRange2", "leg2CCY", "leg2_DISCIDX");
        }
        #endregion
    }
}

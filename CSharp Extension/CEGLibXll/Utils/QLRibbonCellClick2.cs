using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using Xl = Microsoft.Office.Interop.Excel;
using System.Runtime.InteropServices;
using ExcelDna.Integration;
using ExcelDna.Integration.CustomUI;
using ExcelDna.Logging;

namespace QLXLL
{
    public partial class QLRibbon : ExcelRibbon
    {
        static CommandBars bars = ExcelCommandBarUtil.GetCommandBars();
        static CommandBar popUp = bars["Cell"];
        static CommandBarPopup pctrl = (CommandBarPopup)popUp.Controls.Add(MsoControlType.msoControlPopup, Type.Missing, Type.Missing, 1, true);

        static CommandBarControl Menu_Fwd;
        static CommandBarControl Menu_Disc;
        static CommandBarControl Menu_PV01;
        static CommandBarControl Menu_Leg;

        public static void set_Menu_Invisible()
        {
            Menu_Fwd.Enabled = false;
            Menu_Fwd.Visible = false;

            Menu_Disc.Enabled = false;
            Menu_Disc.Visible = false;

            Menu_PV01.Enabled = false;
            Menu_PV01.Visible = false;

            Menu_Leg.Enabled = false;
            Menu_Leg.Visible = false;
        }

        public static string cell_category()
        {
            Xl.Application oXl = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            if (oXl.Workbooks.Count == 0)
                return "invalid";

            try
            {
                Xl.Workbook oWB = (Xl.Workbook)oXl.ActiveWorkbook;
                Xl.Worksheet oWS = (Xl.Worksheet)oWB.ActiveSheet;
                Xl.Range oWR = (Xl.Range)oWS.Application.Selection;

                // loop through multiple selected cells, return a valid cell click type once first valid one found.
                foreach (Xl.Range cell in oWR)
                {
                    string cellstr = cell.Text.ToString();

                    // check BCurve first!
                    // for BCurve object("CRV@..."), only plot discount curve
                    if (OHRepository.Instance.containsObject(cellstr) && ((Type)OHRepository.Instance.getObjectType(cellstr) == typeof(QuantLib.DiscountCurve)))
                    {
                        return "disc";
                    }
                    // for BIborIndex object, plot either forward curve or discount curve
                    if (OHRepository.Instance.containsObject(cellstr) && ((Type)OHRepository.Instance.getObjectType(cellstr) == typeof(QuantLib.IborIndex)))
                    {
                        return "fwd_disc";
                    }
                    if ((oWS.get_Range("swapID").Count > 0) && (cell.Row == oWS.get_Range("swapID").Row))
                    {
                        return "swap";
                    }
                    if (OHRepository.Instance.containsObject(cellstr) && ((Type)OHRepository.Instance.getObjectType(cellstr) == typeof(QuantLib.Leg)))
                    {
                        return "leg";
                    }
                }
                return "invalid";
            }
            catch (Exception)
            {
                return "";
            }
        }

        public static void QL_Action_Click()
        {
            string click_type = cell_category();

            try
            {
                // have to do it here!
                set_Menu_Invisible();

                switch (click_type)
                {
                    case "fwd_disc":
                        Menu_Fwd.Enabled = true;
                        Menu_Fwd.Visible = true;
                        break;
                    case "disc":
                        Menu_Disc.Enabled = true;
                        Menu_Disc.Visible = true;
                        break;
                    case "swap":
                        Menu_PV01.Enabled = true;
                        Menu_PV01.Visible = true;
                        break;
                    case "leg":
                        Menu_Leg.Enabled = true;
                        Menu_Leg.Visible = true;
                        break;
                    case "invalid":
                        break;
                }
            }
            catch (Exception)
            {
            }
        }

        // this is invoked from commandbar
        public static void Cell_Error_Click()
        {
            Xl.Application xlApp = (Xl.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            Xl.Workbook oWB = (Xl.Workbook)xlApp.ActiveWorkbook;
            Xl.Worksheet sheet = (Xl.Worksheet)xlApp.ActiveSheet;
            Xl.Range range = sheet.Application.ActiveCell;
            range.Calculate();

            string address = QLAddin.getSelectCellAddress();
            if (range.Text != null)
            {
                //LogDisplay.ShowThisSheet(address);
                LogDisplay.Show();
            }
        }

        public static void cell_plot_chart(Xl.Worksheet oWS, string Text, object[] curveDateStr, string[] curveStr, double[,] curveValue,
            double Left, double Top, double Width, double Height)
        {
            try
            {
                Xl.ChartObjects xlCharts = (Xl.ChartObjects)oWS.ChartObjects(Type.Missing);
                Xl.ChartObject myChart = (Xl.ChartObject)xlCharts.Add(Left, Top, Width, Height);
                Xl.Chart chartPage = myChart.Chart;
                Xl.SeriesCollection sc = chartPage.SeriesCollection();
                Xl.Series s;

                chartPage.ChartType = Xl.XlChartType.xlLine;
                chartPage.HasTitle = true;
                chartPage.ChartTitle.Text = Text;

                int nDate = curveValue.GetLength(0);
                int nCrv = curveValue.GetLength(1);
                for (Int16 i = 0; i < nCrv; i++)
                {
                    s = sc.NewSeries();
                    s.MarkerStyle = Xl.XlMarkerStyle.xlMarkerStyleNone;
                    s.Name = curveStr[i];
                    s.XValues = curveDateStr;

                    double[] curveValueOne = new double[nDate];
                    for (Int16 j = 0; j < nDate; j++)
                        curveValueOne[j] = curveValue[j, i];
                    s.Values = curveValueOne;
                }
            }
            catch (Exception)
            {
            }
        }

        public static void Cell_PlotDiscCurve_Click()
        {
            try
            {
                Xl._Application oXl = (Xl._Application)ExcelDna.Integration.ExcelDnaUtil.Application;
                Xl.Workbook oWB = (Xl.Workbook)oXl.ActiveWorkbook;
                Xl.Worksheet oWS = (Xl.Worksheet)oWB.ActiveSheet;
                Xl.Range oWR = (Xl.Range)oWS.Application.Selection;

                String cellstr;
                int nCrv = 0;
                List<String> PlotCrvSetString = new List<string>();
                List<DateTime> list = new List<DateTime>();

                // get common date set of multiple discount curves (distinct join)
                foreach (Xl.Range cell in oWR)
                {
                    cellstr = cell.Text.toString();

                    if (OHRepository.Instance.containsObject(cellstr)
                        && ((Type)OHRepository.Instance.getObjectType(cellstr) == typeof(QuantLib.YieldTermStructureHandle))
                            || ((Type)OHRepository.Instance.getObjectType(cellstr) == typeof(QuantLib.IborIndex)))
                    {
                        nCrv++;
                        QuantLib.YieldTermStructureHandle bcurve = null;

                        if ((Type)OHRepository.Instance.getObjectType(cellstr) == typeof(QuantLib.IborIndex))
                        {
                            QuantLib.IborIndex idxobj = OHRepository.Instance.getObject<QuantLib.IborIndex>(cellstr);
                            bcurve = idxobj.forwardingTermStructure();
                        }
                        else
                        {
                            Xl.Range rng = QLAddin.getActiveCellRange();
                            bcurve = OHRepository.Instance.getObject<QuantLib.YieldTermStructureHandle>(cellstr);
                        }

                        // double[] tmpDays = bcurve.getDays();
                        double[] tmpDays = new double[0];
                        DateTime[] curveDateOne = new DateTime[tmpDays.Length];
                        for (int i = 0; i < tmpDays.Length; i++)
                            curveDateOne[i] = DateTime.Today.AddDays(tmpDays[i]);
                        list.AddRange(curveDateOne);
                        PlotCrvSetString.Add(cellstr);
                    }
                }
                list = list.Distinct().ToList();
                list.Sort((a,b) => a.CompareTo(b));

                DateTime[] curveDate_ = list.ToArray();
                int nDate = curveDate_.Length;
                if (nDate == 0)
                    throw new Exception("Discount curves need to have at least one date point. ");

                // prepare curve values
                object[,] curveDate = new object[nDate, 1];
                object[] curveDateStr = new object[nDate];
                string[] curveStr = PlotCrvSetString.ToArray();
                double[,] curveValue = new double[nDate, nCrv];
                for (int i = 0; i < nDate; i++)
                {
                    curveDate[i,0] = curveDate_[i].ToOADate();
                    curveDateStr[i] = curveDate_[i];
                }

                for (int j = 0; j < nCrv; j++)
                {
                    if (QLAddin.isQLError(curveStr[j]))
                        throw new Exception("Index Id invalid");

                    // double[,] tmpobj = (double[,])qlCalulator.getDiscountFactor(curveStr[j], curveDate);
                    double[,] tmpobj = new double[0,0];
                    for (int i = 0; i < nDate; i++)
                    {
                        curveValue[i,j] = tmpobj[i,0];
                    }
                }

                // plot
                if (nCrv > 0)
                    cell_plot_chart(oWS, "Discount Curve".ToString(), curveDateStr, curveStr, curveValue, oWR.Left+120, oWR.Top+90, 800, 250);
            }
            catch (Exception)
            {
            }
        }


        public static void Cell_PlotFwdCurve_Click()
        {
            try
            {
                Xl._Application oXl = (Xl._Application)ExcelDna.Integration.ExcelDnaUtil.Application;
                Xl.Workbook oWB = (Xl.Workbook)oXl.ActiveWorkbook;
                Xl.Worksheet oWS = (Xl.Worksheet)oWB.ActiveSheet;
                Xl.Range oWR = (Xl.Range)oWS.Application.Selection;

                String cellstr;
                String cellstrCenter;

                QuantLib.Frequency freq = (QuantLib.Frequency)1;
                int nCrv = 0;
                List<QuantLib.IborIndex> PlotCrvSetString = new List<QuantLib.IborIndex>();
                List<String> CrvStr = new List<String>();
            }
            catch (Exception)
            {
            }
        }

        public static void Cell_ShowPV01_Click()
        {

        }

        public static void Cell_DispLeg_Click()
        {

        }

    }
}

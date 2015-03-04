using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ExcelDna.Integration;
using ExcelDna.Integration.Rtd;
using Xl = Microsoft.Office.Interop.Excel;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using QuantLib;


namespace CEGLibXll
{
    public class SimCube
    {
        [ExcelFunction(Description = "query CECDBP-MSW-01", Category = "CEGLibXll - Database")]
        public static object cegSimCubGetRAFT(
            [ExcelArgument(Description = "Date ")]DateTime AsOfDate)
        {
            if (SystemUtil.CallFromWizard())
                return "";

            string callerAddress = "";
            try
            {
                callerAddress = SystemUtil.getActiveCellAddress();

                OHRepository.Instance.removeErrorMessage(callerAddress);
            }
            catch (Exception)
            {
            }
            try
            {
                if (AsOfDate == DateTime.MinValue)
                    throw new Exception("AsOfDate must not be empty. ");
                // QuantLib.Date d = Conversion.ConvertObject<QuantLib.Date>(AsOfDate, "date");

                DataTable dataTable = new DataTable();
                object[,] ret;

                using (SqlConnection conn = new SqlConnection(CEGLib.SimCube.connectingString))
                {
                    SqlCommand cmd = new SqlCommand("RptCollateral", conn);
                    cmd.CommandType = CommandType.StoredProcedure;
                    // cmd.Parameters.Add("@TaskName", SqlDbType.NVarChar, 50).Value = t;
                    cmd.Parameters.Add(new SqlParameter("@DataAsOfDate", AsOfDate.ToString("yyyy-MM-dd")));
                    conn.Open();

                    // create data adapter
                    SqlDataAdapter da = new SqlDataAdapter(cmd);
                    // this will query your database and return the result to your datatable
                    da.Fill(dataTable);
                    da.Dispose();

                    ret = CEGLib.APrime.DumpTableToObject(dataTable);
                }

                return ret;
            }
            catch (Exception e)
            {
                SystemUtil.logError(callerAddress, System.Reflection.MethodInfo.GetCurrentMethod().Name.ToString(), e.Message);
                return e.Message;
            }
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ExcelDna.Integration;
using ExcelDna.Integration.Rtd;
using Xl = Microsoft.Office.Interop.Excel;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using System.Data;
using System.Data.SqlClient;
using QuantLib;

namespace CEGLibXll
{
    public class APrime
    {
        [ExcelFunction(Description = "query APrime_Base_Agg", Category = "CEGLibXll - Database")]
        public static object cegAPrimeGetDeals(
            [ExcelArgument(Description = "Date ")]DateTime AsOfDate,
            [ExcelArgument(Description = "Counterparty Name ")]string counterparty,
            [ExcelArgument(Description = "Agreement ID ")]string agreement)
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
                QuantLib.Date d = Conversion.ConvertObject<QuantLib.Date>(AsOfDate, "date");

                string sql = @"select * from RMGSAS.APRIME_BASE_AGG where asofdate = to_date('" + AsOfDate.ToString("yyyy-MM-dd") + "', 'YYYY-MM-DD')";

                if (! string.IsNullOrEmpty(counterparty))
                {
                    sql += " and counterparty='" + counterparty.ToUpper() + "'";
                }
                else if (! string.IsNullOrEmpty(agreement))
                {
                    sql += " and credit_nettingagreement = 'Agreement " + agreement.ToUpper() + "'";
                }
                else
                {
                    return "Counterpary or AgreementId not assigned";
                }

                object[,] ret;
                using (OracleConnection conn = new OracleConnection(CEGLib.APrime.connectingString))
                {
                    conn.Open();
                    OracleCommand cmd = new OracleCommand();
                    cmd.Connection = conn;
                    cmd.CommandText = sql;
                    cmd.CommandType = System.Data.CommandType.Text;     // or stored procedure

                    OracleDataAdapter adapter = new OracleDataAdapter(cmd);
                    adapter.ReturnProviderSpecificTypes = true;

                    DataTable resultsTable = new DataTable();
                    int nrows = adapter.Fill(resultsTable);
                    ret = CEGLib.APrime.DumpTableToObject(resultsTable);
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

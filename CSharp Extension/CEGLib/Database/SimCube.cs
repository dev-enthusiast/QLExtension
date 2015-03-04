using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using System.Data;
using System.Data.SqlClient;

namespace CEGLib
{
    public class SimCube
    {
        public static string connectingString = "Data Source=CECDBP-MSW-01;" + "Initial Catalog=EnergyCredit;" +
            "User id=ExecSpOnly;" + "Password=EnergyCredit_ExecSPOnly;";

        public static DataTable GetRAFTForLiquidityStress(DateTime asOfDate)
        {
            DataTable dataTable = new DataTable();

            SqlConnection conn = new SqlConnection(connectingString);
            SqlCommand cmd = new SqlCommand("RptCollateral", conn);
            cmd.CommandType = CommandType.StoredProcedure;
            // cmd.Parameters.Add("@TaskName", SqlDbType.NVarChar, 50).Value = t;
            cmd.Parameters.Add(new SqlParameter("@DataAsOfDate", "2015-01-02"));
            conn.Open();

            // create data adapter
            SqlDataAdapter da = new SqlDataAdapter(cmd);
            // this will query your database and return the result to your datatable
            da.Fill(dataTable);
            conn.Close();
            da.Dispose();

            return dataTable;
        }
    }
}

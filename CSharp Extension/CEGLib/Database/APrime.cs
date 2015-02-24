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
    public static class APrimeDB
    {
        static string connectingStrAPrime = "Data Source=rmgdbp;User Id=rmgdb_read;Password=rmgdb_read;";
        static string connectingStrSqlServer = "Data Source=CECDBP-MSW-01;" + "Initial Catalog=EnergyCredit;" +
            "User id=ExecSpOnly;" + "Password=EnergyCredit_ExecSPOnly;";

        public static DataTable GetAPrimeDataForLiquidityStress(DateTime asOfDate, DateTime TerminationDate, string filter = "")
        {
            string dateformat = "yyyy-MM-dd";
            string defaultfilter = @"risk_reporting_flag = 'Y' AND";

            DataTable resultsTable = new DataTable();
            DataTable returnTable = new DataTable();
            try
            {
                string fromtable = @"select (CASE WHEN ContractMonth > to_date('" + TerminationDate.ToString(dateformat) + "','YYYY-MM-DD') THEN Add_Months(ContractMonth, -(12*(to_char(ContractMonth,'YYYY') - to_char(to_date('" + TerminationDate.ToString(dateformat) + "', 'YYYY-MM-DD') ,'YYYY') ))) ELSE ContractMonth END) AS ContractMonthAdj"
                    + ", NVL((  CASE    WHEN commodity_grp IN ('POWER', 'GAS', 'DOMESTIC COAL', 'INTERNATIONAL COAL') THEN mwh_pos END),0) AS MWHeMgnble"
                    + ", Counterparty"
                    + ", commod_curve"
                    + ", MTM"
                    + ", Credit_NettingAgreement"
                    + ", position"
                    + ", Exposure"
                    + ", CashPost"
                    + ", LCPOST"
                    + ", CASHHELD"
                    + ", LCHELD"
                    + ", forecastdnp"           // Positive means ctpy's colalteral to CEG
                    + ", MARGINAL"              // 'Marginal' means Marginable 
                    + ", MARGINED"              // 'Margined=Y' & 'Marginal' means actually margined
                    + ", PRICE"
                    + ", REGION_BU"
                    + ", GLT"
                    + ", MWH_POS"
                    + " from RMGSAS.aprime_base_agg";

                string sql = @"SELECT ContractMonthAdj as ContractMonth "
                    + ", sum(MWHeMgnble) as MWHeMgnble"
                    + ", Counterparty" 
                    + ", commod_curve as Commod_Curve"
                    + ", sum(Exposure) as Exposure"
                    + ", Credit_NettingAgreement"
                    + ", sum(position) as Position"
                    + ", sum(CashPost) as CashPost"
                    + ", sum(LCPOST) as LCPost"
                    + ", sum(CASHHELD) as CashHeld"
                    + ", sum(LCHELD) as LCHeld"
                    + ", sum(MTM) as MTM"
                    + ", sum(forecastdnp) as ForecastDNP"
                    + ", MARGINAL as Marginal"
                    + ", MARGINED as Margined"
                    + ", PRICE as Price"
                    + ", REGION_BU as Region_Bu"
                    + ", GLT" 
                    + ", sum(MWH_POS) as MWH_Pos"
                    + " FROM(" + fromtable
                    + " where " + (filter == "" ? defaultfilter : filter)
                    + " AsofDate = to_date('" + asOfDate.ToString(dateformat) + "', 'YYYY-MM-DD'))"
                    + " Where rownum <= 1000"               // For test
                    + " GROUP BY Counterparty, Credit_NettingAgreement, commod_curve, ContractMonthAdj, MARGINAL, MARGINED, PRICE, REGION_BU, GLT"
                    + " ORDER BY Counterparty, Credit_NettingAgreement";

                using (OracleConnection conn = new OracleConnection(connectingStrAPrime))
                {
                    conn.Open();
                    OracleCommand cmd = new OracleCommand();
                    cmd.Connection = conn;
                    cmd.CommandText = sql;
                    cmd.CommandType = System.Data.CommandType.Text;     // or stored procedure

                    OracleDataAdapter adapter = new OracleDataAdapter(cmd);
                    // adapter.SafeMapping.Add("*", typeof(byte[]));
                    adapter.ReturnProviderSpecificTypes = true;
                    
                    // OracleCommandBuilder cb = new OracleCommandBuilder(adapter);
                    // adapter.FillSchema(resultsTable, SchemaType.Source);
                    // resultsTable.Columns[0].DataType = typeof(DateTime);
                    var rows = adapter.Fill(resultsTable);

                    // foreach (DataColumn c in resultsTable.Columns)
                    //{
                    //    Console.WriteLine(c.DataType);
                    //}

                    // conn.Dispose();

                    // return DataTable
                    returnTable.Columns.Add("ContractMonth", typeof(DateTime));
                    returnTable.Columns.Add("MWHeMgnble", typeof(double));
                    returnTable.Columns.Add("Counterparty", typeof(string));
                    returnTable.Columns.Add("Commod_Curve", typeof(string));
                    returnTable.Columns.Add("Exposure", typeof(double));
                    returnTable.Columns.Add("Credit_NettingAgreement", typeof(string));
                    returnTable.Columns.Add("Position", typeof(double));
                    returnTable.Columns.Add("CashPost", typeof(double));
                    returnTable.Columns.Add("LCPost", typeof(double));
                    returnTable.Columns.Add("CashHeld", typeof(double));
                    returnTable.Columns.Add("LCHeld", typeof(double));
                    returnTable.Columns.Add("MTM", typeof(double));
                    returnTable.Columns.Add("ForecastDNP", typeof(double));
                    returnTable.Columns.Add("Marginal", typeof(string));
                    returnTable.Columns.Add("Margined", typeof(string));
                    returnTable.Columns.Add("Price", typeof(double));
                    returnTable.Columns.Add("Region_Bu", typeof(string));
                    returnTable.Columns.Add("GLT", typeof(string));
                    returnTable.Columns.Add("MWH_Pos", typeof(double));

                    DateTime contractMonth;                 // 0
                    double MWHeMgnble;                      // 1
                    string Counterparty;                    // 2
                    string Commod_Curve;                    // 3
                    double Exposure;                        // 4
                    string Credit_NettingAgreement;         // 5
                    double Position;                        // 6
                    double CashPost;                        // 7
                    double LCPost;                          // 8
                    double CashHeld;                        // 9
                    double LCHeld;                          // 10
                    double MTM;                             // 11
                    double ForecastDNP;                     // 12
                    string Marginal;                        // 13
                    string Margined;                        // 14
                    double Price;                           // 15
                    string Region_Bu;                       // 16
                    string GLT;                             // 17
                    double MWH_Pos;                         // 18

                    DateTime bomDate = new DateTime(asOfDate.Year, asOfDate.Month, 1);
                    foreach (DataRow row in resultsTable.Rows)
                    {
                        if (!DateTime.TryParse(row[0].ToString(), out contractMonth))
                            contractMonth = bomDate;

                        if (!double.TryParse(row[1].ToString(), out MWHeMgnble))
                            MWHeMgnble = 0;
                        Counterparty = row[2].ToString();
                        Commod_Curve = row[3].ToString();

                        if (!double.TryParse(row[4].ToString(), out Exposure))
                            Exposure = 0;
                        Credit_NettingAgreement = row[5].ToString();
                        if (!double.TryParse(row[6].ToString(), out Position))
                            Position = 0;
                        if (!double.TryParse(row[7].ToString(), out CashPost))
                            CashPost = 0;
                        if (!double.TryParse(row[8].ToString(), out LCPost))
                            LCPost = 0;
                        if (!double.TryParse(row[9].ToString(), out CashHeld))
                            CashHeld = 0;
                        if (!double.TryParse(row[10].ToString(), out LCHeld))
                            LCHeld = 0;
                        if (!double.TryParse(row[11].ToString(), out MTM))
                            MTM = 0;
                        if (!double.TryParse(row[12].ToString(), out ForecastDNP))
                            ForecastDNP = 0;
                        
                        Marginal = row[13].ToString();
                        Margined = row[14].ToString();
                        if (!double.TryParse(row[15].ToString(), out Price))
                            Price = 0;
                        Region_Bu = row[16].ToString();
                        GLT = row[17].ToString();
                        if (!double.TryParse(row[18].ToString(), out MWH_Pos))
                            MWH_Pos = 0;

                        //******* Initial Adjustment *******************//
                        // fill out zeros for empty cells
                        int emptyCounterparty = 1;
                        if (contractMonth.CompareTo(bomDate) < 0)      // earlier than bomDate
                        {
                            contractMonth = bomDate;
                            ForecastDNP = 0;
                        }

                        if (string.IsNullOrEmpty(Commod_Curve))
                        {
                            Commod_Curve = "COMMOD NG EXCHANGE";
                            Position = 0;
                        }

                        if (string.IsNullOrEmpty(Counterparty))
                        {
                            Counterparty = "Counterparty.NA" + (emptyCounterparty++);
                        }

                        if (string.IsNullOrEmpty(Credit_NettingAgreement))
                        {
                            Credit_NettingAgreement = "Agreement " + Counterparty;
                        }

                        returnTable.Rows.Add(contractMonth, MWHeMgnble, Counterparty, Commod_Curve, Exposure,
                                    Credit_NettingAgreement, Position, CashPost, LCPost,
                                    CashHeld, LCHeld, MTM, ForecastDNP, Marginal, Margined,
                                    Price, Region_Bu, GLT, MWH_Pos);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

            return returnTable;
        }

        public static DataTable GetRAFTForLiquidityStress(DateTime asOfDate)
        {
            DataTable dataTable = new DataTable();

            SqlConnection conn = new SqlConnection(connectingStrSqlServer);
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

        public static void DumpDataTableToCSV(DataTable dt, string filename)
        {
            if (System.IO.File.Exists(filename))
                System.IO.File.Delete(filename);

            StringBuilder sb = new StringBuilder();

            IEnumerable<string> columnNames = dt.Columns.Cast<DataColumn>().
                                              Select(column => column.ColumnName);

            sb.AppendLine(string.Join(",", columnNames));

            int nrow = 0;
            foreach (DataRow row in dt.Rows)
            {
                IEnumerable<string> fields = row.ItemArray.Select(field => field.ToString());
                sb.AppendLine(string.Join(",", fields));
                nrow++;
                if (nrow == 10000)
                {
                    System.IO.File.AppendAllText(filename, sb.ToString());
                    sb.Clear();
                    nrow = 0;
                }
            }

            System.IO.File.AppendAllText(filename, sb.ToString());
        }
    }
}

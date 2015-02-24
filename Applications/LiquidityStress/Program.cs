using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;

using CEGLib;

namespace LiquidityStress
{
    class Program
    {
        static void Main(string[] args)
        {
            DateTime asOfDate = new DateTime(2015, 1, 30);
            DateTime terminationDate = new DateTime(2023, 12, 31);
            DataTable aprimeTable = APrimeDB.GetAPrimeDataForLiquidityStress(asOfDate, terminationDate);

            APrimeDB.DumpDataTableToCSV(aprimeTable, "aprime_raw.csv");

            // Get Counterparty/Agreement table
            var agreementTable = aprimeTable.AsEnumerable().GroupBy(row => new { Counterparty = row.Field<string>("Counterparty"), Credit_NettingAgreement = row.Field<string>("Credit_NettingAgreement"), ContractMonth = row.Field<DateTime>("ContractMonth") })
                .Select(group => new { Counterparty = group.Key.Counterparty, Credit_NettingAgreement = group.Key.Credit_NettingAgreement, ContractMonth = group.Key.ContractMonth, Marginal = group.Min(row => row.Field<string>("Marginal")), MTM = group.Sum(row => row.Field<double>("MTM")) });


            ClassifyCollateral();
        }

        static void ClassifyCollateral()
        {

        }
    }
}

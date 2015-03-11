using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Exelon.Data.SecDb;

namespace CEGLib.SecDb
{
    public class SecDb
    {
        public SecDb()
        {
        }

        #region database interaction

        public static string GetCurrentRootDatabaseName()
        {
            return SecDbAPI.Database.CurrentSecurityDatabase;
        }

        public static string GetRootDatabaseSearchPath()
        {
            return SecDbAPI.Database.SecDbPath;
        }

        public static void SetRootDatabaseSearchPath(string path = "BAL Production;BAL Local;")
        {
            SecDbAPI.Database.SecDbPath = path;
        }

        public static string GetSourceDatabase()
        {
            return SecDbAPI.Database.SecDbSourceDb;
        }

        public static void SetSourceDatabase(string path = "BAL DevSource;")
        {
            SecDbAPI.Database.SecDbSourceDb = path;
        }

        public static void Rest()
        {
            SecDbAPI.Database.Transaction trans = new SecDbAPI.Database.Transaction("transaction name");
            trans.Begin();
            // do something
            trans.Commit();
            trans.Abort();


            string transitions = SecDbAPI.Database.Transaction.SecDbTransactionCurrent();
        }

        public static List<string> GetUncommittedTransactions()
        {
            string trans = SecDbAPI.Database.Transaction.SecDbTransactionCurrent();

            string[] ret = trans.Split(';');
            return new List<string>(ret);
        }
        #endregion

        #region Slang interaction
        public static Exelon.Data.SecDb.DataType.DtValue EvaluateSlang(string slangcode)
        {
            Exelon.Data.SecDb.DataType.DtValue result = SecDbAPI.SlangEvalExpr(slangcode);
            return result;
        }
        #endregion

        #region Util
        #endregion
    }
}

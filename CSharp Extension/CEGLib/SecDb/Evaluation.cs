using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CEGLib.SecDb;

namespace CEGLib.SecDb
{
    public class Evaluation
    {
        public int GetConnection(string application, string username, string databaase, string sourcedb = "", int deadpoolsize = -1)
        {
            if (string.IsNullOrEmpty(databaase))
                databaase = "Test";

            int ret = -1;
            try
            {
                ret = (int)Uberglue.Connect("cegLib", username, databaase, sourcedb, deadpoolsize);

                // Remove the top level scope from the stack.  We always push a new
                // scope on to the stack before we evaluate code, so we never use the
                // top level scope that was constructed by uberglue.Connect.  See
                // _exec_and_eval_slang_expression below.
                // uberglue.PopVariableScope()
            }
            catch (Exception e)
            {
            }
            return ret;
        }

        public int GetLastError(ref Uberglue.Result result)
        {
            return (int)Uberglue.LastError(result);
        }
    }
}

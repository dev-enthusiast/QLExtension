using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CEGLib.SecDb;

namespace CEGLib.SecDb
{
    public class Interface
    {
        public static int Connect(string application, string username, string databaase = "Test", string sourcedb = "", int deadpoolsize = -1)
        {
            if (string.IsNullOrEmpty(application))
                application = "python";

            int ret = -1;
            try
            {
                ret = (int)Uberglue.Connect(application, username, databaase, sourcedb, deadpoolsize);

                // Remove the top level scope from the stack.  We always push a new
                // scope on to the stack before we evaluate code, so we never use the
                // top level scope that was constructed by uberglue.Connect.  See
                // _exec_and_eval_slang_expression below.
                Uberglue.PopVariableScope();
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message + " ret = " + ret );
            }
            return ret;
        }

        public void TSDBRead()
        {

        }

        public static void GetSymbolInfo()
        {

        }

        public static void GetValueType()
        {

        }

        /// <summary>
        /// Evaluate a Slang expression and return the result.
        /// The expression is evaluated in a new anonymous scope.  
        /// Any keyword arguments are bound to Slang variables before evaluation.
        /// </summary>
        /// <param name="slangcode"></param>
        /// An instance of DtValue or one of its subclasses that contains the
        /// result of the Slang expression.
        public static void EvaluateSlang(string slangcode, Dictionary<string, double> keywordargs = null)
        {
            SlangVariableScope scope = new LocalSlangVariableScope("evaluate_slang");
            foreach(var kvp in keywordargs)
            {
                scope[kvp.Key] = kvp.Value;
            }

            return scope.eval(slangcode);
        }

        public static SlangVariableScope ExecuteSlang(string slangcode, Dictionary<string, double> keywordargs = null)
        {
            SlangVariableScope scope = new LocalSlangVariableScope("execute_slang");
            foreach (var kvp in keywordargs)
            {
                scope[kvp.Key] = kvp.Value;
            }

            scope.eval(slangcode);
            return scope;
        }

        public static void CallFunction()
        {

        }
        /// <summary>
        /// Evaluate a Slang expression and return the result.
        /// The expression is evaluated in a new anonymous scope.  Any keyword
        /// arguments are bound to Slang variables before evaluation.
        /// </summary>
        /// <param name="text"></param>
        /// <returns>An instance of DtValue or one of its subclasses that contains the result of the Slang expression.</returns>
        public int GetSlangEvaluation(string text)
        {

        }

        public static string GetLastError()
        {
            StringHandle result = new StringHandle();
            int rcode = (int)Uberglue.LastError(result);
            return result.content;
        }
    }
}

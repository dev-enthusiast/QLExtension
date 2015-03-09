using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Text;

namespace CEGLib.SecDb
{
    public class Security : DtValue
    {
        public static string FindClasses(string expression)
        {
            DtValue res = new DtValue();
            Uberglue.ListSecurityClassNames(res.chandle_);

            Regex regex = new Regex(expression);
            // get all values in array
            Match match = regex.Match("");

            if (match.Success)
            {
                return match.Value;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="database">database (Database, None): database to search, None corresponds to the current SecDb root dictionary.</param>
        /// <param name="classname">class_name (str, None): If given, the result is restricted to objects of this class.</param>
        /// <param name="prefix">prefix (str, None): If given, only returns security names that begin with prefix.  The string comparision is case-insensitive.</param>
        /// <param name="regex">regex (str, None): If given, only returns security names that match the given regular expression.  Regular expressions follow the
        /// syntax of the re module, and the comparision is case-insensitive (see re.IGNORECASE).</param>
        /// <param name="chunksize">chunk_size (int, None): Security names are retrieved from SecDb in batches.  
        /// If this argument is provided, it overrides the default batch size.  Modifying this paramter could have significant perform implications.</param>
        /// <param name="max_size">max_size (int, None): If provided, an SecDbLargeResultError error is thrown when the iterator increments pass the max_size'th element.
        /// This is mainly provided as convenience for user; its quite easy to hang a script by attempting a iterate over too large of a set.
        /// If max_size is None, no check is performed and the iterate will eventually reach every name in the set.</param>
        public static void FindSecurities(Database database, string classname, string prefix="", string regex = "", int chunksize = 5000, int max_size = 50*1000)
        {
            try
            {
                // Uberglue.FindSecurities();
            }
            catch
            {

            }
        }

        public void GetSecurity(string name)
        {
            try
            {
                Security security = new security();
                Uberglue.GetSecurity();
            }
            catch
            {

            }
        }

        public void SecurityType()
        {
            Interface.EvaluateSlang("'Security Type( sec )", sec = self);
        }

        public void ValueTypes()
        {
            Uberglue.Security_ListValueTypeNames();
        }
    }

    public class Database : DtValue
    {
        public static void GetSecurity(string name)
        {
            Uberglue.GetSecurity();
        }

        public static Security NewSecurity(string classname, string securityname="")
        {
            Uberglue.NewSecurity();
        }

        public static void FindSecurities()
        { }
    }

    public class ValueTypeInfo : DtValue
    {

    }
}

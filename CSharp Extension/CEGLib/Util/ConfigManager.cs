using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CEGLib
{
    public class ConfigManager
    {
        public delegate void VoidStringDelegate(string msg);
        public static VoidStringDelegate debughandler_;

        public static void Debug(string msg)
        {
            if (debughandler_ != null)
            {
                debughandler_(msg);
            }
        }
    }
}

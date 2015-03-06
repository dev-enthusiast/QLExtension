using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CEGLib
{
    public class RInterface
    {
        public struct primsxp_struct
        {
            int offset;
        };

        struct symsxp_struct {
            IntPtr pname;
            IntPtr value;
            IntPtr intrnl;
        };

        struct listsxp_struct
        {
            IntPtr carval;
            IntPtr cdrval;
            IntPtr tagval;
        };

        struct envsxp_struct
        {
            IntPtr frame;
            IntPtr enclos;
            IntPtr hashtab;
        };
        struct closxp_struct
        {
            IntPtr formals;
            IntPtr body;
            IntPtr env;
        };
        struct promsxp_struct
        {
            IntPtr value;
            IntPtr expr;
            IntPtr env;
        };
    }
}

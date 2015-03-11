using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace CEGLib.R
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct SEXPREC_HEADER
    {
        public sxpinfo sxpinfo;
        public IntPtr attrib;
        public IntPtr gengc_next_node;
        public IntPtr gengc_prev_node;
    }
}

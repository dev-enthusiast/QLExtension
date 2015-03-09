using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CEGLib.SecDb
{
    public class Date : DtValue
    {
        public Date()
        {
            Uberglue.DtValue_GetNumber();
        }
    }

    public class Time : DtValue
    {
        public Time()
        {
            Uberglue.DtValue_GetNumber();
        }
    }

    public class Curve : DtValue
    {
        public Curve()
        {
            Uberglue.Curve_GetValues();
            Uberglue.Curve
        }
    }

    public class TCurve : DtValue
    {
        public TCurve()
        {
            Uberglue.NewTCurve();
            Uberglue.TCurve_GetTimestamps();
            Uberglue.TCurve_GetValues();
        }
    }
}

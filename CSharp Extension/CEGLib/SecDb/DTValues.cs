/*
Converting and wrapping SecDb objects with .Net objects.
 
Implicit conversion to .Net objects:

  The DtValue method has method "value" that may provide a pure .Net
  representation of the data in a DtValue object when such a representation
  exists.  The default implementation simply returns self, and this
  corresponds to the general case where there is no natural pure .Net
  representation.

  A "value" method should only be provided if there exists a pure .Net
  object that is truly isomorphic to the SecDb value type and allows for
  lossless round trip conversions.


Implicit conversion from .Net objects:

  Unlike implicit conversion to .Net objects, there is not reason to be
  conservative about implicit conversion from .Net objects.  The function
  as_dtvalue is correct way to convert a .Net object to a DtValue object.
  This function is a noop when applying to DtValue object (like
  numpy.assarray), so it may be apply it liberally.  Conversion paths are
  registed by the type of the .Net object that they can convert using the
  decorator DtValue._register_conversion_function.
 
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CEGLib.SecDb
{
    public class DTValues
    {

    }
}

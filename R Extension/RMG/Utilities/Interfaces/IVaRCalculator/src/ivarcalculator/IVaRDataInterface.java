/*
 * IVaRDataInterface.java
 *
 * Created on May 17, 2007, 9:05 AM
 *
 */

package ivarcalculator;

import java.util.Vector;

public interface IVaRDataInterface 
{
 
    public String[] getCurveList();
    
    public String[] getContractDatesForCurve(String curveName);
    
}

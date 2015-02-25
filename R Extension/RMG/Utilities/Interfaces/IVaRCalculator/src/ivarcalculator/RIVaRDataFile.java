/*
 * RIVaRDataFile.java
 *
 * Created on May 17, 2007, 9:08 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package ivarcalculator;

import java.util.concurrent.ExecutionException;
import javax.swing.ImageIcon;
import javax.swing.SwingWorker;
import org.rosuda.JRI.REXP;
import org.rosuda.JRI.Rengine;


public class RIVaRDataFile implements IVaRDataInterface {
    
    static Rengine rEngine;
    
    static SwingWorker<String, Void> worker = null;
    
    public RIVaRDataFile(String fileName) {
        if (rEngine == null) {
            rEngine = createREngine();
        }
        
        try {
            
            loadFile(fileName);
            
            
        } catch (Exception e) {
            System.out.println("EX:"+e);
            e.printStackTrace();
        }
    }
    
    private void loadFile(final String fileName) 
    {
        worker = createSwingWorker(fileName);
        
        worker.execute();
        
    }
    
    private SwingWorker<String, Void> createSwingWorker(final String fileName) {
        SwingWorker<String, Void> worker = new SwingWorker<String, Void>() {
            @Override
            public String doInBackground() {
                REXP x;
                
                System.out.println("Removing all variables...");
                System.out.println(x=rEngine.eval("rm(list=ls())"));
                
                
                System.out.println("Sourcing interface file...");
                System.out.println(x=rEngine.eval("sys.source(\"H:/user/R/RMG/Energy/VaR/IVAR/ivarInterface.R\", .GlobalEnv)"));
                
                System.out.println("Loading Data File...");
                System.out.println(rEngine.eval("ivarInterface.loadFile(\""+fileName+"\")"));
                return "Load Complete.";
            }
            
            @Override
            public void done() {
                try {
                    System.out.println( get() );
                } catch (InterruptedException ignore) {} catch (java.util.concurrent.ExecutionException e) {
                    String why = null;
                    Throwable cause = e.getCause();
                    if (cause != null) {
                        why = cause.getMessage();
                    } else {
                        why = e.getMessage();
                    }
                    System.err.println("Error retrieving file: " + why);
                }
            }
        };
        
        return worker;
    }
    
    private Rengine createREngine() {
        if (!Rengine.versionCheck()) {
            System.err.println("** Version mismatch - Java files don't match library version.");
            System.exit(1);
        }
        
        System.out.println("Creating Rengine (with arguments)");
        Rengine rEngine=new Rengine(null, false, null);
        rEngine.DEBUG = 5;
        System.out.println("Rengine created, waiting for R");
        
        if (!rEngine.waitForR()) {
            System.out.println("Cannot load R");
            return null;
        }
        return rEngine;
    }
    
    public SwingWorker<String, Void> getSwingWorker()
    {
        return worker;
    }
    
    public String[] getCurveList() {
        
        String[] list = null;
        
        REXP exp = rEngine.eval("ivarInterface.getCurveList()");
        if (exp == null) {
            System.out.println("RIVaRDataFile: Got null curve list.");
        } else {
            System.out.println("Got:"+exp);
            list = exp.asStringArray();
            for (int i = 0; i < list.length; i++) {
                System.out.println("Item:"+list[i]);
            }
        }
        
        return list;
    }
    
    public String[] getContractDatesForCurve(String curveName) {
        if (curveName==null || curveName.equals("")) {
            return new String[]{""};
        }
        
        String[] list = rEngine.eval("ivarInterface.getContractDatesForCurve(\""+
                curveName+"\")").asStringArray();
        
        System.out.println("Printing Contract Dates:");
        for (int i = 0; i < list.length; i++) {
            System.out.println("Item:"+list[i]);
        }
        
        System.out.println("Done Printing Dates For Curve: "+curveName);
        
        return list;
    }
    
    public Long calculateIVaR() {
        REXP ivarObj = rEngine.eval("ivarInterface.calculateIVaR()");
        System.out.println("IVaR Type: "+ivarObj);
        if (ivarObj != null)
        {
            Long ivar = new Long(new Double(ivarObj.asDouble()).longValue());
            return ivar;
        }
        else
        {
            return new Long(-1);
        }
    }
    
    public void addPosition(String curveName, String contractDate, Integer position) {
        String expression = "ivarInterface.addPosition(\"";
        expression = expression + curveName +"\", \"";
        expression = expression + contractDate + "\",";
        expression = expression + position + ")";
        
        REXP ivarObj = rEngine.eval(expression);
        System.out.println("Add Position Result: "+ivarObj.asString());
    }
    
    private void rls() {
        String[] listToPrint = rEngine.eval("ivarInterface.testLS()").asStringArray();
        for (int i = 0; i < listToPrint.length; i++) {
            System.out.println("Item:"+listToPrint[i]);
        }
    }
    
}

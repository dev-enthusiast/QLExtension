package ivarcalculator;

import java.beans.PropertyChangeListener;
import java.io.File;
import java.util.Vector;
import javax.swing.SwingWorker;
import javax.swing.table.AbstractTableModel;

class TradeTableModel extends AbstractTableModel {
    
    RIVaRDataFile dataFile ;
    
    boolean DEBUG = true;
    String[] curveList = null;
    
    private Long ivar = new Long(0);
    
    private StatusBean statusBean = StatusBean.getInstance();
    
    public TradeTableModel() {
        data.add(new Object[]{ new Boolean(false), "", "", "",
        new Integer(0), new Long(0) });
    }
    
    public TradeTableModel(String fileName) {
        loadFile(fileName);
    }
    
    public void loadFile(final String fileName) {
        
        curveList = null;
        
        data = new Vector<Object[]>();
        data.add(new Object[]{ new Boolean(false), "", "", "",
        new Integer(0), new Long(0) });
        fireTableDataChanged();
        
        System.out.println(System.getProperty("user.dir"));
        System.out.println(System.getProperty("java.library.path"));
        
        System.load(System.getProperty("user.dir")+"/lib/jri/JRI.dll");

        System.out.println("\nLibrary Loaded.\n");
        
        dataFile = new RIVaRDataFile(fileName);
        if (!new File(fileName).exists()) {
            statusBean.setStatus("IVaR Data File Not Found.");
        }
    }
    
    public String[] getCurveList() 
    {
        if (curveList == null)
        {
            curveList = dataFile.getCurveList();
            if (curveList == null) {
                statusBean.setStatus("<html><font color=red>No Curves Found For Portfolio.</font></html>");
            } else {
                statusBean.setStatus("Curves Loaded.");
            }
        }
        return curveList;
    }
    
    public SwingWorker getSwingWorker()
    {
        return dataFile.getSwingWorker();
    }
    
    public String[] getContractDatesForCurve(String curveName) {
        return dataFile.getContractDatesForCurve(curveName);
    }
    
    public void calculateIVaR() {
        for( Object[] row : data ) {
            if ( ((Boolean)row[0]).booleanValue()==true &&
                    row[1] != null &&
                    row[2] != null &&
                    row[3] != null && !((String)row[3]).equals("") &&
                    ((Integer)row[4]).intValue() != 0 ) {
                String curve = (String)row[2];
                String date = (String)row[3];
                Integer position = (Integer)row[4];
                
                if ( ((String)row[1]).equals("SELL") ) {
                    position = position * -1;
                }
                
                dataFile.addPosition(curve, date, position);
            }
        }
        
        if (dataFile != null) {
            ivar = dataFile.calculateIVaR();
        } else {
            ivar = 0l;
        }
        if (getRowCount() > 1) {
            setValueAt(ivar, getRowCount()-2, 5);
        }
    }
    
    public Long getIVaR() {
        return ivar;
    }
    
    private String[] columnNames = {
        "Include",
        "Buy/Sell",
        "Curve",
        "Month",
        "Quantity",
        "VaR" };
    
    private Vector<Object[]> data = new Vector<Object[]>();
    
    
    public int getColumnCount() {
        return columnNames.length;
    }
    
    public int getRowCount() {
        return data.size();
    }
    
    public String getColumnName(int col) {
        return columnNames[col];
    }
    
    public Object getValueAt(int row, int col) {
        return data.elementAt(row)[col];
    }
    
    /*
     * JTable uses this method to determine the default renderer/
     * editor for each cell.  If we didn't implement this method,
     * then the first column would contain text ("true"/"false"),
     * rather than a check box.
     */
    public Class getColumnClass(int c) {
        return getValueAt(0, c).getClass();
    }
    
        /*
         * Don't need to implement this method unless your table's
         * editable.
         */
    public boolean isCellEditable(int row, int col) {
        //Note that the data/cell address is constant,
        //no matter where the cell appears onscreen.
        if (col == 0) {
            return true;
        } else if ( ((Boolean)(getValueAt(row, 0))).booleanValue() == true) {
            if (col > 4) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }
    
        /*
         * Don't need to implement this method unless your table's
         * data can change.
         */
    public void setValueAt(Object value, int row, int col) {
        if (value == null) {
            return;
        }
        
        if (DEBUG) {
            System.out.println("Setting value at " + row + "," + col
                    + " to " + value
                    + " (an instance of "
                    + value.getClass() + ")");
        }
        
        if ( col == 0 &&
                ((Boolean)(value)).booleanValue() &&
                row == getRowCount()-1 ) {
            appendRow(new Object[]
            { new Boolean(false),"", "",
              "", new Integer(0), new Long(0) });
            
            fireTableRowsInserted(getRowCount()-1, getRowCount());
        }
        
        Object[] rowValues = data.elementAt(row);
        rowValues[col] = value;
        data.setElementAt(rowValues, row);
        fireTableCellUpdated(row, col);
        
//        if (DEBUG) {
//            System.out.println("New value of data:");
//            printDebugData();
//        }
    }
    
    private void appendRow(Object[] rowValues) {
        data.add(rowValues);
    }
    
    private void printDebugData() {
        int numRows = getRowCount();
        int numCols = getColumnCount();
        
        for (int i=0; i < numRows; i++) {
            System.out.print("    row " + i + ":");
            for (int j=0; j < numCols; j++) {
                System.out.print("  " + data.elementAt(i)[j]);
            }
            System.out.println();
        }
        System.out.println("--------------------------");
    }
}
/*
 * DateCellEditor.java
 *
 * Created on May 18, 2007, 2:15 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package ivarcalculator;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;
import javax.swing.DefaultCellEditor;
import javax.swing.JComboBox;
import javax.swing.JTable;
import javax.swing.JTextField;

public class DateCellEditor extends DefaultCellEditor implements ActionListener {
    
    private Vector<JComboBox> combos = null;
    private JComboBox editor = null;
    
    public DateCellEditor() {
        super(new JComboBox());
        
        combos = new Vector<JComboBox>();
        
        // We need to register ourself so we know when the user decided they
        // had enough and we should stop the cell editing
        // Obviously this introduces some proplems with using arrow keys and
        // mouse clicks, but we decided that the users know how to make the 
        // action event appear.
        for(int i = 0; i < combos.size(); i++) {
            combos.elementAt(i).addActionListener( this );
        }
        
    }
    
    public void setBoxForRow(JComboBox box, int row) {
        while (row >= combos.size()) {
            combos.addElement(new JComboBox());
        }
        box.addActionListener(this);
        combos.setElementAt(box, row);
    }
    
    public void actionPerformed(ActionEvent ae) {
        // If any of the comboboxes are editing, we better stop it.
        stopCellEditing();
        // we could at this point make sure that the editor is null
        // both for editing stopped and editing cancelled
    }
    
    public Object getCellEditorValue() {
        if( editor == null ) {
            // Panick! We are in a state we should never end upp in.
            // Normally this should be an assert.
            System.out.println("WARNING! Got a null editor in " +
                    "DateCellEditor.getCellEditorValue()");
            return null;
        }
        return editor.getSelectedItem();
    }
    
    
    public Component getTableCellEditorComponent(JTable table,Object value, boolean isSelected, int row, int column) {
        if( row >= 0 && row < combos.size() ) {
            // We want the current value to be selected
            // Apropriate checks for null could be handled here as well.
            editor = combos.elementAt(row);
            editor.setSelectedItem(value);
            return editor;
        } 
        else {
            /**
             * if we're outside our bounds, it's because the stupid JComboBox
             * didn't register that we changed it's value. This safety check
             * will update the list with the proper dates.  I know it's crusty,
             * I'm all ears for another way.
             **/
            TradeTableModel model = (TradeTableModel) table.getModel();
            String curve = (String)model.getValueAt(row, column-1);
            System.out.println("Portfolio = "+curve);
            String[] curves = model.getContractDatesForCurve(curve);
            JComboBox newBox = new JComboBox(curves);
            
            setBoxForRow(newBox,row);

            return getTableCellEditorComponent(table, value, isSelected, row, column);
        }
    }
}

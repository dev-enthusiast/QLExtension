/*
 * StatusBean.java
 *
 * Created on May 22, 2007, 9:03 AM
 */

package ivarcalculator;

import java.beans.*;
import java.io.Serializable;

/**
 * @author e14600
 */
public class StatusBean extends Object implements Serializable {
    
    public static final String STATUS_PROPERTY = "statusProperty";
    
    private static String statusProperty;
    
    private PropertyChangeSupport propertySupport;
    
    private static StatusBean instance = null;
    
    public static StatusBean getInstance()
    {
        if (instance==null)
        {
            instance = new StatusBean();
        }
        
        return instance;
    }
    
    private StatusBean() {
        propertySupport = new PropertyChangeSupport(this);
    }
    
    public synchronized String getStatus() {
        return statusProperty;
    }
    
    public synchronized void setStatus(String value) {
        String oldValue = statusProperty;
        statusProperty = value;
        propertySupport.firePropertyChange(STATUS_PROPERTY, oldValue, statusProperty);
    }
    
    
    public void addPropertyChangeListener(PropertyChangeListener listener) {
        propertySupport.addPropertyChangeListener(listener);
    }
    
    public void removePropertyChangeListener(PropertyChangeListener listener) {
        propertySupport.removePropertyChangeListener(listener);
    }
    
}

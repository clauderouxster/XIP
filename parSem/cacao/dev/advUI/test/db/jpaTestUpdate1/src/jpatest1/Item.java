package jpatest1;

import java.io.Serializable;
import javax.persistence.*;

@Entity
public class Item implements Serializable {

    @Transient
    private static final long serialVersionUID = 1L;
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Long id;
    private String itemId;
    private String label;

    public String getItemId() {
        return itemId;
    }

    public String getLabel() {
        return label;
    }
   
    protected void setItemId(String itemId) {
        this.itemId = itemId;
    }

    protected void setLabel(String label) {
        this.label = label;
    }   
}

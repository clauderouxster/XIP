package classificationExplorer;

import java.io.Serializable;
import javax.persistence.*;

/**
 *
 * @author Gilbert
 */
@Entity
@Table(name = Const.itemTableName)
public class Item implements Serializable {

    @Transient
    private static final long serialVersionUID = 1L;
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Long id;
    @Column(nullable = false)
    private String itemId;
    @Column(nullable = false)
    private String label;
    @Column(name = "ddepth")
    private int depth;
    private long parent;
    @Column(nullable = false)
    private String parentId;
    // --------- Relationship Definitions -----------
    //fetch=FetchType.LAZY =>  Lorsqu'un Item sera mis dans le contexte de persistance, on
    // demande à ce que sa Classification n'y soit pas mise immédiatement
    @ManyToOne(fetch = FetchType.LAZY)
    //La relation Many(Item) to One (Classification) est implémentée
    // par une clé étrangère (classification_id) dans Item
    //Elle matérialise la relation de clé étrangère qui lie
    //l'Entity Item à l'Entity Classification.
    @JoinColumn(name = "classification_id", nullable = false)
    private Classification classification;

    /**
     *
     * @return
     */
    public int getDepth() {
        return depth;
    }

    /**
     *
     * @param depth
     */
    protected void setDepth(int depth) {
        this.depth = depth;
    }

    /**
     *
     * @return
     */
    protected Long getId() {
        return id;
    }

    /**
     *
     * @return
     */
    public String getItemId() {
        return itemId;
    }

    /**
     *
     * @param itemId
     */
    protected void setItemId(String itemId) {
        this.itemId = itemId;
    }

    /**
     *
     * @return 
     */
    public String getLabel() {
        return label;
    }

    /**
     *
     * @param label
     */
    protected void setLabel(String label) {
        this.label = label;
    }

    /**
     *
     * @return
     */
    public String getParentId() {
        return parentId;
    }

    /**
     *
     * @param parentId
     */
    protected void setParentId(String parentId) {
        this.parentId = parentId;
    }

    /**
     * 
     * @param parent
     */
    protected void setParent(long parent) {
        this.parent = parent;
    }

    /**
     *
     * @return
     */
    protected long getParent() {
        return parent;
    }

    /**
     *
     * @return the classification of the item
     */
    public Classification getClassification() {
        return classification;
    }

    /**
     *
     * @param classification
     */
    protected void setClassification(Classification classification) {
        this.classification = classification;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("itemId=");
        sb.append(getItemId());
        sb.append(" parentId=");
        sb.append(getParentId());
        sb.append(" parent=");
        sb.append(getParent());
        sb.append(" depth=");
        sb.append(getDepth());
        sb.append(" classification=");
        sb.append(getClassification().getName());
        sb.append(" label=");
        sb.append(getLabel());
        return sb.toString();
    }
}

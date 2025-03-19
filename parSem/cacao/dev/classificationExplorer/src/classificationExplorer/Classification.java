package classificationExplorer;

import java.io.Serializable;
import javax.persistence.*;

/**
 *
 * @author grondeau
 */
@Entity
@Table(name = Const.classificationTableName)
public class Classification implements Serializable {

    @Transient
    private static final long serialVersionUID = 1L;
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Long id;
    // @Column(nullable = false, unique = true)  //"unique= true" not accepted by HSQLDB
    @Column(nullable = false)
    private String name;

    /**
     *
     */
    public Classification() {
    }

    /**
     *
     * @return classification Id
     */
    public Long getId() {
        return id;
    }

    /**
     *
     * @return classification name
     */
    public String getName() {
        return name;
    }

    /**
     *
     * @param name
     */
    protected void setName(String name) {
        this.name = name;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("Classification name = ");
        sb.append(getName());
        sb.append(" id = ");
        sb.append(getId());
        return sb.toString();
    }
}


import java.util.Enumeration;
import java.util.Properties;
import java.util.Set;

class Test {

    public static void main(String args[]) {
        try {
            Properties properties = System.getProperties();
            Set<Object> keys = properties.keySet();
            for (Object key : keys) {
                System.out.println(key + " = " + properties.get(key));
            }
            System.out.println("-------------------");
            System.out.println("sun.arch.data.model = " + System.getProperty("sun.arch.data.model"));
            System.out.println("os.arch = " + System.getProperty("os.arch"));
        } catch (Exception ex) {
            System.out.println(ex);
            System.exit(-1);
        }
    }
}

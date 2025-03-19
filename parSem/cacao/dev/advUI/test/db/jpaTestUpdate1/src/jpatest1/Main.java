package jpatest1;

import java.util.List;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import javax.persistence.Query;

public class Main {

    EntityManagerFactory emf;
    EntityManager em;

    public static void main(String[] args) {
        Main main = new Main();
        main.initDB();
        // main.test0();
        main.test1();
    }

    protected void initDB() {
        //emf = Persistence.createEntityManagerFactory("hsqldb-emb");
        emf = Persistence.createEntityManagerFactory("h2-emb");
        // emf = Persistence.createEntityManagerFactory("mysql");
        em = emf.createEntityManager();
        em.close();
        //clean DB
        createTransactionalEntityManager();
        try {
            em.createNativeQuery("delete from Item").executeUpdate();
        } catch (Exception ex) {
            System.out.println(ex.getMessage());
        }
        closeTransactionalEntityManager();
    }

    private void dump() {
        em = emf.createEntityManager();
        Query query = em.createQuery("SELECT item FROM Item item");
        List<Item> list = query.getResultList();
        for (Item item : list) {
            System.out.println(item.getItemId() + " " + item.getLabel());
        }
        em.close();
    }

    private void test1() {
        createTransactionalEntityManager();
        Item item = new Item();
        item.setItemId("itemId1");
        item.setLabel("aaa");
        em.persist(item);
        item = new Item();
        item.setItemId("itemId2");
        item.setLabel("bbb");
        em.persist(item);
        closeTransactionalEntityManager();
        dump();

        createTransactionalEntityManager();
        //UPDATE [<candidate-class>] SET item1=value1, item2=value2 [WHERE <filter>]
       // Query query = em.createQuery("UPDATE Item item SET itemId = :p1 WHERE item.itemId = :p2");
        Query query = em.createQuery("UPDATE Item item SET itemId = :p1");
        query.setParameter("p1", "ccc");
       // query.setParameter("p2", "itemId1");
        int cnt = query.executeUpdate();
        System.out.println(cnt + " item updated");
        closeTransactionalEntityManager();
        dump();
    }

    private void test0() {
        int cnt = 0;
        long mem1 = memUsed();
        long startTime = System.currentTimeMillis();
        createTransactionalEntityManager();
        Item item = null;
        for (int i = 0; i < 100000; i++) {
            item = new Item();
            item.setItemId("itemId" + i);
            item.setLabel("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" + i);
            em.persist(item);
            if (cnt++ > 10000) {
                cnt = 0;
                closeTransactionalEntityManager();
                createTransactionalEntityManager();
            }
        }
        closeTransactionalEntityManager();
        System.out.println("duration: " + (System.currentTimeMillis() - startTime));
        long memUsed = memUsed() - mem1;
        System.out.println("mem used: " + memUsed);
    }

    protected void createTransactionalEntityManager() {
        em = emf.createEntityManager();
        em.getTransaction().begin();
    }

    /**
     *
     */
    protected void closeTransactionalEntityManager() {
        em.getTransaction().commit();
        em.close();
    }

    static long memUsed() {
        for (int i = 0; i < 20; i++) {
            System.gc();
        }
        return Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();
    }
}

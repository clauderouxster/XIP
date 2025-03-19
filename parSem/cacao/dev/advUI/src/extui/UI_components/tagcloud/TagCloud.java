/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package extui.UI_components.tagcloud;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class TagCloud {

    private static final Logger logger = Logger.getLogger(TagCloud.class.getName());
    private List<String> sources;
    private ArrayList<Tag> tags;
    /**
     *
     */
    private long totTags;

    /**
     *
     * @param s
     */
    @SuppressWarnings("unchecked")
    public TagCloud(List s) {
        this.sources = new ArrayList();
        //System.out.println("SIZE LIST TAG CLOUD: " + s.size());
        if (s!=null) {
        for (int x = 0; x < s.size(); x++) {
            //System.out.println(s.get(x));
            //this.sources.add(s.get(x).toString().replace('[', ' ').replace(']', ' '));
            this.sources.add((String)s.get(x));
        }
        //System.out.println("SIZE SOURCE LIST TAG CLOUD: " + this.sources.size());
        //for(int x=0;x<this.sources.size();x++) {
        //     System.out.println(this.sources.get(x));
        // }
        }
        tags = new ArrayList();
    }

    /**
     *
     * @return
     */
    public List<String> getSources() {
        return sources;
    }

    /**
     *
     * @param source
     */
    public void addSource(String source) {
        this.sources.add(source);
    }

    /**
     *
     * @param sources
     */
    public void setSource(List<String> sources) {
        this.sources = sources;
    }

    /**
     *
     * @param n
     * @return
     */
    public ArrayList<Tag> getTags(int n) {

        if (tags.size() < n) {
            n = tags.size();
        }
        // Compute n tags more frequent
        // 1) Put in order
        for (int x = 0; x < tags.size(); x++) {
            for (int y = 0; y < tags.size(); y++) {
                if (tags.get(x).getFreq() > tags.get(y).getFreq()) {
                    Tag t = tags.get(x);
                    tags.set(x, tags.get(y));
                    tags.set(y, t);
                }
            }
        }
        // 2) Put List n more frequent
        ArrayList<Tag> newList = new ArrayList<Tag>();
        for (int x = 0; x < n; x++) {
            newList.add(tags.get(x));
        }
        // 3) Order according to string
        for (int x = 0; x < newList.size(); x++) {
            for (int y = 0; y < newList.size(); y++) {
                if (newList.get(x).getName().compareToIgnoreCase(newList.get(y).getName()) < 0) {
                    Tag t = newList.get(x);
                    newList.set(x, newList.get(y));
                    newList.set(y, t);
                }
            }
        }
        return newList;
    // return (ArrayList<Tag>) tags.subList(0, n); ? casting problem (runtime)
    }

    /**
     *
     * @param tag
     */
    public void addTag(Tag tag) {
        this.tags.add(tag);
    }

    /**
     *
     * @param tags
     */
    public void setTags(ArrayList<Tag> tags) {
        this.tags = tags;
    }

    /**
     * @throws java.io.FileNotFoundException
     * @throws java.io.IOException
     */
    public void processTags() throws FileNotFoundException, IOException {
        // 1. For each source:
        // a) remove special characteres and normalize (maiscule and minuscule)
        // b) compute frequence
        removeStopWords();
        computeFrequence();
    }

    private void removeStopWords() throws FileNotFoundException, IOException {
        for (int x = 0; x < sources.size(); x++) {
            logger.debug("SOURCE: " + sources.get(x));
            String[] source = sources.get(x).toString().split(" ");
            String newSource = remove(source, readFile(new File("englishST.txt")));
            newSource = remove(source, readFile(new File("frenchST.txt")));
            newSource = remove(source, readFile(new File("germanST.txt")));
            newSource = remove(source, readFile(new File("italianST.txt")));
            sources.set(x, newSource);
        }
    }

    /**
     * @param source
     * @param stopWords
     * @return
     */
    public String remove(String[] source, ArrayList<String> stopWords) {
        String newSource = "";
        for (int x = 0; x < source.length; x++) {
            boolean isStop = false;
            for (int y = 0; y < stopWords.size(); y++) {
                if (source[x].equalsIgnoreCase(stopWords.get(y))) {
                    isStop = true;
                }
            }
            if (!isStop) {
                newSource = source[x] + " ";
            }
        }
        return newSource;
    }

    /**
     *
     * @param aFile
     * @return
     * @throws java.io.FileNotFoundException
     * @throws java.io.IOException
     */
    public ArrayList<String> readFile(File aFile) throws FileNotFoundException, IOException {
        ArrayList<String> stopWords = new ArrayList<String>();
        try {
            BufferedReader input = new BufferedReader(new FileReader(aFile));
            try {
                String line = null;
                while ((line = input.readLine()) != null) {
                    stopWords.add(line);
                }
            } finally {
                input.close();
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        return stopWords;
    }

    private void computeFrequence() {
        // Tags for each source
        ArrayList<ArrayList<Tag>> tagsSources = new ArrayList<ArrayList<Tag>>();
        for (int x = 0; x < sources.size(); x++) {
            ArrayList<Tag> tagsSource = new ArrayList<Tag>();
            String[] source = sources.get(x).toString().split(" ");
            for (int a = 0; a < source.length; a++) {
                long freq = 1;
                String word = source[a].toLowerCase();
                for (int b = 0; b < source.length; b++) {
                    if (a != b) {
                        if (word.equals(source[b].toLowerCase())) {
                            freq++;
                        }
                    }
                }
                // criar cada vector
                if (!addTagSource(tagsSource, word)) {
                    tagsSource.add(new Tag(word, freq,this));
                }
            }
            tagsSources.add(x, tagsSource);
        }
        for (int x = 0; x < tagsSources.size(); x++) {
            ArrayList<Tag> tagsSource = tagsSources.get(x);
            for (int y = 0; y < tagsSource.size(); y++) {
                addTagTags(tagsSource.get(y));
            }
        }
    }

    private boolean addTagSource(ArrayList<Tag> source, String tag) {
        boolean already = false;
        for (int x = 0; x < source.size(); x++) {
            if (tag.equals(source.get(x).getName())) {
                already = true;
            }
        }
        return already;
    }

    private void addTagTags(Tag tag) {
        boolean already = false;
        for (int x = 0; x < tags.size(); x++) {
            if (tag.getName().equals(tags.get(x).getName())) {
                tags.get(x).setFreq(tags.get(x).getFreq() + tag.getFreq());
                already = true;
            }
        }
        if (!already) {
            addTag(tag);
            totTags++;
        }
    }

    public long getTotTags() {
           return totTags;
    }


    public static void main(String arg[]) throws FileNotFoundException, IOException {
           List<String> listTitles = new ArrayList<String>();
           listTitles.add("The bank management");
           listTitles.add("finantial bank");

           // Get as input the list of strings
           TagCloud tagCloud = new TagCloud(listTitles);
           tagCloud.processTags();
           // Result: list of Tag
           ArrayList<Tag> tags = tagCloud.getTags(100); // Size of the tag cloud (example)
           
           for(int x=0;x<tags.size();x++) {
               System.out.println("Tag: " + tags.get(x).getName() + " Freq: " + tags.get(x).getFreq());
           }
    }

}

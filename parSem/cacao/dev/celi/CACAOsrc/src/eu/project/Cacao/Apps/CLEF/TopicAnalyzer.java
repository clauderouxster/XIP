package eu.project.Cacao.Apps.CLEF;

import java.io.File;
import java.io.IOException;
import java.util.HashSet;

import org.apache.log4j.Logger;
import org.apache.lucene.analysis.WordlistLoader;


public class TopicAnalyzer {
    HashSet<String> contents;
    HashSet<String> junk;
    protected Logger logger = Logger.getLogger(TopicAnalyzer.class.getName());

    public TopicAnalyzer(String contentsFileName, String junkFileName) {
        try {
            File contentsFile = new File(contentsFileName);
            File junkFile = new File(junkFileName);
            this.contents = (HashSet<String>) WordlistLoader.getWordSet(contentsFile);
            this.junk = (HashSet<String>) WordlistLoader.getWordSet(junkFile);
        } catch (IOException e) {
            e.printStackTrace();
            this.contents = new HashSet<String>();
            this.junk = new HashSet<String>();
        }
    }

    public TopicDescription analyzeString(String txt) {
        String newTxt = new String(" " + txt + " ");
        newTxt = newTxt.replaceAll(",|;|\\.|!|:", "");

        HashSet<String> buffSet = new HashSet<String>();
        String buff = "";

        for (String type : contents) {
            if (newTxt.contains(" " + type + " ")) {
                buffSet.add(type);
            }
        }

        for (String s : buffSet) {
            buff += (s + " ");
        }

        for (String s : junk) {
            if (newTxt.contains(" " + s + " ")) {
                newTxt = newTxt.replaceAll(" " + s + " ", " ");
            }
        }

        TopicDescription d = new TopicDescription();
        d.what = buff.trim();
        d.about = newTxt.trim();

        return d;
    }
}

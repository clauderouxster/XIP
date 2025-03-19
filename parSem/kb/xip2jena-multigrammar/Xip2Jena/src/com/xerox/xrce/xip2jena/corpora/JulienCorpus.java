/*
 * Copyright Xerox 2009
 */
package com.xerox.xrce.xip2jena.corpora;

import com.xerox.xrce.xip2jena.corpus.FilesCorpus;
import com.xerox.xrce.xip2jena.Util;
import common.Options;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;

public class JulienCorpus extends FilesCorpus {
    public JulienCorpus() {
        List<File> infiles = new ArrayList<File>();
        infiles.addAll(Util.listFiles(new File("/opt/ParSem/Corpora/ENGLISH/corpus_news/ap/filtr_text")));
        infiles.addAll(Util.listFiles(new File("/opt/ParSem/Corpora/ENGLISH/corpus_news/bbc/filtr_text")));
        infiles.addAll(Util.listFiles(new File("/opt/ParSem/Corpora/ENGLISH/corpus_news/usatoday/filtr_text")));
        init(infiles);
    }
    @Override
    public common.corpora.Corpus getTemporalCorpus(Options o) {
        return new JulienTemporalCorpus(o);
    }
}

class JulienTemporalCorpus extends common.corpora.Corpus {
    public JulienTemporalCorpus(Options o){}

    Logger log = Logger.getLogger(JulienTemporalCorpus.class);

    /**
     *  get the Document Creation Time (DCT)
     *   from the directory name
     *  for Julien's corpus-RSS files
     *  first 9 characters of the directory base name
     *
     * @param options the parser options.
     */
    @Override
    public void getInputInfo(Options options) {
        File inputDirName = new File(options.getInputFileDir());

        if (!options.getInputFileDir().equals("/home/nlagos/test_data/rss_files")) {
            VagueDate dct = null;
            String format = null;
            log.debug("Filename DIR : " + inputDirName);
            log.debug("Filename DIR : " + inputDirName.length());
            int i = 33;
            String year = options.getInputFileDir().substring(i, i + 4);
            String month = options.getInputFileDir().substring(i + 5, i + 6);
            String month2 = options.getInputFileDir().substring(i + 6, i + 7);
            String day = options.getInputFileDir().substring(i + 7, i + 9);

            String day2 = null;
            if (month2.equals("-")) {
                day = options.getInputFileDir().substring(i + 7, i + 8);
                day2 = options.getInputFileDir().substring(i + 8, i + 9);
                if (day2.equals("-")) {
                    format = "+" + year + "0" + month + "0" + day + "T-------O---W---";
                } else {
                    format = "+" + year + "0" + month + day + day2 + "T-------O---W---";
                }
            } else {
                day = options.getInputFileDir().substring(i + 7, i + 8);
                day2 = options.getInputFileDir().substring(i + 8, i + 9);
                if (day2.equals("-")) {
                    format = "+" + year + month + "0" + day + "T-------O---W---";
                } else {
                    format = "+" + year + month + day + day2 + "T-------O---W---";
                }
            }

            log.debug("Filename DIR : " + inputDirName);
            log.debug("YEAR DCT : " + year);
            log.debug("MONTH DCT : " + month);
            log.debug("DAY DCT : " + day);
            format = "+" + year + "0" + month + day + "T-------O---W---";
            try {
                dct = VagueDate.getNewDate(format);
                options.setDct(dct);
            } catch (TemporalConsistencyException ex) {
                log.warn("Failed to get DCT for " + inputDirName, ex);
            } catch (BadFormatException e) {
                log.warn("Failed to get DCT for " + inputDirName, e);
            }
        }
    }
}

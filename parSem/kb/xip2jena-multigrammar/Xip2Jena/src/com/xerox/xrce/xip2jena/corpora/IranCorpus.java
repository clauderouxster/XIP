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
import java.util.Collections;
import java.util.List;
import org.apache.log4j.Logger;


public class IranCorpus extends FilesCorpus {

    public IranCorpus() {
        List<File> infiles = new ArrayList<File>();
        infiles.addAll(Util.listFiles(new File("/home/akaplan/kr/iran-corpus")));
        Collections.sort(infiles);
        init(infiles);
    }

    @Override
    public common.corpora.Corpus getTemporalCorpus(Options o) {
        return new IranTemporalCorpus(o);
    }

}

class IranTemporalCorpus extends common.corpora.Corpus {
    static Logger log = Logger.getLogger(IranTemporalCorpus.class);

    public IranTemporalCorpus(Options o){}

    /**
     * DCT for multi-file Iran corpus
     */
    @Override
    public void getInputInfo(Options options) {
        File file = new File(options.getInputFileName());
        String fname = file.getName();
        String datestring = fname.substring(fname.length()-8);
        String format = "+" + datestring + "T-------O---W---";
        //String format = "+20090410T-------O---W---";
        try {
            VagueDate dct;
            dct = VagueDate.getNewDate(format);
            options.setDct(dct);
        } catch (BadFormatException ex) {
            log.error("Failed to get DCT for " + file, ex);
        } catch (TemporalConsistencyException ex) {
            log.error("Failed to get DCT for " + file, ex);
        }
    }
}

/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_components;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import java.io.Serializable;
import java.util.HashSet;

public class Document implements Serializable {

    private static final long serialVersionUID = 1L;
    public static final String recordElementTag = "Record",  libraryID_Elm = "libraryID",  recordID_Elm = "recordID",  title_Elm = "title",  subj_Elm = "subject",  lang_Elm = "language",  publisher_Elm = "publisher",  OAIset_Elm = "OAI_set",  type_Elm = "type",  scoreAttr = "recordRelevance",  telRecordID_URL_Elm = "telRecordID_URL",  ISSN_Elm = "ISSN",  ISBN_Elm = "ISBN",  url_Elm = "URL",  contributor_Elm = "contributor",  description_Elm = "description",  tableOfContents_Elm = "tableOfContents",  identifier_Elm = "identifier",  creator_Elm = "creator";
    private String score,  title,  publisher,  URL,  ISSN,  ISBN,  telRecordID_URL,  recordID,  type,  identifier,  libraryID;
    private HashSet<String> subjects,  oaiSets,  descriptions,  tableOfContents,  languages,  contributors,  creators;
    static long id  = 0;
    long docID;

    public Document(DocsSearchResult doc) {
        id++;
        docID = id;
        score = doc.getScore();
        title = doc.getTitle();
        publisher = doc.getPublisher();
        URL = doc.getURL();
        ISSN = doc.getISSN();
        ISBN = doc.getISBN();
        telRecordID_URL = doc.getTelRecordID_URL();
        recordID = doc.getRecordID();
        type = doc.getType();
        identifier = doc.getIdentifier();
        libraryID = doc.getLibraryID();
        subjects = doc.getSubjects();
        oaiSets = doc.getOaiSets();
        descriptions = doc.getDescriptions();
        tableOfContents = doc.getTableOfContents();
        languages = doc.getLanguages();
        contributors = doc.getContributors();
        creators = doc.getCreators();
    }

    public String getISBN() {
        return ISBN;
    }

    public String getISSN() {
        return ISSN;
    }

    public String getURL() {
        return URL;
    }

    public HashSet<String> getContributors() {
        return contributors;
    }

    public HashSet<String> getCreators() {
        return creators;
    }

    public HashSet<String> getDescriptions() {
        return descriptions;
    }

    public String getIdentifier() {
        return identifier;
    }

    public HashSet<String> getLanguages() {
        return languages;
    }

    public String getLibraryID() {
        return libraryID;
    }

    public HashSet<String> getOaiSets() {
        return oaiSets;
    }

    public String getPublisher() {
        return publisher;
    }

    public String getRecordID() {
        return recordID;
    }

    public String getScore() {
        return score;
    }

    public HashSet<String> getSubjects() {
        return subjects;
    }

    public HashSet<String> getTableOfContents() {
        return tableOfContents;
    }

    public String getTelRecordID_URL() {
        return telRecordID_URL;
    }

    public String getTitle() {
        return title;
    }

    public String getType() {
        return type;
    }

    public long getDocID() {
           return this.docID;
    }
}

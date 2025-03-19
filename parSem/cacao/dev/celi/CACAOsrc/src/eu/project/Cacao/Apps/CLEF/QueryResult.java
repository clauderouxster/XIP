package eu.project.Cacao.Apps.CLEF;

import java.util.Comparator;


public class QueryResult {
    private static final int HASH_PRIME = 100000;
    private float luceneScore;
    private String documentID = null;

    public QueryResult(float luceneScore, String documentID) {
        this.luceneScore = luceneScore;
        this.documentID = documentID;
    }

    //In order to enable a class 4 Hash based containers it must ovverride equals(Object) and not overload with another Class
    //It must also implement the hashCode method
    public boolean equals(Object obj) {
        if ((obj == null) || !this.getClass().equals(obj.getClass())) {
            return false;
        }

        QueryResult x = (QueryResult) obj;

        return (documentID.equals(x.documentID) &&
        (luceneScore == x.luceneScore));
    }

    public int hashCode() {
        int hash = (int) (HASH_PRIME * luceneScore);
        hash += documentID.toUpperCase().hashCode();

        return hash;
    }

    public float getLuceneScore() {
        return luceneScore;
    }

    public String getDocumentID() {
        return documentID;
    }

    public void setLuceneScore(float luceneScore) {
        this.luceneScore = luceneScore;
    }

    public static class LuceneScoreComparator implements Comparator<QueryResult> {
        public int compare(QueryResult a, QueryResult b) {
            return ((b.luceneScore != a.luceneScore)
            ? ((int) ((b.luceneScore - a.luceneScore) * HASH_PRIME))
            : a.documentID.compareTo(b.documentID));
        }
    }
}

/* 
 * OCKHAM P2PREGISTRY Copyright 2006 Oregon State University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package eu.project.Cacao.Components.Utils;

import java.io.IOException;
import java.util.HashSet;
import java.util.Properties;
import java.util.StringTokenizer;

import org.apache.log4j.Logger;
import org.apache.lucene.index.Term;
import org.apache.lucene.search.BooleanClause;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.PhraseQuery;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.RangeQuery;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.search.WildcardQuery;
import org.z3950.zing.cql.CQLAndNode;
import org.z3950.zing.cql.CQLBooleanNode;
import org.z3950.zing.cql.CQLNode;
import org.z3950.zing.cql.CQLNotNode;
import org.z3950.zing.cql.CQLOrNode;
import org.z3950.zing.cql.CQLParser;
import org.z3950.zing.cql.CQLTermNode;


/**
 * @author peter
 *         Date: Oct 25, 2005
 *         Time: 10:38:43 AM
 */
public class LuceneTranslator  {
	
    private static Logger log = Logger.getLogger(LuceneTranslator.class);
    public static final String DEFAULT_ID_TERM = "uniqueidentifier";
    public static final String PROPERTY_INDEXPATH = "cqlTranslator.indexPath";
    public static final String PROPERTY_IDENTIFIER_TERM = "cqlTranslator.identifierTerm";
    
    public static HashSet<String> terms;
    
    /**
     *  IndexPath
     */
    private String indexPath;
    public String getIndexPath(){return indexPath;}
    public void setIndexPath(String inp){
        indexPath = inp;
    }

    /**
     *  IndexSearcher
     */
    private IndexSearcher searcher;
    public IndexSearcher getSearcher(){return searcher;}
    public void setSearcher(IndexSearcher inp){
        searcher = inp;
    }

    /**
     *  IdentifierTerm
     */
    private String identifierTerm;
    public String getIdentifierTerm(){return identifierTerm;}
    public void setIdentifierTerm(String inp){
        identifierTerm = inp;
    }

    public LuceneTranslator() {
        identifierTerm = DEFAULT_ID_TERM;
    }

    public LuceneTranslator(String indexPath) throws IOException {
        this.indexPath = indexPath;
        searcher = new IndexSearcher(indexPath);
        identifierTerm = DEFAULT_ID_TERM;
    }

    public LuceneTranslator(String indexPath, String identifierTerm) throws IOException {
        this.indexPath = indexPath;
        this.identifierTerm = identifierTerm;
        searcher = new IndexSearcher(indexPath);
    }

    public void init(Properties properties) {
        String temp;

        temp = (String) properties.get(PROPERTY_INDEXPATH);
        if (temp != null && temp.trim().length() != 0) {
           indexPath = temp;
            try {
                searcher = new IndexSearcher(indexPath);
            } catch (IOException e) {
                //rethrow as runtimeException so super doesnt have to be modified
                throw new RuntimeException(e);
            }
        }

        temp = (String) properties.get(PROPERTY_IDENTIFIER_TERM);
        if (temp != null && temp.trim().length() != 0) {
           identifierTerm = temp;
        }

    }


    public static Query makeQuery(CQLNode node) throws SRWDiagnostic{
        return makeQuery(node, null);
    }

    public static Query makeQuery(CQLNode node, Query leftQuery) throws SRWDiagnostic{
        Query query = null;
        
        /*if(node instanceof CQLProxNode) {
        	CQLProxNode cpn=(CQLProxNode)node;
        	Query left = makeQuery(cpn.left);
        	Query right = makeQuery(cpn.right);
        	left.equals(right);
        	for(Modifier md: cpn.ms.getModifiers()){
        		System.out.println(md.getType());
        	}
        }*/
        
        if(node instanceof CQLBooleanNode) {
            CQLBooleanNode cbn=(CQLBooleanNode)node;

            Query left = makeQuery(cbn.left);
            Query right = makeQuery(cbn.right, left);

            if(node instanceof CQLAndNode) {
                if (left instanceof BooleanQuery) {
                    query = left;
                    log.info("  Anding left and right");
                    AndQuery((BooleanQuery) left, right);
                } else {
                    query = new BooleanQuery();
                    log.info("  Anding left and right in new query");
                    AndQuery((BooleanQuery) query, left);
                    AndQuery((BooleanQuery) query, right);
                }

            } else if(node instanceof CQLNotNode) {

                if (left instanceof BooleanQuery) {
                    log.debug("  Negating left and right");
                    query = left;
                    NotQuery((BooleanQuery) left, right);
                } else {
                    query = new BooleanQuery();
                    log.debug("  Negating left and right in new query");
                    AndQuery((BooleanQuery) query, left);
                    NotQuery((BooleanQuery) query, right);
                }

            } else if(node instanceof CQLOrNode) {
                if (left instanceof BooleanQuery) {
                    log.debug("  Or'ing left and right");
                    query = left;
                    OrQuery((BooleanQuery) left, right);
                } else {
                    log.debug("  Or'ing left and right in new query");
                    query = new BooleanQuery();
                    OrQuery((BooleanQuery) query, left);
                    OrQuery((BooleanQuery) query, right);
                }
            } else {
                throw new RuntimeException("Unknown boolean");
            }

        } else if(node instanceof CQLTermNode) {
            CQLTermNode ctn=(CQLTermNode)node;

            String relation = ctn.getRelation().getBase();
            String index=ctn.getIndex();

            if (!index.equals("")) {
                if(relation.equals("=") || relation.equals("scr")) {
                    query = createTermQuery(index,ctn.getTerm(), relation);
                } else if (relation.equals("<")) {
                    Term term = new Term(index, ctn.getTerm());
                    //term is upperbound, exclusive
                    query = new RangeQuery(null,term,false);
                } else if (relation.equals(">")) {
                    Term term = new Term(index, ctn.getTerm());
                    //term is lowerbound, exclusive
                    query = new RangeQuery(term,null,false);
                } else if (relation.equals("<=")) {
                    Term term = new Term(index, ctn.getTerm());
                    //term is upperbound, inclusive
                    query = new RangeQuery(null,term,true);
                } else if (relation.equals(">=")) {
                    Term term = new Term(index, ctn.getTerm());
                    //term is lowebound, inclusive
                    query = new RangeQuery(term,null,true);

                } else if (relation.equals("<>")) {
                    /**
                     * <> is an implicit NOT.
                     *
                     * For example the following statements are identical results:
                     *   foo=bar and zoo<>xar
                     *   foo=bar not zoo=xar
                     */

                    if (leftQuery == null) {
                        // first term in query create an empty Boolean query to NOT
                        query = new BooleanQuery();
                    } else {
                        if (leftQuery instanceof BooleanQuery) {
                            // left query is already a BooleanQuery use it
                            query = leftQuery;
                        } else {
                            // left query was not a boolean, create a boolean query
                            // and AND the left query to it
                            query = new BooleanQuery();
                            AndQuery((BooleanQuery)query, leftQuery);
                        }
                    }
                    //create a term query for the term then NOT it to the boolean query
                    Query termQuery = createTermQuery(index,ctn.getTerm(), relation);
                    NotQuery((BooleanQuery) query, termQuery);

                } else if (relation.equals("any")) {
                    //implicit or
                    query = createTermQuery(index,ctn.getTerm(), relation);

                } else if (relation.equals("all")) {
                    //implicit and
                    query = createTermQuery(index,ctn.getTerm(), relation);
                } else if (relation.equals("exact")) {
                    /**
                     * implicit and.  this query will only return accurate
                     * results for indexes that have been indexed using
                     * a non-tokenizing analyzer
                     */
                    query = createTermQuery(index,ctn.getTerm(), relation);
                } else {
                    //anything else is unsupported
                    throw new SRWDiagnostic(19, ctn.getRelation().getBase());
                }

            }
        } else {
            throw new SRWDiagnostic(47, "UnknownCQLNode: "+node+")");
        }
        if (query != null) {
            log.info("Query : " + query.toString());
        }
        return query;
    }

    public static Query createTermQuery(String field, String value, String relation) {

        Query termQuery = null;
        LuceneTranslator.terms.add(value);
        
        /**
         * check to see if there are any spaces.  If there are spaces each
         * word must be broken into a single term search and then all queries
         * must be combined using an and.
         */
        if (value.indexOf(" ") == -1) {
            // no space found, just create a single term search
            //todo case insensitivity?
            Term term = new Term(field, value);
            if (value.indexOf("?") != -1 || value.indexOf("*")!=-1 ){
                termQuery = new WildcardQuery(term);
            } else {
                termQuery = new TermQuery(term);
            }

        } else {
            // space found, iterate through the terms to create a multiterm search

            if (relation == null || relation.equals("=") || relation.equals("<>") || relation.equals("exact")) {
                /**
                 * default is =, all terms must be next to eachother.
                 * <> uses = as its term query.
                 * exact is a phrase query
                 */
                PhraseQuery phraseQuery = new PhraseQuery();
                StringTokenizer tokenizer = new StringTokenizer(value, " ");
                while (tokenizer.hasMoreTokens()) {
                    String curValue = tokenizer.nextToken();
                    phraseQuery.add(new Term(field, curValue));
                }
                termQuery = phraseQuery;

            } else if(relation.equals("any")) {
                /**
                 * any is an implicit OR
                 */
                termQuery = new BooleanQuery();
                StringTokenizer tokenizer = new StringTokenizer(value, " ");
                while (tokenizer.hasMoreTokens()) {
                    String curValue = tokenizer.nextToken();
                    Query subSubQuery = createTermQuery(field, curValue, relation);
                    OrQuery((BooleanQuery) termQuery, subSubQuery);
                }

            } else if (relation.equals("all")) {
                /**
                 * any is an implicit AND
                 */
                termQuery = new BooleanQuery();
                StringTokenizer tokenizer = new StringTokenizer(value, " ");
                while (tokenizer.hasMoreTokens()) {
                    String curValue = tokenizer.nextToken();
                    Query subSubQuery = createTermQuery(field, curValue, relation);
                    AndQuery((BooleanQuery) termQuery, subSubQuery);
                }
            }

        }

        return termQuery;
    }

    /**
     * Join the two queries together with boolean AND
     * @param query
     * @param query2
     */
    public static void AndQuery(BooleanQuery query, Query query2) {
        /**
         * required = true (must match sub query)
         * prohibited = false (does not need to NOT match sub query)
         */
        query.add(query2, BooleanClause.Occur.MUST);
    }

    public static void OrQuery(BooleanQuery query, Query query2) {
        /**
         * required = false (does not need to match sub query)
         * prohibited = false (does not need to NOT match sub query)
         */
        query.add(query2, BooleanClause.Occur.SHOULD);
    }

    public static void NotQuery(BooleanQuery query, Query query2) {
        /**
         * required = false (does not need to match sub query)
         * prohibited = true (must not match sub query)
         */
        query.add(query2, BooleanClause.Occur.MUST_NOT);
    }

    private static void dumpQueryTree(CQLNode node) {
        if(node instanceof CQLBooleanNode) {
            CQLBooleanNode cbn=(CQLBooleanNode)node;
            dumpQueryTree(cbn.left);
            if(node instanceof CQLAndNode)
                log.info(" AND ");
            else if(node instanceof CQLNotNode)
                log.info(" NOT ");
            else if(node instanceof CQLOrNode)
                log.info(" OR ");
            else log.info(" UnknownBoolean("+cbn+") ");
            dumpQueryTree(cbn.right);
        }
        else if(node instanceof CQLTermNode) {
            CQLTermNode ctn=(CQLTermNode)node;
            log.info("term(qualifier=\""+ctn.getIndex()+"\" relation=\""+
                ctn.getRelation().getBase()+"\" term=\""+ctn.getTerm()+"\")");
        }
        else log.info("UnknownCQLNode("+node+")");
    }
    
    
    public static void main(String[] args){
    	try {
    		System.setProperty("cql.serverChoice", "title");
			CQLParser parser=new CQLParser();

			CQLNode queryRoot = parser.parse("\"caudal or dorsal\" and vertebra");
			LuceneTranslator lt=new LuceneTranslator("D:\\Indexes\\idxCLEF_new\\index","title");
			LuceneTranslator.terms =new HashSet<String>();
			Query qr = makeQuery(queryRoot);
			System.out.println(">> "+ qr.toString());
			for(String s :LuceneTranslator.terms)
				System.out.println("\t- "+ s);
		} catch (Exception e) {
			e.printStackTrace();
		}
    	
    }
}

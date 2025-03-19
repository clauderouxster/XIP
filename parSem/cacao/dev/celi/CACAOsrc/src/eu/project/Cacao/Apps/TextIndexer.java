package eu.project.Cacao.Apps;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.HashSet;
import java.util.Vector;

import org.apache.log4j.Logger;
import org.apache.lucene.analysis.PerFieldAnalyzerWrapper;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.store.LockObtainFailedException;


import eu.project.Cacao.Apps.LuceneIndexMerger.MySimpleAnalyzer;

import eu.project.Cacao.Components.NamedEntitiesDetector;
import eu.project.Cacao.Components.Utils.Lemmatizer;
import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Components.Utils.PolishLemmatizer;
import eu.project.Cacao.Components.Utils.RunTimer;
import eu.project.Cacao.Components.Utils.XIPwrapper;

public class TextIndexer {

	private Logger logger = Logger.getLogger(TextIndexer.class);
	private Lemmatizer xipWrapper;
	private IndexWriter writer;
	private String lang;
	private static int globalCnt=0;
	
	public TextIndexer(String indexOutputFolder, String lang ) throws CorruptIndexException, LockObtainFailedException, IOException {
		this.lang= lang;
		
		this.initXIPGrammarsPaths();
		
		PerFieldAnalyzerWrapper pfan = getAnalyzer();
		this.writer = new IndexWriter(indexOutputFolder, pfan, true, IndexWriter.MaxFieldLength.LIMITED);
	}

	public static void main(String[] args) throws IOException, InterruptedException {
		Logger logger = Logger.getLogger(TextIndexer.class);
		
		String repository=args[0];
		String indexOutputFolder = args[1];
		String lang=args[2];
		String cmd=args[3];
		
		if(cmd.equalsIgnoreCase("compact")){
			File f=new File(indexOutputFolder);
			if(!f.exists())
				f.mkdirs();
			convertFiles(repository,f);
		}
		else{
		TextIndexer idx = new TextIndexer(indexOutputFolder, lang);
		
		RunTimer rt = new RunTimer();
		logger.info("Indexing to directory '" + indexOutputFolder + "'...");
	
		idx.getCollectionsFromFiles(repository);
		
		idx.writer.optimize();
		idx.writer.close();
		logger.info("Indexed tot records :" + idx.globalCnt + " - time elapsed: " + rt.getElapsedTimeLabel());
		}
	}
	

	private void initXIPGrammarsPaths() {
		if(this.lang.equalsIgnoreCase("it"))
			this.xipWrapper = new XIPwrapper(XIPwrapper.itGram, "it");
		else if(this.lang.equalsIgnoreCase("en"))
			this.xipWrapper = new XIPwrapper(XIPwrapper.enGram, "en");
		else if(this.lang.equalsIgnoreCase("fr"))
			this.xipWrapper = new XIPwrapper(XIPwrapper.frGram, "fr");
		else if(this.lang.equalsIgnoreCase("de"))
			this.xipWrapper = new XIPwrapper(XIPwrapper.deGram, "de");
		else if(this.lang.equalsIgnoreCase("pl"))
			this.xipWrapper = new PolishLemmatizer();
	}

	private Vector<NLPtoken> lemmatize( String toBeLemmatized) {
		try {
			logger.info("lemmatizing "+toBeLemmatized);
			Vector<NLPtoken> list = new Vector<NLPtoken>();
			list = this.xipWrapper.parsePhrase(toBeLemmatized);
			//list = this.ne.detectNE(list, this.lang);
			return list;
		} catch (Exception e) {
			return new Vector<NLPtoken>();
		}
	}

	
	private PerFieldAnalyzerWrapper getAnalyzer() {
		PerFieldAnalyzerWrapper pfan = new PerFieldAnalyzerWrapper(new MySimpleAnalyzer());
		pfan.addAnalyzer("contents", new StandardAnalyzer());
		
		return pfan;
	}
	
	
	public static void convertFiles(String repository, File directory) throws IOException, InterruptedException {
		Logger logger = Logger.getLogger(TextIndexer.class);
		File dir = new File(repository);
		logger.info("-Descending into folder :"+dir.getAbsolutePath());	
		File[] files = dir.listFiles();
		PrintWriter pw=null;
		for (File f : files) {
			if (f.isDirectory()) {
				logger.info("-Descending into folder :"+f.getAbsolutePath());
				convertFiles(f.getAbsolutePath(), directory);

			} else {
				if(pw==null){
					pw=new PrintWriter(new File(directory,"webCorpusDoc_"+globalCnt+".txt"),"UTF-8");
					globalCnt++;
				}
				BufferedReader br  = new BufferedReader(new InputStreamReader(new FileInputStream(f.getAbsolutePath()),"CP1252"));
				StringBuffer buff=new StringBuffer("");
				if(br.ready()){
					
					
					String url=br.readLine();
					if(url.length()>2){
						url=url.substring(1,url.length()-2);
					}
					
					while(br.ready()){
						buff=buff.append(br.readLine()+" ");
					}
					
					String tmp=buff.toString().trim();
					pw.println(url);
					pw.println(tmp);
					pw.flush();
				}
			}
		}
		if(pw!=null)
			pw.close();
		return;
	}

	public void getCollectionsFromFiles(String repository) throws IOException {
		File dir = new File(repository);
		logger.info("-Descending into folder :"+dir.getAbsolutePath());	
		File[] files = dir.listFiles();

		for (File f : files) {
			if (f.isDirectory()) {
				logger.info("-Descending into folder :"+f.getAbsolutePath());
				getCollectionsFromFiles(f.getAbsolutePath());

			} else {
				indexFromSingleFile(f.getAbsolutePath());
				logger.info("processed file: "+f.getAbsolutePath());
			}
		}
		return;
	}
	
	public void indexFromSingleFile(String fname) throws CorruptIndexException, IOException{
		
		BufferedReader br=new BufferedReader(new FileReader(fname));
		
		while(br.ready()){
			Document doc =new Document();
			
			doc.add(new Field("documentID", ""+globalCnt, Field.Store.YES, Field.Index.NOT_ANALYZED));
			
			String url=br.readLine();
			doc.add(new Field("URL", url, Field.Store.YES, Field.Index.NOT_ANALYZED));
			
			String tmp=br.readLine();
			
			doc.add(new Field("contents", tmp, Field.Store.YES, Field.Index.ANALYZED));
			
			HashSet<String> splitted=this.splitIntoSentences(tmp);
			StringBuffer lemmatized=new StringBuffer();
			for(String sentence: splitted){
				Vector<NLPtoken> analyzed =this.lemmatize( sentence);
				
				if (analyzed.size() > 0) {
					String lemmatizedStr = "";
					for (NLPtoken tok : analyzed) {
						lemmatizedStr += tok.getLemma_Category() + " ";
						if (tok.isNE) {
							doc.add(new Field("NE_"+tok.neCategory, tok.surface, Field.Store.YES, Field.Index.ANALYZED));
						}
					}
					if (lemmatizedStr.length() > 0)
						lemmatized =lemmatized.append(lemmatizedStr);
				}
			}
			doc.add(new Field("contents_lemmatized", lemmatized.toString().trim(), Field.Store.YES, Field.Index.ANALYZED));
			
			this.writer.addDocument(doc);
			
			if(globalCnt %10 ==0)
				logger.info(globalCnt);
			
			globalCnt++;
		}
		br.close();
	}

	private HashSet<String> splitIntoSentences(String tmp) {
		HashSet<String> splitted=new HashSet<String>();
		int idx=tmp.indexOf(". ");
		if(idx<0){
			splitted.add(tmp);
		}
		else{
			int begin=0;
			while(idx>0){
				String sentence=tmp.substring(begin,idx+1);
				splitted.add(sentence);
				begin=idx+1;
				idx=tmp.indexOf(". ",begin);
			}
			String sentence=tmp.substring(begin);
			if(sentence.trim().length()>0)
				splitted.add(sentence);
		}
		return splitted;
	}
}


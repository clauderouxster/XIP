package com.xerox.xrce.uimaxip.core;


import java.io.File;
import java.net.URL;
import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Properties;

import jcoref.Referent;

import org.apache.uima.UimaContext;
import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.FSIterator;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.tcas.Annotation;
import org.apache.uima.resource.ResourceInitializationException;

import com.xerox.jxip.JXip;
import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import com.xerox.xrce.uima.sharedtypes.TextBlock;
import com.xerox.xrce.uima.sharedtypes.TextBlockAnnotated;


/**
 * This abstract class provides functionality to facilitate wrapping XIP in an
 * application-specific UIMA annotator.
 * 
 * @author akaplan
 *
 */
public abstract class UimaXip_Base extends JCasAnnotator_ImplBase {
	

	/*
	 *  Any of the following methods can be overridden in the implementation if desired.
	 */
	protected void processReferent(Referent r, JCas cas, int offset) {}
	
	protected void processDependency(XipDependency dep, JCas cas, int offset) throws AnalysisEngineProcessException {}
	
	protected void processToken(XipNode token, JCas cas, int offset) {}
	
	protected void startDocument(JCas cas) {}
	
	protected void endDocument(JCas cas) {}
	
	protected void processXipUnit(XipUnit unit, JCas cas, int offset) throws AnalysisEngineProcessException {
		for (XipDependency dep : unit.getDependencies()) {
			processDependency(dep, cas, offset);
		}

		for (XipNode xipToken : unit.getLeafNodes()) {
			processToken(xipToken, cas, offset);		
		}
	}

	protected Iterator<TextBlock> getTextBlockIterator(final JCas cas) {
		if (cas.getJFSIndexRepository().getAllIndexedFS(TextBlockAnnotated.type).hasNext()){
			return new Iterator<TextBlock>() {
				private FSIterator<Annotation> textBlockIterator;
				{ textBlockIterator = cas.getAnnotationIndex(TextBlock.type).iterator(); }
				public boolean hasNext() { return textBlockIterator.hasNext(); }
				public TextBlock next() {
					TextBlock textBlock = (TextBlock) textBlockIterator.next();
					if (textBlock == null) {
						throw new NoSuchElementException();
					}
					return textBlock;
				}
				public void remove() {
					throw new UnsupportedOperationException();			
				}
			};
		} else {
			return new Iterator<TextBlock>() {
				private TextBlock textBlock;
				{ textBlock = new TextBlock(cas,0,cas.getDocumentText().length());	}

				public boolean hasNext() {
					return textBlock != null;
				}
				public TextBlock next() {
					if (textBlock == null) {
						throw new NoSuchElementException();
					} else {
						TextBlock result = textBlock;
						textBlock = null;
						return result;
					}
				}
				public void remove() {
					throw new UnsupportedOperationException();			
				}

			};
		}
	}

	/*
	 * End of methods to be overridden by implementation class
	 */

	
	
	private static JXip jxip;
	static {
	    try {
	        jxip = new JXip();
	    } catch (Exception e) {
	        throw new RuntimeException(e);
	    }
	}
	
	public static JXip getJXip() {
	    return jxip;
	}
	
	protected boolean doCoref = false; // todo: replace with reflection on method processReferent?
	public Parser parser;
	
	/**
	 * Will be set from properties file
	 */
	public String main_root;

	/**
	 * The dotted string name of the resource bundle containing this class' internationalizable strings 
	 */
	public static String stringsResourceName = UimaXip_Base.class.getName()+ "Strings";
	
	/**
	 * Absolute pathnames that are set in properties files that will be modified by
	 * the PEAR installer
	 */
	protected Properties properties;
	

	
	
	protected String getPropOrError(String prop) throws ResourceInitializationException {
		String result;
		result = properties.getProperty(prop);
		if (result == null) {
			throw new ResourceInitializationException(stringsResourceName, "PropertyUndef",
					new Object[] {prop});
		}
		return result;
	}

	/** 
	 * @see org.apache.uima.analysis_component.AnalysisComponent_ImplBase#initialize(org.apache.uima.UimaContext)
	 */
	public void initialize(UimaContext context)
		throws ResourceInitializationException {

		try {
			// If the system property is set, use its value as the path to the grm file.
			// Otherwise, look for it as a java resource on the classpath, using the
			// annotator class's fully qualified name as a prefix to avoid the risk of
			// one annotator loading another annotator's grammar.
			// todo: this scheme prevents me from intentionally sharing one grammar
			// among multiple annotators.  Do I want to be able to do that?
			String grmFilePath = System.getProperty("com.xerox.xrce.uimaxip.core.grammar.grmfile");
			if (context.getResourceFilePath("grammarPath") != null){
				grmFilePath = context.getResourceFilePath("grammarPath");
			}
			if (grmFilePath == null) {
					
				String gramResourcePath = "/"
						+ getClass().getName().replaceAll("\\.", "/")
						+ "-grammar/grammar.grm";

				URL gramResourceUrl = getClass().getResource(gramResourcePath);

				if (gramResourceUrl == null) {
					throw new ResourceInitializationException(stringsResourceName,
															  "GramNotFound",
															  new Object[] {gramResourcePath,
																			null});
				} else if (!gramResourceUrl.getProtocol().equals("file")) {
					throw new ResourceInitializationException(stringsResourceName,
							  "GramNotFound",
							  new Object[] {gramResourcePath,
											gramResourceUrl});
				}
				grmFilePath = new File(gramResourceUrl.toURI()).getPath();
			}
			
			if (doCoref) {
				parser = new CorefParser(grmFilePath);
			} else {
				parser = new RegularParser(grmFilePath);
			}
		} catch (Exception ex) {
			throw new ResourceInitializationException(ex);
		}
		super.initialize(context);
	}

	/**
	 * @see org.apache.uima.analysis_component.JCasAnnotator_ImplBase#process(org.apache.uima.jcas.JCas)
	 */
	public void process(JCas cas) throws AnalysisEngineProcessException {
		startDocument(cas);
		Iterator<TextBlock> itr = getTextBlockIterator(cas);
		while (itr.hasNext()) {
			processTextBlock(itr.next(), cas);
		}
		endDocument(cas);
	}
	
	

	protected void processTextBlock(TextBlock textBlock, JCas cas) throws AnalysisEngineProcessException {
		int begin = textBlock.getBegin();
		int end = textBlock.getEnd();
		String text = cas.getDocumentText();
		try {
			parser.parseString(text.substring(begin, end));
		} catch (Exception e) {
			throw new AnalysisEngineProcessException(e);
		}
		if (doCoref) {
			for (Referent r : parser.getReferents()) {
				processReferent(r, cas, begin);
			}
		}
		for (XipUnit xipUnit : parser.getXipUnits()) {
			processXipUnit(xipUnit, cas, begin);
		}
		
	}
	
}

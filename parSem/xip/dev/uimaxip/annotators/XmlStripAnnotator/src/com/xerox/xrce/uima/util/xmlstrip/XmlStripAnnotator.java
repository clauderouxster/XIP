package com.xerox.xrce.uima.util.xmlstrip;

import java.io.StringReader;
import java.util.HashSet;
import java.util.Set;
import java.util.Stack;

import org.apache.uima.UimaContext;
import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.CAS;
import org.apache.uima.cas.Feature;
import org.apache.uima.cas.FeatureStructure;
import org.apache.uima.cas.Type;
import org.apache.uima.jcas.JCas;
import org.apache.uima.resource.ResourceInitializationException;
import org.apache.xerces.parsers.XMLDocumentParser;
import org.apache.xerces.xni.Augmentations;
import org.apache.xerces.xni.QName;
import org.apache.xerces.xni.XMLAttributes;
import org.apache.xerces.xni.XMLResourceIdentifier;
import org.apache.xerces.xni.XMLString;
import org.apache.xerces.xni.XNIException;
import org.apache.xerces.xni.parser.XMLEntityResolver;
import org.apache.xerces.xni.parser.XMLInputSource;
import org.apache.xerces.xni.parser.XMLParserConfiguration;
import org.cyberneko.html.HTMLConfiguration;
import org.cyberneko.html.HTMLEventInfo;

import com.xerox.xrce.uima.sharedtypes.TextBlock;
import com.xerox.xrce.uima.sharedtypes.TextBlockAnnotated;
import com.xerox.xrce.uima.util.xmlstrip.uimatypes.OffsetBlock;

public class XmlStripAnnotator extends JCasAnnotator_ImplBase {
	
	private static Set<String> ignoreTags;
	private static Set<String> noBreakTags;
	private static Set<String> annotTags;
	
	private static String annotPackage = com.xerox.xrce.uima.xhtml.uimatypes.I.class.getPackage().getName();
	
	private String documentText; 
	
	static {
		
		// TODO: read tag lists from a config file
		
		// Names of tags of which the contents will be ignored, rather than treated as text to be processed.
		ignoreTags = new CaseInsensitiveStringSet();
		ignoreTags.add("script");
		ignoreTags.add("style");
		ignoreTags.add("object");
		ignoreTags.add("map");
		ignoreTags.add("embed");
		ignoreTags.add("applet");

		// Names of tags that are not sentence boundaries (any tag not listed here is treated as 
		// a sentence boundary)
		noBreakTags = new CaseInsensitiveStringSet();
		noBreakTags.add("a");
		noBreakTags.add("i");
		noBreakTags.add("b");
		noBreakTags.add("em");
		noBreakTags.add("s");
		noBreakTags.add("strike");
		noBreakTags.add("tt");
		noBreakTags.add("u");
		noBreakTags.add("strong");
		noBreakTags.add("font");
		noBreakTags.add("big");
		noBreakTags.add("small");
		
		// Tags that will be turned into UIMA annotations
		// When adding a new one, need to add it to XhtmlTypeSystem as well.
		annotTags = new CaseInsensitiveStringSet();
		annotTags.add("i");
		annotTags.add("b");
		annotTags.add("em");
		annotTags.add("strong");
		annotTags.add("big");
		annotTags.add("small");
		annotTags.add("u");
		annotTags.add("h1");
		annotTags.add("h2");
		annotTags.add("h3");
		annotTags.add("h4");
		annotTags.add("h5");
		annotTags.add("h6");
		annotTags.add("title");
		
	}
	
	private static String initialCap(String s) {
		if (s.length() == 0) {
			return new String(s);
		} else {
			return s.substring(0,1).toUpperCase() + s.substring(1).toLowerCase();
			// Although the xerces NonValidatingConfiguration normalizes element names to
			// lowercase, the neko HTMLConfiguration doesn't.  Hence the toLowerCase() above.
		}
	}

	private Parser parser;
	
	static String AUGMENTATIONS = "http://cyberneko.org/html/features/augmentations";
	static String IGNORE_OUTSIDE_CONTENT = "http://cyberneko.org/html/features/balance-tags/ignore-outside-content";

	@Override
	public void initialize(UimaContext aContext) throws ResourceInitializationException {
		//XMLParserConfiguration parserConfig = new NonValidatingConfiguration();
		//parserConfig.setFeature("http://apache.org/xml/features/nonvalidating/load-external-dtd", false);
		
		XMLParserConfiguration parserConfig = new HTMLConfiguration();
		parserConfig.setFeature(AUGMENTATIONS, true);
		// Workaround for bug in neko v. 1.9.11
		// https://sourceforge.net/tracker/index.php?func=detail&aid=2538071&group_id=195122&atid=952178
		parserConfig.setFeature(IGNORE_OUTSIDE_CONTENT, true);
		
		parserConfig.setEntityResolver(new XMLEntityResolver() {
			public XMLInputSource resolveEntity(XMLResourceIdentifier id) {
				return new XMLInputSource(null, null, null, new StringReader(""), null);
			}
		});
		parser = new Parser(parserConfig);
	}
	
	@Override
	public void process(JCas cas) throws AnalysisEngineProcessException {
		JCas xmlView;
		JCas txtView;
		try {
			// TODO: allow the names of the views to be configured
			xmlView = cas.getView("_InitialView");
			txtView = cas.createView("txt");
			parser.reset(txtView);
			documentText = xmlView.getDocumentText();
			parser.parse(new XMLInputSource(null, null, null,
											new StringReader(documentText),
											null));
			txtView.setDocumentText(parser.getTxt());
			new TextBlockAnnotated(txtView).addToIndexes();
		} catch (Exception e) {
			throw new AnalysisEngineProcessException(e);
		} catch (StackOverflowError e) {
			throw new AnalysisEngineProcessException(e);
		}
	}

	private class Parser extends XMLDocumentParser {
		private Stack<String> cutStack;
		private Stack<FeatureStructure> annotStack;
		private StringBuffer txt;
		private JCas jcas;
		private int textBlockStart;
		// if adding a new field, be sure to reset it in reset()
		
		public Parser(XMLParserConfiguration parserConfig) {
			super(parserConfig);
			cutStack = new Stack<String>();
			annotStack = new Stack<FeatureStructure>();
			txt = new StringBuffer();
		}
		
		public String getTxt() {
			return txt.toString();
		}

		public void reset(JCas newCas) {
			super.reset();
			cutStack.clear();
			annotStack.clear();
			txt.setLength(0);
			jcas = newCas;
			textBlockStart = 0;
		}

		public void startElement(QName element, XMLAttributes attributes, Augmentations augs)
			throws XNIException {
			if (ignoreTags.contains(element.localpart)) {
				cutStack.push(element.localpart);
			}
			if (annotTags.contains(element.localpart)) {
				CAS cas = jcas.getCas();
				String typeName = annotPackage + "." + initialCap(element.localpart);
				org.apache.uima.cas.TypeSystem ts = cas.getTypeSystem();
				Type type = ts.getType(typeName);
				assert type != null;
				FeatureStructure annot = cas.createFS(type);
				Feature beginFeat = type.getFeatureByBaseName("begin");
				annot.setIntValue(beginFeat, txt.length());
				annotStack.push(annot);
			}
			maybeBreakTextBlock(element.localpart);
		}
		
		public void endElement (QName element, Augmentations augs) {
			if (ignoreTags.contains(element.localpart)) {
				assert cutStack.peek().equals(element.localpart);
				cutStack.pop();
			}
			if (annotTags.contains(element.localpart)) {
				FeatureStructure annot = annotStack.pop();
				assert annot.getType().getName().equals(annotPackage + "." + initialCap(element.localpart));
				Feature endFeat = annot.getType().getFeatureByBaseName("end");
				annot.setIntValue(endFeat, txt.length());
				jcas.getCas().addFsToIndexes(annot);
			}
			maybeBreakTextBlock(element.localpart);
		}

		public void characters (XMLString newText, Augmentations augs) throws XNIException {
			// In neko, HTMLScanner.java line 2637 calls characters() with augs=null.
			// This code path is followed when parsing the string <$BlogMetaData$> (the dollar
			// sign is not valid in an element name, so the parser reads this as ordinary
			// text rather than a tag).  The null augs looks like a bug to me, and I'm going to
			// pursue it with the neko authors.  In the meantime, my workaround is just to drop
			// the < sign.
			if (augs==null) return;
			HTMLEventInfo info = (HTMLEventInfo) augs.getItem(AUGMENTATIONS);
			if (cutStack.empty()) {
				int txtStart = txt.length();
				// I'm aware of two reasons for which the length of newText (in characters) may be
				// different from that of the corresponding part of the original text:
				//
				//    - When the original text is a character entity e.g. &amp; , the new
				//      text is a single character.  We use the single-character version in
				//      the new view because the new view isn't supposed to have any XML/HTML
				//      in it.  This shouldn't cause a problem with offsets because I observe
				//      (empirically, don't know if this is documented) that the character gets
				//      reported in a separate call to characters(), so it gets its own
				//      one-character offset block, so there's no risk of offsets being
				//      misaligned within the block.
				//
				//    - Line endings are normalized to single-character unix line endings,
				//      so if the original text used DOS line endings there's a mismatch.
				//      In this case we preserve the original encoding in order to preserve
				//      character alignment.
				//
				// If there are other cases that I didn't know about, an assertion below
				// will fail.
				if (newText.length == 1 &&
						documentText.charAt(info.getBeginCharacterOffset()) == '&' &&
						documentText.charAt(info.getEndCharacterOffset()-1) == ';')
				{
					txt.append(newText.ch[newText.offset]);
				} else {
					String oldText = documentText.substring(info.getBeginCharacterOffset(), info.getEndCharacterOffset());
					assert checkAlignment(newText, oldText);
					txt.append(oldText);
				}
				int txtEnd = txt.length();
				OffsetBlock b = new OffsetBlock(jcas, txtStart, txtEnd);
				b.setXmlBegin(info.getBeginCharacterOffset());
				b.setXmlEnd(info.getEndCharacterOffset());
				b.addToIndexes();
			}
		}

		private boolean checkAlignment(XMLString newText, String oldText) {
			int oldi = 0;
			int newi = 0;
			while (oldi < oldText.length()) {
				char oldc = oldText.charAt(oldi);
				char newc = newText.ch[newText.offset+newi];
				if (oldc != newc) {
					if (oldc == '\r' && newc == '\n') {
						oldi++;
					} else {
						return false;
					}
				}
				oldi++;
				newi++;
			}
			return true;
		}
		
		private void maybeBreakTextBlock(String tagname) {
			if (!noBreakTags.contains(tagname) && txt.length() > textBlockStart) {
				if (containsNonWhitespace(txt,textBlockStart)) {
					TextBlock textBlock = new TextBlock(jcas, textBlockStart, txt.length());
					textBlock.addToIndexes();
				}
				textBlockStart = txt.length();
			}
		}
		
		private boolean containsNonWhitespace(StringBuffer s, int start) {
			for (int i = start; i < s.length(); i++) {
				if (!Character.isWhitespace(s.charAt(i)) && !Character.isSpaceChar(s.charAt(i))) {
					return true;
				}
			}
			return false;
		}

		
	}
	
	// Although the xerces NonValidatingConfiguration normalizes element names to
	// lowercase, the neko HTMLConfiguration doesn't.
	private static class CaseInsensitiveStringSet extends HashSet<String> {
		private static final long serialVersionUID = 1L;
		@Override
		public boolean contains(Object o) {
			if (o == null) {
				return super.contains(null);
			} else if (o instanceof String) {
				return super.contains(((String) o).toLowerCase());
			} else {
				return false;
			}
		}
		@Override
		public boolean add(String s) {
			return super.add(s.toLowerCase());
		}
	}
	

}

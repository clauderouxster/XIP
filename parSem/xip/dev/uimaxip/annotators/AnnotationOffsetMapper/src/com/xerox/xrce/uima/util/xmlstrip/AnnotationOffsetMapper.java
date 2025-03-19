package com.xerox.xrce.uima.util.xmlstrip;

import java.util.HashSet;
import java.util.Set;

import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.CASException;
import org.apache.uima.cas.FSIndex;
import org.apache.uima.cas.FSIterator;
import org.apache.uima.cas.Feature;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.tcas.Annotation;
import org.apache.uima.jcas.tcas.DocumentAnnotation;

import com.xerox.xrce.uima.util.xmlstrip.uimatypes.OffsetBlock;

public class AnnotationOffsetMapper extends JCasAnnotator_ImplBase {
	
	@Override
	public void process(JCas cas) throws AnalysisEngineProcessException {
		JCas xmlView;
		JCas txtView;
		try {
			xmlView = cas.getView("_InitialView");
			txtView = cas.getView("txt");
		} catch (CASException e) {
			throw new AnalysisEngineProcessException(e);
		}
				
		FSIterator allAnnotIterator = txtView.getAnnotationIndex().iterator();
		FSIndex offsetBlockIndex = txtView.getAnnotationIndex(OffsetBlock.type);
		FSIterator firstobIterator = offsetBlockIndex.iterator();
		while (allAnnotIterator.hasNext()) {
			// The begin and end of this annotation are each within an OffsetBlock; not necessarily the same OffsetBlok 
			Annotation annot = (Annotation) allAnnotIterator.next();
			if (copyable(annot)) {
				// advance firstobIterator to the first OffsetBlock whose begin is > annot.begin, then back up one.
				while (firstobIterator.isValid() && ((OffsetBlock) firstobIterator.get()).getBegin() <= annot.getBegin()) {
					firstobIterator.moveToNext();
				}
				firstobIterator.moveToPrevious();
				OffsetBlock firstob = (OffsetBlock) firstobIterator.get();

				FSIterator lastobIterator = offsetBlockIndex.iterator(firstob);
				while (((OffsetBlock) lastobIterator.get()).getEnd() < annot.getEnd()) {
					lastobIterator.moveToNext();
					assert lastobIterator.isValid(); // all text comes from OffsetBlocks, so all annotations should be within OffsetBlocks.
				}
				OffsetBlock lastob = (OffsetBlock) lastobIterator.get();

				Annotation newAnnot = (Annotation) annot.clone();
				newAnnot.setBegin(firstob.getXmlBegin() + annot.getBegin() - firstob.getBegin());
				newAnnot.setEnd(lastob.getXmlEnd() - (lastob.getEnd() - annot.getEnd()));
				Feature sofaFeature = newAnnot.getType().getFeatureByBaseName("sofa");
				newAnnot.setFeatureValue(sofaFeature, xmlView.getSofa());
				newAnnot.addToIndexes(xmlView);
			}
		}
	}
	
	private static Set<Integer> ignoreTypes;
	static {
		// TODO: is typeIndexID the right field to use?
		// TODO: make these configurable
		ignoreTypes = new HashSet<Integer>();
		ignoreTypes.add(DocumentAnnotation.typeIndexID);
		ignoreTypes.add(OffsetBlock.typeIndexID);
	}
	

	protected boolean copyable(Annotation annot) {
		return (!ignoreTypes.contains(annot.getTypeIndexID())); 
	}

}

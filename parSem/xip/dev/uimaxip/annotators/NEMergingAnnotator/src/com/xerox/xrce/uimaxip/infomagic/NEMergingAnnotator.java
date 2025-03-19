package com.xerox.xrce.uimaxip.infomagic;

import infomagic.types.*;

import java.util.Vector;
import java.util.List;
import java.util.Iterator;
import java.util.Enumeration;

import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.CASException;
import org.apache.uima.cas.FSIterator;
import org.apache.uima.cas.TypeSystem;
import org.apache.uima.jcas.cas.FSArray;
import org.apache.uima.jcas.JCas;
import org.apache.uima.cas.CAS;
import org.apache.uima.jcas.tcas.Annotation;
import org.apache.uima.cas.Feature;
import java.util.Hashtable;


public class NEMergingAnnotator extends JCasAnnotator_ImplBase {
	private boolean displayConflicts = false;
	private boolean cleanRedundantNamedEntity = true;
	private boolean finestAnnotation = true;
	private boolean deleteConflictualAnnotations = true;
	private boolean majoritaryVoteForConflict = true;
	//beginOfPreviousAnnot et endOfPreviousAnnot contiennent les annotations qui vont être comparées avec l'annotation courant (annot)
	private Hashtable<Integer,Vector<NamedEntity>> beginOfPreviousAnnot;
	private Hashtable<Integer,Vector<NamedEntity>> endOfPreviousAnnot;
	
	//beginOfTextBlocks et endOfTextBlocks contiennent les textBlocks à traiter par les annotateurs
	private Hashtable<Integer,Integer> beginAndEndOfTextBlocks;
	

	//contient les annotations de type Fusion (ou un des soustypes) qui peuvent être affichée ou non (displayConflicts true or false)
	private Vector<Fusion> fusionToAdd;

	//contient les annotations de type NamedEntity (ou un des soustypes sauf Fusion)
	//à ajouter ou supprimer, affichable ou non (cleanRedundantNamedEntity true or false)
	private Vector<NamedEntity> NEtoAdd;
	private Vector<NamedEntity> NEtoRemove;
	
	
	//private Vector<Integer> TextBlockEnds;
	
	@SuppressWarnings("unchecked")
	@Override
	public void process(JCas cas) throws AnalysisEngineProcessException {
		JCas xmlView;
		CAS cas_init;
		try {
			xmlView = cas.getView("_InitialView");
			cas_init = xmlView.getCas();
		} catch (CASException e) {
			throw new AnalysisEngineProcessException(e);
		}
			
		preprocessing(cas, xmlView);
		
		TypeSystem ts = cas.getTypeSystem();
		
		//contient les annotations de type Fusion (ou un des soustypes) qui peuvent être affichée ou non (displayConflicts true or false)
		fusionToAdd = new Vector<Fusion>();


		//test pour eval en supprimant un des annotateurs
		boolean test = false;
		String sourceToDelete = "Xerox";
		if (test){
			System.out.println("Attention : suppression des annotations de "+sourceToDelete);
		}
		
		

		//beginOfPreviousAnnot et endOfPreviousAnnot contiennent les annotations qui vont être comparées avec l'annotation courant (annot)
		beginOfPreviousAnnot = new Hashtable<Integer,Vector<NamedEntity>>();
		endOfPreviousAnnot = new Hashtable<Integer,Vector<NamedEntity>>();
		
		//contient les annotations de type NamedEntity (ou un des soustypes sauf Fusion)
		//à ajouter ou supprimer, affichable ou non (cleanRedundantNamedEntity true or false)
		NEtoAdd = new Vector<NamedEntity>();
		NEtoRemove = new Vector<NamedEntity>();
		int begin = 0;
		int end = 0;
		
		
		
		beginAndEndOfTextBlocks = new Hashtable<Integer,Integer>();
		
		//stockage des debuts et fins des TextBlock pour supprimer les entités qui ne sont pas inclues dans un TextBlock
		FSIterator allAnnotIterator = xmlView.getAnnotationIndex().iterator();
		while (allAnnotIterator.hasNext()) {
			Annotation annot_init = (Annotation) allAnnotIterator.next();
			if (annot_init.getType().getShortName().equals("TextBlock")){
				beginAndEndOfTextBlocks.put(annot_init.getBegin(), annot_init.getEnd());
			}
		}

			
		//ETAPE 1 : traitement des mergings
		
			
		allAnnotIterator = xmlView.getAnnotationIndex().iterator();
		while (allAnnotIterator.hasNext()) {
			Annotation annot_init = (Annotation) allAnnotIterator.next();
			if (!annot_init.getType().getName().startsWith("infomagic.types.")){
				continue;
			}
			NamedEntity annot = (NamedEntity) annot_init;
			begin = annot.getBegin();
			end = annot.getEnd();
			
			
			//suppression des entités à cheval entre textblock
			if (!isInTextBlock(begin,end,beginAndEndOfTextBlocks)){
				NEtoRemove.add(annot);
				continue;
			}
			
			//test pour eval en supprimant un des annotateurs
			if (test && annot.getSource().equals(sourceToDelete)){
				NEtoRemove.add(annot);
				continue;
			}
			
			//ajout, si necessaire du canonical string
			annot.setCanonicalString(annot.getCoveredText());
			
			
			
			
			
			Vector<NamedEntity> listPreviousAnnot = getPreviousAnnot(begin,end); 
			
			if (listPreviousAnnot != null){
				Iterator<NamedEntity> i = listPreviousAnnot.iterator();
				NamedEntity parent = null;
				while (i.hasNext()) {
					NamedEntity previousAnnot = (NamedEntity) i.next();
					
					//cas special double annotation (DateAndTime & NumericalExpression) d'Arisem  
					if (annot.getType().getShortName().equals("NumericalExpression") &&
							  ts.subsumes(ts.getType("infomagic.types.TemporalExpression"), previousAnnot.getType()) &&
							  annot.getSource().equals("Arisem") &&
							  previousAnnot.getSource().equals("Arisem")){
						NEtoRemove.add(annot);
	
					//cas special double annotation (DateAndTime & NumericalExpression) d'Arisem 
					}else if (ts.subsumes(ts.getType("infomagic.types.TemporalExpression"), annot.getType()) &&
							  previousAnnot.getType().getShortName().equals("NumericalExpression") &&
							  annot.getSource().equals("Arisem") &&
							  previousAnnot.getSource().equals("Arisem")){
						NEtoRemove.add(previousAnnot);
						replaceInPreviousAnnot(previousAnnot,annot,begin,end);
	
					//cas de double annotation par le même annotateur 
					}else if (previousAnnot.getType().equals(annot.getType()) &&
							  annot.getSource().equals(previousAnnot.getSource())){
						NEtoRemove.add(annot);
	
					//cas 1.a : même offsets, même annotation
					}else if (previousAnnot.getType().getShortName().equals(annot.getType().getShortName())){
						createMerging(xmlView,previousAnnot,annot,begin,end,false);
	
					//cas 1.b.1 : même offsets, le type de previousAnnot est un sous type de annot
					}else if ((finestAnnotation && ts.subsumes(previousAnnot.getType(), annot.getType())) ||
							  (!finestAnnotation && ts.subsumes(annot.getType(), previousAnnot.getType()))){
						createMerging(xmlView,previousAnnot,annot,begin,end,false);
	
					//cas 1.b.2 : même offsets, le type de annot est un sous type de previousAnnot
					}else if ((finestAnnotation && ts.subsumes(annot.getType(), previousAnnot.getType())) ||
							(!finestAnnotation && ts.subsumes(previousAnnot.getType(), annot.getType()))){
						createMerging(xmlView,annot,previousAnnot,begin,end,false);
	
					//cas 1.c.1 : même offsets, annotations différentes mais dans même hierarchie
					}else if (ts.subsumes(ts.getParent(previousAnnot.getType()), ts.getParent(annot.getType())) &&
							 !ts.getParent(previousAnnot.getType()).getShortName().equals("NamedEntity")){
						//créer une nouvelle annotation parent
						parent = (NamedEntity) cas_init.createAnnotation(ts.getParent(previousAnnot.getType()), begin, end);
						parent.setSource(previousAnnot.getSource());
						NEtoRemove.add(previousAnnot);
						NEtoAdd.add(parent);
						removeInPreviousAnnot(previousAnnot,begin,end);
						createMerging(xmlView,annot,parent,begin,end,false);
						
					//cas 1.c.2 : même offsets, annotations différentes mais dans même hierarchie
					}else if (ts.subsumes(ts.getParent(annot.getType()) , ts.getParent(previousAnnot.getType())) &&
							 !ts.getParent(annot.getType()).getShortName().equals("NamedEntity")){
						//créer nouvelle annotation
						parent = (NamedEntity) cas_init.createAnnotation(ts.getParent(annot.getType()), begin, end);
						parent.setSource(annot.getSource());
						NEtoRemove.add(annot);
						NEtoAdd.add(parent);
						removeInPreviousAnnot(annot,begin,end);
	
						createMerging(xmlView,previousAnnot,parent,begin,end,false);
	
					//cas 1.d : même offsets, annotations différentes (cas de conflit). Suite du traitement des conflits ci-dessous
					}else{
						beginOfPreviousAnnot.get(begin).addElement(annot);
						endOfPreviousAnnot.get(end).addElement(annot);
					}
				}
			}else{
				if (beginOfPreviousAnnot.containsKey(begin)){
					beginOfPreviousAnnot.get(begin).addElement(annot);
				}else{
					beginOfPreviousAnnot.put(begin, new Vector<NamedEntity>());
					beginOfPreviousAnnot.get(begin).addElement(annot);
				}
				if (endOfPreviousAnnot.containsKey(end)){
					endOfPreviousAnnot.get(end).addElement(annot);
				}else{
					endOfPreviousAnnot.put(end, new Vector<NamedEntity>());
					endOfPreviousAnnot.get(end).addElement(annot);
				}
			}
		}
		if (cleanRedundantNamedEntity){
			for (NamedEntity ne:NEtoAdd){
				ne.addToIndexes(xmlView);
			}
			for (NamedEntity ne:NEtoRemove){
				ne.removeFromIndexes(xmlView);
			}
			
			
		}
		
		
		
		//ETAPE 2 : traitement des conflits et overlaps
		NEtoAdd.removeAllElements();
		NEtoRemove.removeAllElements();
		beginOfPreviousAnnot.clear();
		endOfPreviousAnnot.clear();
		allAnnotIterator = xmlView.getAnnotationIndex().iterator();
		begin = 0;
		end = 0;
		
		
		while (allAnnotIterator.hasNext()) {
			Annotation annot_init = (Annotation) allAnnotIterator.next();
			if (!annot_init.getType().getName().startsWith("infomagic.types.")){
				continue;
			}
			NamedEntity annot = (NamedEntity) annot_init;
			begin = annot.getBegin();
			end = annot.getEnd();
			if (beginOfPreviousAnnot.containsKey(begin)){
				beginOfPreviousAnnot.get(begin).addElement(annot);
			}else{
				beginOfPreviousAnnot.put(begin, new Vector<NamedEntity>());
				beginOfPreviousAnnot.get(begin).addElement(annot);
			}
			if (endOfPreviousAnnot.containsKey(end)){
				endOfPreviousAnnot.get(end).addElement(annot);
			}else{
				endOfPreviousAnnot.put(end, new Vector<NamedEntity>());
				endOfPreviousAnnot.get(end).addElement(annot);
			}

		}
		
	
		
		allAnnotIterator = xmlView.getAnnotationIndex().iterator();
		begin = 0;
		end = 0;
		while (allAnnotIterator.hasNext()) {
			Annotation annot_init = (Annotation) allAnnotIterator.next();
			if (!annot_init.getType().getName().startsWith("infomagic.types.")){
				continue;
			}
			NamedEntity annot = (NamedEntity) annot_init;
			if (begin == annot.getBegin() && end == annot.getEnd()){
//				System.out.println("bizarre : "+annot.getType().getShortName());
//				System.out.println("bizarre : "+annot.getCanonicalString());
//				System.out.println("bizarre : "+annot.getBegin());
//				System.out.println("bizarre : "+annot.getEnd());
//				System.out.println("attention doublons");
				continue;
			}
			begin = annot.getBegin();
			end = annot.getEnd();
			Vector<NamedEntity> listPreviousAnnot = getPreviousAnnot(begin,end); 
			if (listPreviousAnnot == null){
//				System.out.println("bizarre : "+annot.getType().getShortName());
//				System.out.println("bizarre : "+annot.getBegin());
//				System.out.println("bizarre : "+annot.getEnd());
//				System.out.println("bizarre : "+annot.getEnd());
				continue;
			}
			
			//cas 1.d : même offsets, annotations différentes (cas de conflit)
			if (listPreviousAnnot != null && listPreviousAnnot.size() > 1){
				NamedEntity[] neArray = new NamedEntity[listPreviousAnnot.size()];
				listPreviousAnnot.toArray(neArray);
				createConflict(xmlView, neArray,begin,end);
			}
			
			listPreviousAnnot = null;
			for (Integer oldBegin : beginOfPreviousAnnot.keySet()){
				if (oldBegin >= begin && oldBegin <= end){
					listPreviousAnnot = (Vector<NamedEntity>) beginOfPreviousAnnot.get(oldBegin).clone();
					if (listPreviousAnnot != null){
						Iterator<NamedEntity> i = listPreviousAnnot.iterator();
						while (i.hasNext()) {
							NamedEntity previousAnnot = (NamedEntity) i.next();
							if (!(previousAnnot.getBegin() == begin && previousAnnot.getEnd() == end)){
								if (previousAnnot.getType().equals(annot.getType())){

									//cas 2.a : previousAnnot inclus dans annot, même annotation
									if (previousAnnot.getEnd() <= end){
										createMerging(xmlView,previousAnnot,annot,begin,end,true);

									//cas 3.a : chevauchement, même annotation
									}else if(previousAnnot.getBegin() == begin){
										createMerging(xmlView,annot,previousAnnot,begin,previousAnnot.getEnd(),true);
									}else if(previousAnnot.getBegin() > begin){
										createMerging(xmlView,previousAnnot,annot,begin,previousAnnot.getEnd(),true);

									//cas 2.a : annot inclus dans previousAnnot, même annotation
//									}else if(previousAnnot.getBegin() == begin && previousAnnot.getEnd() > end){
//										createMerging(xmlView,annot,previousAnnot,begin,previousAnnot.getEnd(),true);
									}
									
									//cas 2.b : previousAnnot inclus dans annot, le type de previousAnnot est un sous type de annot ou l'inverse
									//cas 3.b.1 : chevauchement, le type de previousAnnot est un sous type de annot ou l'inverse
								//cas 2.c : annot inclus dans previousAnnot, annotations différentes
								//creation d'une annotation type Overlap
								}else{
									if (previousAnnot.getEnd() <= end){
										createOverlap(xmlView,previousAnnot,annot,begin,end);
										NEtoRemove.add(previousAnnot);
//									}else if(previousAnnot.getBegin() == begin){
//										createOverlap(xmlView,annot,previousAnnot,begin,previousAnnot.getEnd());
//										NEtoRemove.add(annot);
									}else if(previousAnnot.getBegin() > begin){
										createOverlap(xmlView,previousAnnot,annot,begin,previousAnnot.getEnd());
									}
								}
							}
						}
					}
				}
			}
		}
			
		if (cleanRedundantNamedEntity){
			for (NamedEntity ne:NEtoAdd){
				ne.addToIndexes(xmlView);
			}
			for (NamedEntity ne:NEtoRemove){
				ne.removeFromIndexes(xmlView);
			}
			
		}
			
			
		if (displayConflicts){
			for (Fusion f:fusionToAdd){
					f.addToIndexes();
			}
		}
	}
			
	private void createMerging(JCas cas, NamedEntity annotToDelete, NamedEntity annotToKeep, int begin, int end, boolean isOverlaping){
		Merging newFusion = new Merging(cas);
		
		//supprimer les deux annotations (si cleaning = true)
		NEtoRemove.add(annotToDelete);
		NEtoRemove.add(annotToKeep);
		
		//créer nouvelle annotation
		NamedEntity clone = (NamedEntity) annotToKeep.clone();
		clone.setBegin(begin);
		clone.setEnd(end);
		clone.setSource(clone.getSource() + ";" + annotToDelete.getSource());
		
		attributeMerging(clone, annotToDelete);
		
		removeInPreviousAnnot(annotToDelete,annotToDelete.getBegin(),annotToDelete.getEnd());
		boolean add = true;
		for (NamedEntity ne:NEtoAdd){
			if(	isOverlaping &&
				ne.getType().equals(clone.getType()) &&
				ne.getBegin() == clone.getBegin() &&
				ne.getEnd() == clone.getEnd()){
				add = false;
				break;
			}
		}
		if (!add){
			removeInPreviousAnnot(annotToKeep,annotToKeep.getBegin(),annotToKeep.getEnd());
		}else{
			NEtoAdd.add(clone);
			//remplacer dans l'historique l'annotation initiale par la nouvelle annotation mergée
			replaceInPreviousAnnot(annotToKeep,clone,clone.getBegin(),clone.getEnd());
			FSArray fsa = new FSArray(cas,2);
			newFusion.setNeList(fsa);
			newFusion.setNeList(0, annotToDelete);
			newFusion.setNeList(1, annotToKeep);
			newFusion.setBegin(begin);
			newFusion.setEnd(end);
			fusionToAdd.add(newFusion);
		}
	}
	
	private void createConflict(JCas cas, NamedEntity[] listAnnot, int begin, int end){
		//selection de l'annotation majoritaire si elle existe
		NamedEntity electedEntity = majoritaryVoteSelection(listAnnot);
		//possibilité de suppression des annotations conflictuelles
		if (deleteConflictualAnnotations){
			for (NamedEntity ne : listAnnot){
				if(!majoritaryVoteForConflict || electedEntity == null || !ne.equals(electedEntity)){
					NEtoRemove.add(ne);
				}
			}
		}
		Conflict newFusion = new Conflict(cas);
		FSArray fsa = new FSArray(cas,listAnnot.length);
		fsa.copyFromArray(listAnnot, 0, 0, listAnnot.length);
		newFusion.setNeList(fsa);
		newFusion.setBegin(begin);
		newFusion.setEnd(end);
		fusionToAdd.add(newFusion);

	}
	
	private void createOverlap(JCas cas, NamedEntity previousAnnot, NamedEntity annot, int begin, int end){
		boolean add = true;
		for (Fusion fusion:fusionToAdd){
			if(	fusion.getType().getShortName().equals("Overlap") &&
					fusion.getBegin() == begin &&
					fusion.getEnd() == end){
				FSArray fsa = fusion.getNeList();
				int i;
				for(i = 0;i<fsa.size();i++){
					if (fsa.get(i) == null){
						fsa.set(i, previousAnnot);
						fsa.set(i+1, annot);
						break;
					}else if(fsa.get(i).equals(previousAnnot)){
						previousAnnot = annot;
						annot = null;
					}else if(fsa.get(i).equals(annot)){
						annot = null;
					}
				}
				fusion.setNeList(fsa);
				add = false;
				break;
			}
		}
		
		if (add){
			Overlap newFusion = new Overlap(cas);
			FSArray fsa = new FSArray(cas,10);
			newFusion.setNeList(fsa);
			newFusion.setNeList(0, previousAnnot);
			newFusion.setNeList(1, annot);
			newFusion.setBegin(begin);
			newFusion.setEnd(end);
			fusionToAdd.add(newFusion);
		}
	}


	private void replaceInPreviousAnnot(NamedEntity annotToDelete,NamedEntity annotToKeep, int begin, int end){
		beginOfPreviousAnnot.get(begin).remove(annotToDelete);
		beginOfPreviousAnnot.get(begin).addElement(annotToKeep);
		endOfPreviousAnnot.get(end).remove(annotToDelete);
		endOfPreviousAnnot.get(end).addElement(annotToKeep);
	}

	private void removeInPreviousAnnot(NamedEntity annotToDelete, int begin, int end){
		beginOfPreviousAnnot.get(begin).remove(annotToDelete);
		endOfPreviousAnnot.get(end).remove(annotToDelete);
	}
	
	
	@SuppressWarnings("unchecked")
	private Vector<NamedEntity> getPreviousAnnot(int begin, int end){
		if (beginOfPreviousAnnot.containsKey(begin) && endOfPreviousAnnot.containsKey(end)){
			Vector<NamedEntity> v1 = (Vector<NamedEntity>) beginOfPreviousAnnot.get(begin).clone();
			Vector<NamedEntity> v2 = endOfPreviousAnnot.get(end);
			v1.retainAll(v2);
			if (!v1.isEmpty()){
				return v1;
			}
		}
		return null;
	}

	
	
	private boolean isInTextBlock(int begin, int end, Hashtable<Integer,Integer> TextBlocks){
		Enumeration<Integer> e = TextBlocks.keys();
		while (e.hasMoreElements()) {
			int beginTextBlock = (Integer) e.nextElement();
			if (beginTextBlock<= begin && TextBlocks.get(beginTextBlock) >= end){
				return true;
			}
		}
		return false;
	}
	
	private NamedEntity majoritaryVoteSelection(NamedEntity[] listAnnot){
		int[] scores = new int[listAnnot.length]; 
		int i = 0;
		float sum = 0;
		for (NamedEntity ne : listAnnot){
			scores[i] = ne.getSource().split(";").length;
			sum += scores[i];
			i++;
		}
		for (int j = 0 ; j<scores.length ; j++){
			if (scores[j]/sum > 0.5){
//				System.out.println(listAnnot[j].getType().getShortName()+"\t"+listAnnot[j].getCoveredText()+"\t("+listAnnot[j].getBegin()+";"+listAnnot[j].getEnd()+")");
				return listAnnot[j];
			}
		}
		return null;
	}
	
	private void preprocessing(JCas cas, JCas xmlView){
		TypeSystem ts = cas.getTypeSystem();
		
		//beginOfPreviousAnnot et endOfPreviousAnnot contiennent les annotations qui vont être comparées avec l'annotation courant (annot)
		beginOfPreviousAnnot = new Hashtable<Integer,Vector<NamedEntity>>();
		endOfPreviousAnnot = new Hashtable<Integer,Vector<NamedEntity>>();
		
		//contient les annotations de type NamedEntity (ou un des soustypes sauf Fusion)
		//à ajouter ou supprimer, affichable ou non (cleanRedundantNamedEntity true or false)
		NEtoAdd = new Vector<NamedEntity>();
		NEtoRemove = new Vector<NamedEntity>();
		int begin = 0;
		int end = 0;
		FSIterator allAnnotIterator = xmlView.getAnnotationIndex().iterator();

		
		//ETAPE 1 : traitement des mergings
		while (allAnnotIterator.hasNext()) {
			Annotation annot_init = (Annotation) allAnnotIterator.next();
			if (!annot_init.getType().getName().startsWith("infomagic.types.")){
				continue;
			}
			NamedEntity annot = (NamedEntity) annot_init;
			begin = annot.getBegin();
			end = annot.getEnd();
			
			
			//simplification des noms de source
			if (annot.getSource().matches(".*arisem.*")){
				annot.setSource("Arisem");
			}else if(annot.getSource().equals("com.xerox.xrce")){
				annot.setSource("Xerox");
			}
			
			//suppression temporaire (pour la demo) de la NormDate de IGM
			if (annot.getSource().equals("IGM") &&
				annot.getType().getShortName().equals("Date")){
				Date date = (Date) annot;
				date.setDateNormalizedForm("");
				annot = (NamedEntity) date;
			}
			
			
			
			
			
			//ajout, si necessaire du canonical string
			annot.setCanonicalString(annot.getCoveredText());
			
			Vector<NamedEntity> listPreviousAnnot = getPreviousAnnot(begin,end); 
			
			if (listPreviousAnnot != null){
				Iterator<NamedEntity> i = listPreviousAnnot.iterator();
				while (i.hasNext()) {
					NamedEntity previousAnnot = (NamedEntity) i.next();
					
					//cas special double annotation (DateAndTime & NumericalExpression) d'Arisem  
					if (annot.getType().getShortName().equals("NumericalExpression") &&
							  ts.subsumes(ts.getType("infomagic.types.TemporalExpression"), previousAnnot.getType()) &&
							  annot.getSource().equals("Arisem") &&
							  previousAnnot.getSource().equals("Arisem")){
						NEtoRemove.add(annot);
	
					//cas special double annotation (DateAndTime & NumericalExpression) d'Arisem 
					}else if (ts.subsumes(ts.getType("infomagic.types.TemporalExpression"), annot.getType()) &&
							  previousAnnot.getType().getShortName().equals("NumericalExpression") &&
							  annot.getSource().equals("Arisem") &&
							  previousAnnot.getSource().equals("Arisem")){
						NEtoRemove.add(previousAnnot);
						replaceInPreviousAnnot(previousAnnot,annot,begin,end);
	
					//cas de double annotation par le même annotateur 
					}else if (previousAnnot.getType().equals(annot.getType()) &&
							  annot.getSource().equals(previousAnnot.getSource())){
						NEtoRemove.add(annot);
	
					//cas 1.b.1 : même offsets, le type de previousAnnot est un sous type de annot
					}else if (ts.subsumes(previousAnnot.getType(), annot.getType()) &&
							  annot.getSource().equals(previousAnnot.getSource())){
						NEtoRemove.add(previousAnnot);
						
					//cas 1.b.2 : même offsets, le type de annot est un sous type de previousAnnot
					}else if (ts.subsumes(annot.getType(), previousAnnot.getType()) &&
							  annot.getSource().equals(previousAnnot.getSource())){
						NEtoRemove.add(annot);
						
					//cas 1.c.1 : même offsets, annotations différentes mais dans même hierarchie
					}else if (ts.subsumes(ts.getParent(previousAnnot.getType()), ts.getParent(annot.getType())) &&
							 !ts.getParent(previousAnnot.getType()).getShortName().equals("NamedEntity") &&
							  annot.getSource().equals(previousAnnot.getSource())){
						NEtoRemove.add(previousAnnot);
						NEtoRemove.add(annot);
						
					//cas 1.c.2 : même offsets, annotations différentes mais dans même hierarchie
					}else if (ts.subsumes(ts.getParent(annot.getType()) , ts.getParent(previousAnnot.getType())) &&
							 !ts.getParent(annot.getType()).getShortName().equals("NamedEntity") &&
							  annot.getSource().equals(previousAnnot.getSource())){
						NEtoRemove.add(previousAnnot);
						NEtoRemove.add(annot);
	
					}else if (annot.getSource().equals(previousAnnot.getSource())){
						NEtoRemove.add(previousAnnot);
						NEtoRemove.add(annot);
	
					//cas 1.d : même offsets, annotations différentes (cas de conflit). Suite du traitement des conflits ci-dessous
					}else{
						beginOfPreviousAnnot.get(begin).addElement(annot);
						endOfPreviousAnnot.get(end).addElement(annot);
					}
				}
			}else{
				if (beginOfPreviousAnnot.containsKey(begin)){
					beginOfPreviousAnnot.get(begin).addElement(annot);
				}else{
					beginOfPreviousAnnot.put(begin, new Vector<NamedEntity>());
					beginOfPreviousAnnot.get(begin).addElement(annot);
				}
				if (endOfPreviousAnnot.containsKey(end)){
					endOfPreviousAnnot.get(end).addElement(annot);
				}else{
					endOfPreviousAnnot.put(end, new Vector<NamedEntity>());
					endOfPreviousAnnot.get(end).addElement(annot);
				}
			}
		}
		if (cleanRedundantNamedEntity){
			for (NamedEntity ne:NEtoAdd){
				ne.addToIndexes(cas);
			}
			for (NamedEntity ne:NEtoRemove){
				ne.removeFromIndexes(cas);
			}
		}
		
	}
	
	
	private void attributeMerging(NamedEntity clone, NamedEntity annotToDelete){
		String typeName = clone.getType().getShortName();

		Vector<Feature> attributeList = new Vector<Feature>();
		if (typeName.equals("Person")){
			attributeList.addElement(clone.getType().getFeatureByBaseName("firstname"));
			attributeList.addElement(clone.getType().getFeatureByBaseName("middlename"));
			attributeList.addElement(clone.getType().getFeatureByBaseName("lastname"));
			attributeList.addElement(clone.getType().getFeatureByBaseName("function"));
			attributeList.addElement(clone.getType().getFeatureByBaseName("ethnonym"));
			attributeList.addElement(clone.getType().getFeatureByBaseName("title"));
//		}else if(typeName.equals("Event")){
//			attributeList.addElement(clone.getType().getFeatureByBaseName("date"));
//			attributeList.addElement(clone.getType().getFeatureByBaseName("location"));
		}else if(typeName.equals("Date")){
			attributeList.addElement(clone.getType().getFeatureByBaseName("DateNormalizedForm"));
		}else if(typeName.equals("NumericalExpression")){
			attributeList.addElement(clone.getType().getFeatureByBaseName("value"));
			attributeList.addElement(clone.getType().getFeatureByBaseName("SI"));
			attributeList.addElement(clone.getType().getFeatureByBaseName("numex"));
			attributeList.addElement(clone.getType().getFeatureByBaseName("refnumex"));
		}
		for (Feature f : attributeList){
			if (clone.getFeatureValueAsString(f) != null && clone.getFeatureValueAsString(f).equals("com.arisem.uima2.annotator.HstAnnotator")){
				clone.setFeatureValueFromString(f, "");
			}
//			if (annotToDelete.getFeatureValueAsString(f) != null && annotToDelete.getFeatureValueAsString(f).equals("com.arisem.uima2.annotator.HstAnnotator")){
//				annotToDelete.setFeatureValueFromString(f, "");
//			}
			if ((clone.getFeatureValueAsString(f) == null ||
				clone.getFeatureValueAsString(f).equals("")) &&
				annotToDelete.getFeatureValueAsString(f) != null &&
				!annotToDelete.getFeatureValueAsString(f).equals("") &&
				!annotToDelete.getFeatureValueAsString(f).equals("com.arisem.uima2.annotator.HstAnnotator")){
				clone.setFeatureValueFromString(f, annotToDelete.getFeatureValueAsString(f));
			}else if (clone.getFeatureValueAsString(f) != null &&
					!clone.getFeatureValueAsString(f).equals("") &&
				annotToDelete.getFeatureValueAsString(f) != null &&
				!annotToDelete.getFeatureValueAsString(f).equals("") &&
				!clone.getFeatureValueAsString(f).equalsIgnoreCase(annotToDelete.getFeatureValueAsString(f)) &&
				!annotToDelete.getFeatureValueAsString(f).equals("com.arisem.uima2.annotator.HstAnnotator")){
				String cloneFeat = clone.getFeatureValueAsString(f);
				String toDeleteFeat = annotToDelete.getFeatureValueAsString(f);
				//TODO remplacer matches par autre chose
				if (toDeleteFeat.matches(".*"+cloneFeat+".*")){
					clone.setFeatureValueFromString(f, annotToDelete.getFeatureValueAsString(f));
				}else if(!cloneFeat.matches(".*"+toDeleteFeat+".*")){
					System.out.println ("pas meme attrib");
					System.out.println ("attrib1 : \t"+clone.getFeatureValueAsString(f));
					System.out.println ("source1 : \t"+clone.getSource());
					System.out.println ("attrib2 : \t"+annotToDelete.getFeatureValueAsString(f));
					System.out.println ("source2 : \t"+annotToDelete.getSource());
					System.out.println ("debut : \t"+annotToDelete.getBegin());
					clone.setFeatureValueFromString(f, "");
				}
			}
//			if (clone.getFeatureValueAsString(f) != null ){
//				System.out.println ("seul attrib1 : \t"+clone.getFeatureValueAsString(f));
//			}
//			if (annotToDelete.getFeatureValueAsString(f) != null ){
//				System.out.println ("seul attrib2 : \t"+annotToDelete.getFeatureValueAsString(f));
//			}
		}
		

	}
	

}
package com.xerox.xrce.uimaxip.infomagic;

import infomagic.types.CityEntity;
import infomagic.types.CompanyEntity;
import infomagic.types.CountryEntity;
import infomagic.types.Entity;
import infomagic.types.EntityMention;
import infomagic.types.LocationEntity;
import infomagic.types.OrganisationEntity;
import infomagic.types.PersonEntity;

import java.lang.reflect.InvocationTargetException;
import java.util.Vector;

import jcoref.Referent;
import jcoref.ReferentNode;
import jcoref.ReferentNodeList;

import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.cas.FSArray;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;

public class InfomagicXipCorefEnglish_alternate_coref_representation extends InfomagicXipEnglish {
		
	{ doCoref = true; }

	@Override
	protected void processReferent(Referent r, JCas cas, int offset) {
		try {
			Class<?> entityType = getEntityType(r);
			if (entityType == null) {
				// For the time being, ignore anything that's not a named entity.
				return;
			}
			Entity entity = (Entity) entityType.getConstructor(JCas.class).newInstance(cas);
			ReferentNodeList refNodes = r.getReferentNodes();
			FSArray mentions = new FSArray(cas, refNodes.size());
			entity.setMentions(mentions);
			for (int i = 0; i < refNodes.size(); i++) {
				XipNode xipNode = refNodes.elementAt(i).getNode();
				EntityMention mention =	new EntityMention(cas,
														  (int) xipNode.getLeftChar()+offset,
														  (int) xipNode.getRightChar()+offset);
				mention.setEntity(entity);
				mentions.set(i, mention);
				mention.addToIndexes(cas);
				if (entityType == PersonEntity.class) {
					PersonEntity person = (PersonEntity) entity;
					for (XipDependency dep : xipNode.getDependencies()) {
						String depName = dep.getName();
						// todo:
						// If the same feature is provided for multiple coreferent nodes,
						// e.g. firstname = G. and firstname = George, currently the last one in the
						// list arbitrarily wins.  To be improved.
						// Using surface instead of lemma because they're probably usually the same,
						// but if a name is also a common noun (e.g. Bush) then the lemma might be in
						// lowercase, which is undesired.
						if (depName.equals("INTERNALS_FIRSTNAME1")) {
							person.setFirstname(dep.getArg(2).getSurface());
						} else if (depName.equals("INTERNALS_FIRSTNAME2")) {
							person.setMiddlename(dep.getArg(2).getSurface());
						} else if (depName.equals("INTERNALS_LASTNAME")) {
							person.setLastname(dep.getArg(2).getSurface());
						} else if (depName.equals("INTERNALS_TITLE")) {
							person.setTitle(dep.getArg(2).getSurface());
						}
					}
				}
			}

		// These are exceptions that can be thrown by the reflection methods.  The only
		// way they can happen is if the code and the jcasgen classes are out of sync.
		} catch (IllegalArgumentException e) {
			throw new AssertionError(e);
		} catch (SecurityException e) {
			throw new AssertionError(e);
		} catch (InstantiationException e) {
			throw new AssertionError(e);
		} catch (IllegalAccessException e) {
			throw new AssertionError(e);
		} catch (InvocationTargetException e) {
			throw new AssertionError(e);
		} catch (NoSuchMethodException e) {
			throw new AssertionError(e);
		}			
	}
	
	private Class<?> getEntityType(Referent r) {
		for (ReferentNode refNode : r.getReferentNodes()) {
			Class<?> result = getEntityType(refNode.getNode());
			if (result != null) {
				return result;
			}
		}
		return null;
	}

	private Class<?> getEntityType(XipNode node) {
		Class<?> result = null;

		if (result == null) {
			Vector<XipDependency> deps = node.getDependenciesByName("LOCORG");
			if (! deps.isEmpty()) {
				XipDependency dep = deps.firstElement();
				if (dep.hasFeatures("CITY")) {
					result = CityEntity.class;
				} else if (dep.hasFeatures("COUNTRY")) {
					result = CountryEntity.class;
				} else {
					result = LocationEntity.class;
				}
			}
		}
		
		if (result == null) {
			Vector<XipDependency> deps = node.getDependenciesByName("PERSON");
			if (! deps.isEmpty()) {
				result = PersonEntity.class;
			}
		}

		if (result == null) {
			Vector<XipDependency> deps = node.getDependenciesByName("ORGANISATION");
			if (! deps.isEmpty()) {
				XipDependency dep = deps.firstElement();
				if (dep.hasFeatures("COMP")) {
					result = CompanyEntity.class;
				} else {
					result = OrganisationEntity.class;
				}
			}
		}

		return result;
	}
	
	@Override
	protected void processDependency(XipDependency dep, JCas cas, int offset) 
	  throws AnalysisEngineProcessException {
		// do nothing; suppresses the named entity recognition from InfomagicXipEnglish
	}
}

package com.xerox.xrce.uimaxip.infomagic;

import infomagic.types.City;
import infomagic.types.Company;
import infomagic.types.Country;
import infomagic.types.Location;
import infomagic.types.NamedEntity;
import infomagic.types.Organisation;
import infomagic.types.Person;

import java.lang.reflect.InvocationTargetException;
import java.util.Vector;

import jcoref.Referent;
import jcoref.ReferentNode;
import jcoref.ReferentNodeList;

import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.jcas.JCas;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;

public class InfomagicXipCorefEnglish extends InfomagicXipEnglish {
	
    { doCoref = true; }
	
	@Override
	protected void processReferent(Referent r, JCas cas, int offset) {
		try {

			ReferentNodeList refNodes = r.getReferentNodes();

			// Make three passes through the node list, as commented below.


			// Pass 1: Look at nodes until we find one that has been labeled as a named entity, in order
			// to determine the type.  (Is it possible for different coreferent nodes to have
			// different types?  For now just take the first.)

			Class<?> entityType = null;
			for (ReferentNode refNode : refNodes) {
				entityType = getEntityType(refNode.getNode());
				if (entityType != null) {
					break;
				}
			}
			if (entityType == null) {
				// Ignore anything that's not a named entity.
				return;
			}

			// Pass 2: Collect type-specific information from all the nodes
			NamedEntity annotTemplate = (NamedEntity) entityType.getConstructor(JCas.class).newInstance(cas);
			for (ReferentNode n : refNodes) {
				XipNode xipNode = n.getNode();
				// Take the first mention as the canonical form.  todo: this is wrong in the case of
				// cataphora.  But cataphora isn't working properly right now (bug in jcoref?), so I'm
				// putting off handling it until I can test.
				if (annotTemplate.getAntecedent() == null) {
					annotTemplate.setAntecedent(xipNode.getSurface());
				}
				if (entityType == Person.class) {
					Person person = (Person) annotTemplate;
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
			
			// Pass 3: Annotate each node with information accumulated from all nodes.
			for (ReferentNode n : refNodes) {
				XipNode xipNode = n.getNode();
				NamedEntity annot = (NamedEntity) annotTemplate.clone();
				annot.setBegin((int) xipNode.getLeftChar() + offset);
				annot.setEnd((int) xipNode.getRightChar() + offset);
				annot.addToIndexes(cas);
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
	
	private Class<?> getEntityType(XipNode node) {
		Class<?> result = null;

		if (result == null) {
			Vector<XipDependency> deps = node.getDependenciesByName("LOCORG");
			if (! deps.isEmpty()) {
				XipDependency dep = deps.firstElement();
				if (dep.hasFeatures("CITY")) {
					result = City.class;
				} else if (dep.hasFeatures("COUNTRY")) {
					result = Country.class;
				} else {
					result = Location.class;
				}
			}
		}
		
		if (result == null) {
			Vector<XipDependency> deps = node.getDependenciesByName("PERSON");
			if (! deps.isEmpty()) {
				result = Person.class;
			}
		}

		if (result == null) {
			Vector<XipDependency> deps = node.getDependenciesByName("ORGANISATION");
			if (! deps.isEmpty()) {
				XipDependency dep = deps.firstElement();
				if (dep.hasFeatures("COMP")) {
					result = Company.class;
				} else {
					result = Organisation.class;
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

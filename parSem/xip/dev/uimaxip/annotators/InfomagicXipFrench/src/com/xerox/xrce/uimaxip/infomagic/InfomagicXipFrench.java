package com.xerox.xrce.uimaxip.infomagic;

import infomagic.types.Address;
import infomagic.types.Age;
import infomagic.types.ArchitectureEvent;
import infomagic.types.Area;
import infomagic.types.Award;
import infomagic.types.Building;
import infomagic.types.CinemaEvent;
import infomagic.types.City;
import infomagic.types.ComicBookEvent;
import infomagic.types.Company;
import infomagic.types.Continent;
import infomagic.types.Country;
import infomagic.types.CulturalEvent;
import infomagic.types.DanceEvent;
import infomagic.types.Date;
import infomagic.types.District;
import infomagic.types.Duration;
import infomagic.types.Email;
import infomagic.types.Event;
import infomagic.types.GameEvent;
import infomagic.types.Hydronym;
import infomagic.types.Interval;
import infomagic.types.Length;
import infomagic.types.LiteratureEvent;
import infomagic.types.Location;
import infomagic.types.MilitaryEvent;
import infomagic.types.Money;
import infomagic.types.MusicEvent;
import infomagic.types.MusicOrganisation;
import infomagic.types.NamedEntity;
import infomagic.types.NumericalExpression;
import infomagic.types.Organisation;
import infomagic.types.Oronym;
import infomagic.types.PaintingEvent;
import infomagic.types.Percent;
import infomagic.types.Person;
import infomagic.types.PhotographyEvent;
import infomagic.types.PoliticalOrganisation;
import infomagic.types.Product;
import infomagic.types.Region;
import infomagic.types.SculptureEvent;
import infomagic.types.Set;
import infomagic.types.Speed;
import infomagic.types.SportEvent;
import infomagic.types.SportOrganisation;
import infomagic.types.TelFax;
import infomagic.types.Temperature;
import infomagic.types.TemporalExpression;
import infomagic.types.TheatreEvent;
import infomagic.types.Time;
import infomagic.types.URL;
import infomagic.types.Unknown;
import infomagic.types.Vehicle;
import infomagic.types.Volume;
import infomagic.types.Weight;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;

import org.apache.uima.UimaContext;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.analysis_engine.ResultSpecification;
import org.apache.uima.analysis_engine.TypeOrFeature;
import org.apache.uima.cas.CASException;
import org.apache.uima.cas.CASRuntimeException;
import org.apache.uima.jcas.JCas;
import org.apache.uima.resource.ResourceInitializationException;
import org.apache.uima.util.Level;
import org.apache.uima.util.Logger;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;

public class InfomagicXipFrench extends InfomagicXipFrenchMorph {
		
	private xipNode2EntityMap xipNode2Entity = new xipNode2EntityMap();
	private boolean generateTokens = false;

	// Temporary workaround for UIMA bug https://issues.apache.org/jira/browse/UIMA-1107
	@Override
	public void process(JCas cas) throws AnalysisEngineProcessException {
		Logger logger = getContext().getLogger();
		try {
			JCas newCas = cas.getView("txt");
			cas = newCas;
		} catch (CASException e) {
			logger.log(Level.WARNING, "No txt view, using _InitialView");
		} catch (CASRuntimeException e) {
			logger.log(Level.WARNING, "No txt view, using _InitialView");
		}
		super.process(cas);
	}
	

	
	
	@Override
	protected void processXipUnit(XipUnit unit, JCas cas, int offset) 
		throws AnalysisEngineProcessException {

		Logger logger = getContext().getLogger();

		xipNode2Entity.clear();
		if (generateTokens) {
			for (XipNode xipToken : unit.getLeafNodes()) {
				processToken(xipToken, cas, offset);		
			}
		}
		for (XipDependency dep : unit.getDependencies()) {
			processNEDependency(dep,cas,offset);
		}
		for (XipDependency dep : unit.getDependencies()) {
			processPersonFeatureDependency(dep);
		}
		for (XipDependency dep : unit.getDependenciesByName("DATE-EVENEMENT")) {
			processDateEventDependency(dep,cas);
		}
		for (XipDependency dep : unit.getDependenciesByName("LIEU-EVENEMENT")) {
			processLocationEventDependency(dep,cas);
		}
		
		// I used to filter out conflicting annotations (e.g. PERSON and CITY
		// for "Saint Jean", but I no longer know why I did that.  If I figure
		// out why, put a note here!
		for (List<NamedEntity> nodeAnnots : xipNode2Entity.values()) {
			for (NamedEntity annot : nodeAnnots) {
				annot.addToIndexes();
			}
		}
	}

	private void processLocationEventDependency(XipDependency dep, JCas cas) {
		Logger logger = getContext().getLogger();
		Vector<XipNode> params = dep.getParameters();
		if (params.size() != 2) {
			logger.log(Level.WARNING, "Dependency " + dep.getName() + " had " +
					params.size() + " args instead of 2");
			return;
		}
		XipNode arg1 = params.get(0);
		Event event = (Event) xipNode2Entity.get(arg1, Event.class);
		if (event == null) {
			logger.log(Level.WARNING, "first argument (" + arg1.getSurface() +
					") of " + dep.getName() + " is not an Event");
			return;
		}
		XipNode arg2 = params.get(1);
		Location location = (Location) xipNode2Entity.get(arg2, Location.class);
		//temporary addings
//		if (location == null) {
//	        location = new Location(cas);
//	        location.setSource("com.xerox.xrce");
//	        location.setCanonicalString(params.get(1).getLemma());
//		}
		if (location == null) {
			logger.log(Level.WARNING, "second argument (" + arg2.getSurface() +
					") of " + dep.getName() + " is not a Location");
			return;
		}
		event.setLocation(location);
	}

	private void processDateEventDependency(XipDependency dep, JCas cas) {
		Logger logger = getContext().getLogger();
		Vector<XipNode> params = dep.getParameters();
		if (params.size() != 2) {
			logger.log(Level.WARNING, "Dependency " + dep.getName() + " had " +
					params.size() + " args instead of 2");
			return;
		}
		Event event = (Event) xipNode2Entity.get(params.get(0), Event.class);
		if (event == null) {
			logger.log(Level.WARNING, "first argument (" + params.get(0).getSurface() + ") of " + dep.getName() + " is not an Event");
			return;
		}
		Date date = (Date) xipNode2Entity.get(params.get(1), Date.class);
		
		//temporary addings
//		if (date == null) {
//			DateNormalization dateNorm = new DateNormalization();
//	        String form = dateNorm.normalizeDate(dep.getArg(2));
//	        date = new Date(cas);
//			date.setSource("com.xerox.xrce");
//			date.setCanonicalString(params.get(1).getLemma());
//	        date.setDateNormalizedForm(form);
//		}
		
		if (date == null) {
			logger.log(Level.WARNING, "second argument (" + params.get(1).getSurface() + ") of " + dep.getName() + " is not a Date");
			return;
		}
		event.setDate(date);
	}

	private void processPersonFeatureDependency(XipDependency dep) {
		Logger logger = getContext().getLogger();
		String depname = dep.getName();
		if (depname.equals("PRENOM") ||
				depname.equals("PATRONYME") ||
				depname.equals("ETHNONYME-GEO") ||
				depname.equals("CIVILITE") ||
				depname.equals("FONCTION")) {
			XipNode xipNode1 = dep.getArg(1);
			XipNode xipNode2 = dep.getArg(2);
			if (xipNode1 == null || xipNode2 == null) {
				logger.log(Level.WARNING, "Dependency " + depname + " had " +
						dep.getParameters().size() + " args instead of 2");
				return;
			}
			Person person = (Person) xipNode2Entity.get(xipNode2, Person.class);
			if (person == null) {
				logger.log(Level.WARNING, "second argument (" + xipNode2.getSurface() + ") of " + dep.getName() + " is not a Date");
				return;
			}
			if (depname.equals("PRENOM")) {
				person.setFirstname(xipNode1.getLemma());
			} else if (depname.equals("PATRONYME")) {
				person.setLastname(xipNode1.getLemma());
			} else if (depname.equals("FONCTION")) {
				person.setFunction(xipNode1.getLemma());
			} else if (depname.equals("CIVILITE")) {
				person.setTitle(xipNode1.getLemma());
			} else if (depname.equals("ETHNONYME-GEO")) {
				person.setEthnonym(xipNode1.getLemma());
			} else {
				assert false;
			}
		}
	}

	protected void processNEDependency(XipDependency dep, JCas cas, int offset)  throws AnalysisEngineProcessException {
		Logger logger = getContext().getLogger();
		NamedEntity newAnnot = null;
		String depname = dep.getName();
		if (depname.equals("LIEU")) {
			if (dep.hasFeature("VILLE", "*")) {
				newAnnot = new City(cas);
			} else if (dep.hasFeature("PAYS", "*")) {
				newAnnot = new Country(cas);
			} else if (dep.hasFeature("REGION", "*")) {
				newAnnot = new Region(cas);
			} else if (dep.hasFeature("CONTINENT", "*")) {
				newAnnot = new Continent(cas);
			} else if (dep.hasFeature("ADRESSE", "*")) {
				newAnnot = new Address(cas);
			} else if (dep.hasFeature("QUARTIER", "*")) {
				newAnnot = new District(cas);
			} else if (dep.hasFeature("BATIMENT", "*")) {
				newAnnot = new Building(cas);
			} else if (dep.hasFeature("TELFAX", "*")) {
				newAnnot = new TelFax(cas);
			} else if (dep.hasFeature("HYDRONYME", "*")) {
				newAnnot = new Hydronym(cas);
			} else if (dep.hasFeature("ORONYME", "*")) {
				newAnnot = new Oronym(cas);
			} else {
				newAnnot = new Location(cas);
			}
		} else if (depname.equals("EVENEMENT") ||
				   depname.equals("EVENEMENT-TEMP")) {
			if (dep.hasFeature("GUERRE", "*")) {
				newAnnot = new MilitaryEvent(cas);
			} else if (dep.hasFeature("SPORT", "*")) {
				newAnnot = new SportEvent(cas);
			} else if (dep.hasFeature("CULTUREL", "*")) {
				if (dep.hasFeature("BD", "*")) {
					newAnnot = new ComicBookEvent(cas);
				} else if (dep.hasFeature("CINEMA", "*")) {
					newAnnot = new CinemaEvent(cas);
				} else if (dep.hasFeature("DANSE", "*")) {
					newAnnot = new DanceEvent(cas);
				} else if (dep.hasFeature("THEATRE", "*")) {
					newAnnot = new TheatreEvent(cas);
				} else if (dep.hasFeature("MUSIQUE", "*")) {
					newAnnot = new MusicEvent(cas);
				} else if (dep.hasFeature("JEU", "*")) {
					newAnnot = new GameEvent(cas);
				} else if (dep.hasFeature("LITTERATURE", "*")) {
					newAnnot = new LiteratureEvent(cas);
				} else if (dep.hasFeature("PEINTURE", "*")) {
					newAnnot = new PaintingEvent(cas);
				} else if (dep.hasFeature("ARCHITECTURE", "*")) {
					newAnnot = new ArchitectureEvent(cas);
				} else if (dep.hasFeature("SCULPTURE", "*")) {
					newAnnot = new SculptureEvent(cas);
				} else if (dep.hasFeature("PHOTO", "*")) {
					newAnnot = new PhotographyEvent(cas);
				} else {
					newAnnot = new CulturalEvent(cas);
				}
			} else {
				newAnnot = new Event(cas);
			}
		} else if (depname.equals("PERSONNE")) {
			if (dep.hasFeature("ANIMAL", "*")) {
				newAnnot = new Unknown(cas);
			} else {
				newAnnot = new Person(cas);
			}
		} else if (depname.equals("ORG")) {
			if (dep.hasFeature("BATIMENT", "*")) {
				newAnnot = new Building(cas);
			} else if (dep.hasFeature("ENTREPRISE", "*")) {
				newAnnot = new Company(cas);
			} else if (dep.hasFeature("SPORT", "*")) {
				newAnnot = new SportOrganisation(cas);
			} else if (dep.hasFeature("MUSIQUE", "*")) {
				newAnnot = new MusicOrganisation(cas);
			} else if (dep.hasFeature("POL", "*")) {
				newAnnot = new PoliticalOrganisation(cas);
			} else {
				newAnnot = new Organisation(cas);
			}
		} else if (depname.equals("DATECOORD")||
				   depname.equals("DATEMOD")) {
			newAnnot = new Date(cas);
		} else if (depname.equals("DATE")) {
			if (dep.hasFeature("INFOMAGIC", "*")) {
				Date date = new Date(cas);
	           	        DateNormalization dateNorm = new DateNormalization();
	        	        String form = dateNorm.normalizeDate(dep.getArg(1));
				date.setDateNormalizedForm(form);
				newAnnot = (Date) date;
			} else {
				newAnnot = new Date(cas);
			}

//		} else if (depname.equals("DATECOORD") ||
//				   depname.equals("DATEMOD")) {
//				newAnnot = new TemporalExpression(cas);
//				Date date = new Date(cas);
//	            DateNormalization dateNorm = new DateNormalization();
//	            String form = dateNorm.normalizeDate(dep.getArg(1));
//				date.setDateNormalizedForm(form);
//				newAnnot = (Date) date;

		} else if (depname.equals("HEURE")) {
			Time time = new Time(cas);
            //TimeNormalization timeNorm = new TimeNormalization();
            //String form = timeNorm.normalizeTime(dep.getArg(1));
			//time.setTimeNormalizedForm(form);
			newAnnot = (Time) time;
//		} else if (depname.equals("DUREE") ||
//				   depname.equals("DUREEMOD")) {
//			Duration duration = new Duration(cas);
//			DurationNormalization durationNorm = new DurationNormalization();
//          String form = durationNorm.normalizeDuration(dep.getArg(1));
//          duration.setDurationNormalizedForm(form);
//			newAnnot = (Duration) duration;
		} else if (depname.equals("DUREE") ||
				   depname.equals("DUREEMOD")) {
			newAnnot = new TemporalExpression(cas);
		} else if (depname.equals("INTERVAL-TEMP")) {
			newAnnot = new Interval(cas);
		} else if (depname.equals("TEMPEXPR")) {
			newAnnot = new TemporalExpression(cas);
		} else if (depname.equals("SET")) {
			newAnnot = new Set(cas);
//			newAnnot = new TemporalExpression(cas);
		} else if (depname.equals("PRODUIT")) {
			if (dep.hasFeature("VEHICULE", "*")) {
				newAnnot = new Vehicle(cas);
			} else if (dep.hasFeature("AWARD", "*")) {
				newAnnot = new Award(cas);
			} else {
				newAnnot = new Product(cas);
			}
		} else if (depname.equals("AMOUNT")) {
			if (dep.hasFeature("UNIT_LEN", "*")) {
				newAnnot = new Length(cas);
			} else if (dep.hasFeature("UNIT_SURF", "*")) {
				newAnnot = new Area(cas);
			} else if (dep.hasFeature("UNIT_VOL", "*")) {
				newAnnot = new Volume(cas);
			} else if (dep.hasFeature("UNIT_WEIGHT", "*")) {
				newAnnot = new Weight(cas);
			} else if (dep.hasFeature("UNIT_VIT", "*")) {
				newAnnot = new Speed(cas);
			} else if (dep.hasFeature("UNIT_MONNAIE", "*")) {
				newAnnot = new Money(cas);
			} else if (dep.hasFeature("UNIT_TEMPER", "*")) {
				newAnnot = new Temperature(cas);
			} else if (dep.hasFeature("PERCENT", "*")) {
				newAnnot = new Percent(cas);
			} else {
				newAnnot = new NumericalExpression(cas);
			}
		} else if (depname.equals("AGE")) {
			newAnnot = new Age(cas);
		} else if (depname.equals("URL")) {
			newAnnot = new URL(cas);
		} else if (depname.equals("EMAIL")) {
			newAnnot = new Email(cas);
		}
		
		if (newAnnot != null) {
			XipNode n = dep.getArg(1);
			if (n == null) {
				logger.log(Level.WARNING, "Dependency " + dep.getName() + " had " +
						dep.getParameters().size() + " args instead of 1");
				return;
			}
			xipNode2Entity.put(n, newAnnot);
			newAnnot.setSource("com.xerox.xrce");
			newAnnot.setCanonicalString(n.getLemma());
			newAnnot.setBegin((int) n.getLeftChar()+offset);
			newAnnot.setEnd((int) n.getRightChar()+offset);
			// Don't add to indexes here; will do it at the end of the
			// sentence, when we can see whether there are multiple entities
			// for the same node.
			//newAnnot.addToIndexes();
		}
	}
	
	// generateTokens is now a configuration parameter rather than a
	// result specification, because configuration parameters don't work 
	// at the top level of a CPE.
//	@Override
//	public void setResultSpecification(ResultSpecification rs) {
//		super.setResultSpecification(rs);
//		generateTokens = false;
//		for (TypeOrFeature t : rs.getResultTypesAndFeatures()) {
//			if (t.getName().equals("it.celi.uima.lts.Token")) {
//				generateTokens = true;
//			}
//		}
//	}
	public void initialize(UimaContext aContext) 
	throws ResourceInitializationException {
		super.initialize(aContext);
		generateTokens = (Boolean) aContext.getConfigParameterValue("GenerateTokens");
	}
	
	@SuppressWarnings("serial")
	private class xipNode2EntityMap extends HashMap<XipNode,List<NamedEntity>> {
		public void put(XipNode xn, NamedEntity ne) throws AnalysisEngineProcessException {
			List<NamedEntity> nes = this.get(xn);
			if (nes == null) {
				nes = new ArrayList<NamedEntity>();
				this.put(xn, nes);
			}
			nes.add(ne);
		}
		public NamedEntity get(XipNode xn, Class<? extends NamedEntity> type) {
			List<NamedEntity> nes = this.get(xn);
			if (nes == null) {
				return null;
			}
			for (NamedEntity ne : nes) {
				if (type.isAssignableFrom(ne.getClass())) {
					return ne;
				}
			}
			return null;
		}
	}

}

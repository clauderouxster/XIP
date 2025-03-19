package com.xerox.xrce.uima.infomagic;

import infomagic.types.City;
import infomagic.types.GeoInfo;
import it.celi.nutch.geo.GeoServer;
import it.celi.nutch.searcher.Place;
import it.celi.nutch.searcher.PlaceIF;

import java.io.IOException;
import java.io.InputStream;
import java.util.Vector;

import org.apache.uima.UimaContext;
import org.apache.uima.analysis_component.JCasAnnotator_ImplBase;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.FSIterator;
import org.apache.uima.cas.text.AnnotationIndex;
import org.apache.uima.jcas.JCas;
import org.apache.uima.resource.ResourceInitializationException;


public class InfomagicGeo extends JCasAnnotator_ImplBase {

	@Override
	public void initialize(UimaContext context)
			throws ResourceInitializationException {
		super.initialize(context);
		try {
			InputStream stream = InfomagicGeo.class
					.getResourceAsStream("selectedTown.txt");
			if (stream == null) {
				throw new ResourceInitializationException();
			}
			GeoServer.setGNS(stream, Place.MUNICIPALITY_TYPE);
		} catch (NumberFormatException e) {
			throw new ResourceInitializationException(e);
		} catch (IOException e) {
			throw new ResourceInitializationException(e);
		}
	}

	@Override
	public void process(JCas jcas) throws AnalysisEngineProcessException {

		// Create the context array to be used for disambiguation
		AnnotationIndex cities = jcas.getAnnotationIndex(City.type);
		FSIterator it = cities.iterator();
		String[] context = new String[cities.size()];
		int i = 0;
		while (it.hasNext()) {
			City city = (City) it.next();
			context[i++] = cityName(city);
		}

		// Disambiguate and annotate
		it = cities.iterator();
		while (it.hasNext()) {
			City city = (City) it.next();
			Vector<PlaceIF> ambigPlaces = GeoServer.findPlace(cityName(city));
			if (ambigPlaces != null) {
				Vector<PlaceIF> rankedPlaces = GeoServer.disambiguate(
						ambigPlaces, context);
				PlaceIF place = rankedPlaces.elementAt(0);
				GeoInfo geoInfo = new GeoInfo(jcas, city.getBegin(), city
						.getEnd());
				geoInfo.setLatitude(place.getLatitude());
				geoInfo.setLongitude(place.getLongitude());
				geoInfo.setLocationAnnot(city);
				geoInfo.addToIndexes();
			}
		}
	}
	
	private String cityName(City city) {
		String name = city.getCanonicalString();
		if (name == null) name = city.getCoveredText();
		return name;
	}

}

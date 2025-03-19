/**
 * This file comes from CELI's search engine, written under contract for Xerox.
 * Adapted by Aaron Kaplan to integrate it into a UIMA annotator.
 */
package it.celi.nutch.geo;

import it.celi.nutch.searcher.Place;
import it.celi.nutch.searcher.PlaceIF;
import it.celi.nutch.searcher.PlaceProbabilityComparator;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;
import java.util.Collections;

import org.apache.uima.UIMAFramework;
import org.apache.uima.util.Level;
import org.apache.uima.util.Logger;

//import org.apache.commons.logging.Log;
//import org.apache.commons.logging.LogFactory;
//import org.apache.log4j.PropertyConfigurator;
//import org.apache.lucene.analysis.standard.StandardAnalyzer;
//import org.apache.lucene.document.Document;
//import org.apache.lucene.document.Field;
//import org.apache.lucene.index.IndexReader;
//import org.apache.lucene.index.IndexWriter;
//import org.apache.lucene.index.TermEnum;
//import org.apache.lucene.index.TermPositions;

/**
 * @author LUCA
 * 
 */
public class GeoServer {
	static List<String> geoNormalized;

	static List<String> semNormalized;

//	public static void main(String[] args) {
//		try {
//			PropertyConfigurator.configure("../nutch/conf/log4j.properties");
//			// GeoServer.dumpNames(new File("E:/IM/CrawlIM/index"),new
//			// File("E:/IM/geoEntities.txt"));
//			// set index must be disabled if we are creating a new index.
//			// GeoServer.setIndex(new File("geoCose/index"));
//			GeoServer.setGNS(new File("../nutch/geocose/selectedTown.txt"),
//					Place.MUNICIPALITY_TYPE);
//			// GeoServer.setGNS(new File("geoCose/fr.txt"));
//			if (true) {
//				semNormalized = new Vector<String>();
//				semNormalized.add("region");
//				semNormalized.add("edifice");
//
//				geoNormalized = new Vector<String>();
//				geoNormalized.add("geone");
//				geoNormalized.add("ville");
//				geoNormalized.add("lieu");
//				// System.exit(0);
//				addCoordToIndex(
//						new File(
//								"C:/jakarta-tomcat-5.0.16/webapps/nutchX2/crawlResults/crawlIM/crawlIM/completeIndexes"),
//						new File(
//								"C:/jakarta-tomcat-5.0.16/webapps/nutchX2/crawlResults/crawlIM/crawlIM/index"));
//			}
//			Vector<PlaceIF> neq = GeoServer.findPlace("Valence");
//			String[] context = { "Orange", "Auch", "Toulouse", "Sainte-M�re",
//					"Bordeaux" };
//			// String[] context = {"Paris", "Nangis", "Fontainebleau",
//			// "Nemours", "Provins"};
//			Vector<PlaceIF> sortedPlaces = GeoServer.disambiguate(neq, context);
//			for (PlaceIF p : sortedPlaces) {
//				System.out.println(p.toString());
//			}
//
//		} catch (NumberFormatException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//	}

	static Hashtable<String, Vector<PlaceIF>> GNSRepository;

//	static Hashtable<String, PlaceIF> indexPlaceRepository;

	public static final Logger LOG = UIMAFramework.getLogger(GeoServer.class);

	public static void setGNS(InputStream stream, int type)
			throws NumberFormatException, IOException {
		GNSRepository = new Hashtable<String, Vector<PlaceIF>>();
		LOG.log(Level.INFO, "Start loading GNSServer");
//		if (GeoServer.indexPlaceRepository == null) {
//			LOG
//					.warn("It is more efficient to load the index of names before loading the GNSServer");
//		}
		InputStreamReader isr = new InputStreamReader(stream, "UTF-8");
		BufferedReader br = new BufferedReader(isr);
		String l = br.readLine(); // reads metadata

		while ((l = br.readLine()) != null) {
			Place p;
			if (type == Place.GNS_TYPE) {
				p = new Place(l);
			} else {
				try {
					p = new Place(l, type);
				} catch (java.lang.ArrayIndexOutOfBoundsException e) {
					LOG.log(Level.WARNING, "Line " + l + " could not be converted in text");
					continue;
				}
			}
			String searchName = GeoServer.normalizeForSearch(p
					.getNormalizedName());
			String shortName = p.getShortName();
//			if (GeoServer.indexPlaceRepository == null
//					|| GeoServer.indexPlaceRepository.containsKey(searchName)) {
				Vector<PlaceIF> oldVal = GNSRepository.get(searchName);
				if (oldVal == null) {
					oldVal = new Vector<PlaceIF>();
					GNSRepository.put(searchName, oldVal);
				}
				addUnanbiguous(oldVal, p);
				// oldVal.add(p);
				// GNSRepository.put(searchName,p);
				// eliminato, faceva troppo casino
				// oldVal.
				if (false && !shortName.equals("")) {
					GNSRepository.put(GeoServer.normalizeForSearch(shortName),
							oldVal);
					// LOG.debug("--Inserted " + p.toString() +
					// "|"+GeoServer.normalizeForSearch(shortName));
				}
//			} else {
//				// LOG.debug("Skipping
//				// "+normName+"|"+lat+"|"+longi+"|"+searchName);
//
//			}

		}
		isr.close();
		br.close();
		LOG.log(Level.INFO, "There are " + GNSRepository.keySet().size()
				+ " geo names in indexes AND Geo Server");
		LOG.log(Level.INFO, "GNSServer loading done.");

	}

	private static void addUnanbiguous(Vector<PlaceIF> oldVal, Place p) {

		for (PlaceIF ep : oldVal) {
			if (ep.getLatitude() == p.getLatitude()
					&& ep.getLongitude() == p.getLongitude()) {
				return;
			}
		}
		oldVal.add(p);
		LOG.log(Level.FINEST, "Inserted " + p.toString());

	}

	public static Vector<PlaceIF> disambiguate(Vector<PlaceIF> alternatives,
			String[] context) {
		Vector<PlaceIF> cvect = new Vector<PlaceIF>();
		for (String aContext : context) {
			Vector<PlaceIF> newloc = GeoServer.findPlace(aContext);
			if (null != newloc) {
				cvect.addAll(newloc);
			}
		}
		Vector<PlaceIF> result = new Vector<PlaceIF>();
		for (PlaceIF aPlace : alternatives) {
			double probability = 0.0;
			for (PlaceIF p : cvect) {
				probability += aPlace.getProximityLevel(p);
			}
			// aPlace.setProbability(probability);
			aPlace.setProbabilityWithPopulation(probability);
			result.add(aPlace);
		}
		Collections.sort(result, new PlaceProbabilityComparator());
		return result;
	}

//	static private void normalizeSearchableFileds(Document doc) {
//		for (String geoField : geoNormalized) {
//			Field[] fields = doc.getFields(geoField);
//			if (fields != null) {
//				doc.removeFields(geoField);
//				for (int i = 0; i < fields.length; i++) {
//					Field oldField = fields[i];
//					doc.add(new Field(geoField, GeoServer
//							.normalizeForSearch(oldField.stringValue()),
//							Field.Store.YES, Field.Index.TOKENIZED));
//				}
//			}
//		}
//		for (String semField : semNormalized) {
//			Field[] fields = doc.getFields(semField);
//			if (fields != null) {
//				doc.removeFields(semField);
//				for (int i = 0; i < fields.length; i++) {
//					Field oldField = fields[i];
//					doc
//							.add(new Field(
//									semField,
//									it.celi.nutch.semanticsearch.SearchableNamedEntitiesServer
//											.normalizeForSearch(oldField
//													.stringValue()),
//									Field.Store.YES, Field.Index.TOKENIZED));
//				}
//			}
//
//		}
//
//	}

	public static String normalizeForSearch(String name) {
		String nameO = name.trim();
		nameO = nameO.replaceFirst("\\A[sS]aint[\\s-]", "st");
		nameO = nameO.replaceFirst("\\A[sS]ainte[\\s-]", "ste");
		return nameO.toLowerCase().replaceAll(" ", "").replaceAll("-", "");
	}

//	public static void setIndex(File indexFile) throws IOException {
//		LOG.info("Start loading index names from "
//				+ indexFile.getAbsolutePath());
//
//		indexPlaceRepository = new Hashtable<String, PlaceIF>();
//		IndexReader ir = IndexReader.open(indexFile);
//		// Term t = new Term("geone", "");
//		TermEnum te = ir.terms();
//		while (te.next()) {
//			if (te.term().field().equals("geone")) {
//				String val = te.term().text();
//				String normalized = GeoServer.normalizeForSearch(val);
//				Place p = new Place(val, normalized, "geone", te.docFreq());
//				// LOG.debug("Insert in index "+val+" with key "+normalized);
//				indexPlaceRepository.put(normalized, p);
//			}
//		}
//		LOG.info("Loading index names done.");
//
//	}

//	private static StringBuffer recoverContent(int docNum, String key,
//			IndexReader ir) throws IOException {
//		TermEnum te = ir.terms();
//
//		TermPositions tp = ir.termPositions();
//		GrowableStringArray terms = new GrowableStringArray();
//		;
//		while (te.next()) {
//			String term = null;
//			// skip stored fields
//			// if (sf.contains(te.term().field())) continue;
//			tp.seek(te.term());
//			if (!tp.skipTo(docNum) || tp.doc() != docNum) {
//				// this term is not found in the doc
//				continue;
//			}
//			if (!te.term().field().equals(key)) {
//				continue;
//			}
//			term = te.term().text();
//			// System.out.println(te.term().field());
//			for (int k = 0; k < tp.freq(); k++) {
//				int pos = tp.nextPosition();
//				terms.append(pos, " ", term);
//			}
//		}
//		String sep = " ";
//		if (terms == null)
//			return null;
//		StringBuffer sb = new StringBuffer();
//		String sNull = "null";
//		int k = 0, m = 0;
//		for (int j = 0; j < terms.size(); j++) {
//			if (terms.get(j) == null)
//				k++;
//			else {
//				if (sb.length() > 0)
//					sb.append(sep);
//				if (m > 0 && m % 5 == 0)
//					sb.append('\n');
//				if (k > 0) {
//					sb.append(sNull + "_" + k + sep);
//					k = 0;
//					m++;
//				}
//				// ERA: sb.append(escape(terms.get(j)));
//				sb.append(terms.get(j));
//				// else sb.append(terms.get(j));
//				m++;
//			}
//		}
//		return sb;
//	}
//
//	public static void addCoordToIndex(File indexFile, File newIndex)
//			throws IOException {
//		IndexReader ir = IndexReader.open(indexFile);
//		IndexWriter indexWriter = new IndexWriter(newIndex,
//				new StandardAnalyzer(), true);
//		int maxDocs = ir.numDocs();
//		for (int i = 0; i < maxDocs; i++) {
//			/*
//			 * if(i>200){ return; }
//			 */
//			if (i % 100 == 0)
//				LOG.info(i + " documents rewritten");
//			// LOG.debug(recoverContent(i,"content",ir).toString());
//			Document doc = ir.document(i);
//			Field f = doc.getField("url");
//			doc.add(new Field("content", recoverContent(i, "content", ir)
//					.toString(), Field.Store.NO, Field.Index.TOKENIZED));
//			doc.add(new Field("site", recoverContent(i, "site", ir).toString(),
//					Field.Store.NO, Field.Index.UN_TOKENIZED));
//			LOG.debug(f.stringValue());
//
//			Field[] fs = doc.getFields("ville");
//			if (null != fs) {
//				String places[] = new String[fs.length];
//				for (int j = 0; j < fs.length; j++) {
//					places[j] = fs[j].stringValue();
//				}
//				for (int j = 0; j < places.length; j++) {
//					String ville = places[j];
//					Vector<PlaceIF> ambPLaces = findPlace(ville);
//					if (ambPLaces == null) {
//						LOG.debug("No coordinates for " + ville + " with key "
//								+ GeoServer.normalizeForSearch(ville));
//						continue;
//					}
//					Vector<PlaceIF> nev;
//					if (ambPLaces.size() > 1) {
//						nev = GeoServer.disambiguate(ambPLaces, places);
//					} else {
//						nev = ambPLaces;
//					}
//					PlaceIF p = (PlaceIF) nev.elementAt(0);
//					doc.add(new Field("insee", p.getInsee(), Field.Store.YES,
//							Field.Index.UN_TOKENIZED));
//					double no = p.getLatitude();
//					double ea = p.getLongitude();
//					// LOG.debug(p.getNormalizedName()+"|"+p.getLatitude()+"|"+p.getLongitude()
//					// +" (had "+ambPLaces.size()+" ambiguities");
//					// System.out.println(p.getNormalizedName()+"|"+p.getLatitude()+"|"+p.getLongitude());
//					try {
//						float North = (float) no;
//						float East = (float) ea;
//
//						Domain2Position xyz = new Domain2Position(North, East);
//
//						// add lucene Terms
//						doc.add(new Field("north", xyz.formatPolar(North),
//								Field.Store.YES, Field.Index.NO));
//						doc.add(new Field("east", xyz.formatPolar(East),
//								Field.Store.YES, Field.Index.NO));
//						doc.add(new Field("posX", Domain2Position
//								.formatCartesian(xyz.getX()), Field.Store.NO,
//								Field.Index.TOKENIZED));
//						doc.add(new Field("posY", Domain2Position
//								.formatCartesian(xyz.getY()), Field.Store.NO,
//								Field.Index.TOKENIZED));
//						doc.add(new Field("posZ", Domain2Position
//								.formatCartesian(xyz.getZ()), Field.Store.NO,
//								Field.Index.TOKENIZED));
//					} catch (NumberFormatException c) {
//						System.out.println(c);
//					}
//				}
//			}
//			GeoServer.normalizeSearchableFileds(doc);
//			indexWriter.addDocument(doc);
//		}
//		indexWriter.optimize();
//		indexWriter.close();
//	}

//	public static void dumpNames(File indexFile, File outFile)
//			throws IOException {
//		LOG.info("reading from " + indexFile.getAbsolutePath());
//		LOG.info("Writing to " + outFile.getAbsolutePath());
//		FileOutputStream outputFile = new FileOutputStream(outFile);
//		OutputStreamWriter fw = new OutputStreamWriter(outputFile, "UTF-8");
//		BufferedWriter br = new BufferedWriter(fw);
//
//		IndexReader ir = IndexReader.open(indexFile);
//		int maxDocs = ir.maxDoc();
//		for (int i = 0; i < maxDocs; i++) {
//			Document d = ir.document(i);
//			Field f = d.getField("url");
//			Field[] fs = d.getFields("geone");
//			if (null == fs) {
//				continue;
//			}
//			br.write(f.stringValue() + "\t");
//			// System.out.print();
//			for (int j = 0; j < fs.length; j++) {
//				f = fs[j];
//				br.write(f.stringValue() + "\t");
//			}
//			br.write("\n");
//		}
//		LOG.info("dumping done");
//
//	}

	/**
	 * Looks in the GNS to see if a place with that name is available. If
	 * available it returns an array of possible candidates with normalized form
	 * and geo coordinates.
	 * 
	 * @param name
	 * @return
	 */
	public static Vector<PlaceIF> findPlace(String name) {
		return GNSRepository.get(normalizeForSearch(name));
	}

	public static boolean isInitialized() {
		return GNSRepository != null;
	}
}

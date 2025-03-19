/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

/**
 * The purpose of this class is to provide the constants
 *
 * @author grondeau @created April 8, 2005
 */
public class Const {

    //The different DISPLAY modes used with XipSetDisplayMode function
    public static final int ONE = 1; //this value is add to allow a setDisplayMode to 0
    public static final int DISPLAY_LEMMA = ONE + 0;
    public static final int DISPLAY_SURFACE = ONE + 1;
    public static final int DISPLAY_MARKUP = ONE + 2;
    public static final int DISPLAY_ENTREE = ONE + 3;
    public static final int DISPLAY_CATEGORY = ONE + 4;
    public static final int DISPLAY_REDUCED = ONE + 5;
    public static final int DISPLAY_FULL = ONE + 6;
    public static final int DISPLAY_OFFSET = ONE + 7;
    public static final int DISPLAY_WORDNUM = ONE + 8;
    public static final int DISPLAY_SENTENCE = ONE + 9;
    public static final int DISPLAY_NONE = ONE + 14;
    public static final int DISPLAY_DEPENDENCY_BY_NAME = ONE + 15;
    public static final int DISPLAY_DEPENDENCY_BY_NODE = ONE + 16;
    public static final int DISPLAY_DEPENDENCY_BY_CREATION = ONE + 17;
    public static final int DISPLAY_TREE = ONE + 18;
    public static final int DISPLAY_TREE_PHRASE = ONE + 19;
    public static final int DISPLAY_TREE_COLUMN = ONE + 20;
    public static final int DISPLAY_MERGE_XML_SUBTREE = ONE + 21;
    public static final int DISPLAY_CONVERSION_UTF8 = ONE + 22;
    public static final int DISPLAY_EXECUTION_ERROR = ONE + 23;
    public static final int DISPLAY_MATHEMATICS = ONE + 24;
    public static final int DISPLAY_DEPENDENCY_NUMBER = ONE + 25;
    public static final int UTF8_INPUT = ONE + 26;
    public static final int DISPLAY_EXECUTE_TOKEN = ONE + 27;
    public static final int DISPLAY_SENTENCE_NUMBER = ONE + 28;
    public static final int LANGUAGE_GUESSER = ONE + 29;
    public static final int NOT_USED = ONE + 30;
    public static final int DISPLAY_CHUNK_TREE = ONE + 31;
    public static final int DISPLAY_DEPENDENCY_FEATURE_VALUE = ONE + 32;
    public static final int NO_TAG_NORMALISATION = ONE + 33;
    public static final int LOWER_INPUT = ONE + 34;
    public static final int CHECK_INPUT_UTF8 = ONE + 35;
    public static final int DISPLAY_GENERATION_CATEGORY = ONE + 36;
    public static final int DISPLAY_GENERATION = ONE + 37;
    public static final int RANDOM_ANALYSIS = ONE + 38;
	public static final int DISPLAY_PASSAGE_XML_FORMAT = ONE + 39;
	public static final int DISPLAY_JSON = ONE + 40;
	public static final int DISPLAY_JSON_API = ONE + 41;
	public static final int DISPLAY_COMPUTE_DISTANCE = ONE + 42;
	public static final int DISPLAY_CLOSING_POS = ONE + 43;
	public static final int DISPLAY_TREEBANK = ONE + 44;

    //
    public static final char XML_NONE = 0;
    public static final char XML_OUTPUT = 1;
    public static final char XML_INSERT = 2;
    public static final int COL = 80;
    //value for the ruleType parameter of the method RuleBuilder::buildRule()
    //rule type = depenedency
    public static final int DEPENDENCYRULE = 0;
    //rule type = sequence
    public static final int SEQUENCERULE = 1;
    //rule type = left context
    public static final int LEFTCONTEXT = 2;
    //rule type = right context
    public static final int RIGHTCONTEXT = 3;
    //rule type = ID
    public static final int IDRULE = 4;
    //rule type = tagging
    public static final int TAGGINGRULE = 5;
}

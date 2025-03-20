/* $Id: tok-aux.c,v 1.47 2004/02/03 01:18:43 lk Exp $ */

/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library,
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.63
   filename   : transduct.cxx
   Date       :
   Purpose    : TRANSDUCTION XIPTRANS IMPLEMENTATION
   Programmer : Claude ROUX
   Reviewer   :
   */

#include "parsbin.h"
#include "ntmhmm.h"
#include "automate.h"
#include "transduct.h"

void MettreEnMinuscule(unsigned char* iterateur);

//------------------------------------------------------------------------------
//									LOOK UP
//------------------------------------------------------------------------------
void GlobalParseurTrans::Process(charRead& w, vector<string>& readings) {
	w.reset(); //we releoad a line of fresh strings from a file if ncessary. Iactive for a string parsing
	for (int i = 0; i < alls.size(); i++) {
		readings.clear();
		if (alls[i]->process(w, readings, OUI))
			return;
		w.setpos(w.bbegin, w.cbegin);
	}

	string s;
	//No value found....
	w.setpos(w.bbegin, w.cbegin);
	w.eset(w.bbegin, w.cbegin);

	BULONG cr = w.nextcode();
	if (c_is_punctuation(cr)) {
		w.eset(w.w);
		s = w.extract();
		s += "\t";
		s += GUESSED_TAG;
		s += "+PUNCT";
	}
	else {
		if (w.end())
			return;

		while (!w.end() && !c_is_space(cr) && !c_is_punctuation(cr)) {
			w.eset(w.w);
			cr = w.nextcode();
		}
		if (!w.end())
			w.setpos(w.bend, w.cend);

		s = w.extract();
		if (s == "")
			return;

		s += "\t";
		s += GUESSED_TAG;
	}
	readings.push_back(s);
}

char GlobalParseurTrans::lookup() {

	if (!lectures.dernier)
		return NON;
	if (MaxSentence != -1 && num_phrase > MaxSentence)
		return OUI;

	//Nous disposons donc de notre structure de lecture...
	borneGauche = lectures[0]->gauche;
	borneDroite = lectures[0]->droite;
	borneGaucheCar = lectures[0]->gauchecar;
	borneDroiteCar = lectures[0]->droitecar;

	ostream* os = &cout;
	ListeVoc* voc = NULL;

	if (OS != NULL)
		os = OS;

	string ch;
	char chmin[100];
	char testFinDePhrase = NON;
	char testBalise = NON;

	char retourchariotlocal = NON;

	char ajouteFinLigne = NON;
	//chaque lecture correspond a un mot
	if (gAfficherResultats == OUI &&
		TestOptions(AFFICHAGE_OFFSET) &&
		appliqueGrammaire == NON && xipresult == NULL)
		*os << "[" << borneGauche << "-" << borneDroite << "]" << Endl;

	char* ptt;

	for (int i = 0; i < lectures.dernier; i++) {
		char* s = lectures[i]->mot;
		ptt = NULL;

		//Dans le cas d'un passage par le HMM, il faut garder la trace des offsets...
		if (wbf != NULL) {
			if (borneGauche == borneGaucheCar && borneDroite == borneDroiteCar)
				sprintf_s(chmin, 100, "+%d+%d", borneGauche, borneDroite);
			else
				sprintf_s(chmin, 100, "+%d+%d+%d+%d", borneGauche, borneDroite, borneGaucheCar, borneDroiteCar);
			//On cherche les traits:
			ch = chmin;
			ptt = strrchr(s, 9);
			if (ptt != NULL) {
				ch += ptt + 1;
				ptt[1] = 0;
				lectures[i]->longueur = ptt - s + 1;
				s = lectures[i]->concatene(STR(ch));
			}
		}


		if (appliqueGrammaire == NON && xipresult == NULL) {
			ajouteFinLigne = OUI;
			num_mot++;
			nombre_mots++;
			if (gAfficherResultats == OUI)
				*os << ConversionChaine(this, s) << Endl;
		}
		else {
			if (wbf == NULL) {
				if (testBalise == NON && TestBalise(s) == OUI)
					return NON;

				if (testBalise == NON)
					testFinDePhrase = EndOfSentence(s, NON);

				testBalise = OUI;
				char* copieChaine;
				LexiconEntryParsing(s, &copieChaine);
				if (voc == NULL) {
					voc = ConstruitForme(copieChaine, num_mot);
					if (voc != NULL) {
						voc->borneGauche = borneGauche;
						voc->borneDroite = borneDroite;
						voc->borneGaucheCar = borneGaucheCar;
						voc->borneDroiteCar = borneDroiteCar;
#ifdef XIPLIBXML
						voc->xmlnoeud = noeudxmltokencourrant;
#endif
						num_mot++;
						nombre_mots++;
					}
				}
				else
					FusionneForme(voc, copieChaine);
			}//wbf!=NULL
			else {
				int tag, num_tokens;

				if (testBalise == NON) {
					auto_vocabulaire* trad = RechercheBalise(s);
					pileDeMots.ajoute(trad);
					if (trad != NULL)
						return NON;
					testBalise = OUI;
				}

				analyse_input_line(s, &num_tokens, &tag);
				store_to_word_buffer(wbf, s, tag, (!i));
			}
		}
	}

	if (wbf == NULL) {
		if (testFinDePhrase == OUI || EndOfSentence(NULL, UTILISE)) {
			Parse(OUI);
			Detruit();

			num_phrase++;

			if (renumerotation == OUI) {
				num_token = 0;
				num_mot = 0;
			}
			return OUI;
		}
	}

	if (ajouteFinLigne == OUI && gAfficherResultats == OUI)
		*os << Endl;

	return OUI;
}

//------------------------------------------------------------------------------
//									APPLY
//------------------------------------------------------------------------------

void GlobalParseurTrans::Apply() {

	if (currentreader == NULL)
		return;

	retourchariot = NON;
	vector<string> readings;
	word_offset* wo;
	string surface;
	currentreader->begin();
	lectures.raz();
	agnostring feat;
	int i, sz, ps;
	while (!currentreader->end()) {
		Process(*currentreader, readings);
		sz = readings.size();
		if (!sz)
			continue;

		surface = currentreader->extract();
		for (i = 0; i < sz; i++) {
			wo = fournirWordOffset();
			currentreader->offset(wo->gauche, wo->droite, wo->gauchecar, wo->droitecar);
			wo->concatene(surface);
			wo->concatene("\t");
			feat = readings[i];
			ps = feat.find("\t", 0);
			if (ps == string::npos)
				continue;
			if (feat.find("\t", ps + 1) != string::npos) {
				feat = feat.replace("\t", "");
				feat.insert("\t", ps);
			}
			wo->concatene(feat);
		}

		if (lookup() == OUI) {
			if (wbf != NULL) {
				exchange_vit_vectors(vtree);   /* prev <--> cur */
				clear_vit_vector_and_lists(vtree->cur, vtree);

				set_current_class_data(wbf, vtree);
				viterbi_next_word(TRUE);
			}
		}
		nettoieLectures();
	}

	delete currentreader;
	currentreader = NULL;
}

void GlobalParseurTrans::SentenceAnalysis(char renumerote, char arbre) {
	num_phrase = 0;
	wbf = NULL;
	baseOffset = 0;
	baseOffsetCar = 0;
	renumerotation = renumerote;

	//Dans un mode d'analyse par token, on ne passe par Apply
	if (initialisation_analyse_token == OUI) {
		CloreAnalyse();
		return;
	}


	if (hmmFileName != NULL && hmmFileName[0] != 0) {
		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = TRUE;

		wbf = alloc_word_buffer(MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
		vtree = alloc_viterbi(TAG.num);
	}

	Apply();

	if (wbf != NULL && wbf->num)   /* still words in buffer */
	{
		vtree->cur->flags.sent_end = TRUE;
		viterbi_next_word(FALSE);
	}

	FinAnalyse = OUI;
	while (phrase.dernier != 0) {
		Parse(OUI);
		Detruit();
		num_phrase++;
	}
	FinAnalyse = NON;
	//HMM lu
	if (wbf != NULL) {
		words_total = wbf->words_total;

		free_word_buffer(wbf);
		free_viterbi(vtree);

		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = FALSE;
	}
}


void GlobalParseurTrans::StringAssociation(unsigned char* chaine) {
	donneesATraiter = chaine;
	entree_est_chaine = OUI;

	if (TestOptions(ENTREE_EN_MINUSCULE))
		MettreEnMinuscule(donneesATraiter);

	currentreader = new charReadString(chaine);
}

FILE* GlobalParseurTrans::FileAssociation(FILE* thefile) {
	fichierDonnees = thefile;
	entree_est_chaine = NON;
	currentreader = new charReadFile(thefile);
	return fichierDonnees;
}

void GlobalParseurTrans::StartAnalysis() {
	if (initialisation_analyse_token == OUI)
		return;

	wbf = NULL;
	baseOffset = 0;
	baseOffsetCar = 0;

	if (hmmFileName != NULL && hmmFileName[0] != 0) {
		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = TRUE;

		wbf = alloc_word_buffer(MAX_WORDS, MAX_ALTBF, MAX_LINEBF);
		vtree = alloc_viterbi(TAG.num);
	}
	initialisation_analyse_token = OUI;
}

void GlobalParseurTrans::CloseAnalysis() {
	if (initialisation_analyse_token == NON)
		return;

	if (wbf != NULL && wbf->num)   /* still words in buffer */
	{
		vtree->cur->flags.sent_end = TRUE;
		viterbi_next_word(FALSE);
	}

	FinAnalyse = OUI;
	while (phrase.dernier != 0) {
		Parse(OUI);
		Detruit();
		num_phrase++;
	}

	FinAnalyse = NON;
	//HMM lu
	if (wbf != NULL) {
		words_total = wbf->words_total;

		free_word_buffer(wbf);
		free_viterbi(vtree);

		DISAMB_FLAGS.system.cur_line_num = 0;
		DISAMB_FLAGS.system.reading_hmm = FALSE;
		DISAMB_FLAGS.system.reading_input = FALSE;
	}
	initialisation_analyse_token = NON;
}


//------------------------------------------------------------------------------
//									COMPILATION
//------------------------------------------------------------------------------

GlobalParseurTrans::GlobalParseurTrans(vector<string>& ftrans, char* hmm, char* grammar, char* path) : GlobalParseurHMM(hmm, grammar, path) {
	filenames = ftrans;
	Kif_Automaton* a;
	string name;
	for (int i = 0; i < filenames.size(); i++) {
		name = filenames[i];
		a = new Kif_Automaton;
		if (!a->load(name)) {
			char message[1024];
			sprintf_s(message, 1024, "Wrong file format or unknown file: %s", STR(name));
			erreur(message);
		}
		a->fillencoding(false);
		a->start.shuffle();
		alls.push_back(a);
	}
}


GlobalParseurTrans::~GlobalParseurTrans() {
	for (int i = 0; i < alls.size(); i++)
		delete alls[i];
}

void GlobalParseurTrans::reloading() {
	for (int i = 0; i < alls.size(); i++) {
		alls[i]->load(filenames[i]);
		alls[i]->fillencoding(false);
	}
}


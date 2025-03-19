DATA = ../../data/FST_PocketEngineer
all: $(DATA)/lexiquePE.fst $(DATA)/balises.fst

$(DATA)/lexiquePE.fst: lexique_PE.xfst
	fst -l lexique_PE.xfst -e 'save $@' -stop

$(DATA)/balises.fst: balises.xfst
	fst -l balises.xfst -e 'save $@' -stop

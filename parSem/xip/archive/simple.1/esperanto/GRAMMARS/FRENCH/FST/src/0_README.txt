# ============================================================================
# CONTENT:      Comments about construction of FSTs for French XIP grammar
#
# AUTHOR:       Anne Schiller
#
# CREATED:      02-Feb-2006
# ============================================================================

Source files in this directory are converted from original files under
       /home/chanod/projects/NewParser/french2/NTMlex2003

They are re-written to eliminate direct file access in the scripts
(i.e "load stack" or "save stack") and rather use command line optione
("-s File.fst" or -e "save stack $@") in the Makefile
in order to avoid "hidden" dependencies between different files.


The target FSTs are stored in the subdirectory "data".
The names and contents of the target FSTs are unchanged, 
but the following corrections have been applied:


FST:		MODIFICATION
------------------------------------------------------------------------------

Basictok.fst:	replaced "\v" (\x0B) by "\r" (\x0D) [carriage return]
		minor change in entries for hyphens (right context)

num+.fst:	replaced multi-characters FF and DEM by character sequences

guesser.fst:	removed entries containing (undefined) multi-characters
		
terrors.fst:	removed second network on stack (was a copy of norm3.fst)



NOTE:
The secondary make file "make.test" serves to run regression testing
by comparing the new data files (FSTs) with the original ones
(taking into account the modifications above):
The command "gmake -f make.test" will show the results list of equivalence 
tests which should always be "1" (=YES)


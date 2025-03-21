#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=mac-release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1360937237/generation.o \
	${OBJECTDIR}/_ext/1360937237/readxml.o \
	${OBJECTDIR}/_ext/1425642219/networks.o \
	${OBJECTDIR}/_ext/1425642219/tok-aux.o \
	${OBJECTDIR}/_ext/1360937237/graphe.o \
	${OBJECTDIR}/_ext/1360937237/tokenplus.o \
	${OBJECTDIR}/_ext/1360937237/avlvoc.o \
	${OBJECTDIR}/_ext/1360937237/chaineen.o \
	${OBJECTDIR}/_ext/1360937237/affiche.o \
	${OBJECTDIR}/_ext/1360937237/lliste.o \
	${OBJECTDIR}/_ext/1360937237/varlibxml.o \
	${OBJECTDIR}/_ext/1360937237/disamb.o \
	${OBJECTDIR}/_ext/1360937237/ntm.o \
	${OBJECTDIR}/_ext/1360937237/divers.o \
	${OBJECTDIR}/_ext/1360937237/avltrait.o \
	${OBJECTDIR}/_ext/1360937237/xipkif.o \
	${OBJECTDIR}/_ext/1425642219/readscript.o \
	${OBJECTDIR}/_ext/1360937237/result.o \
	${OBJECTDIR}/_ext/1425642219/lookup.o \
	${OBJECTDIR}/_ext/1360937237/initial.o \
	${OBJECTDIR}/_ext/1360937237/kif.o \
	${OBJECTDIR}/_ext/1360937237/ontologies.o \
	${OBJECTDIR}/_ext/1360937237/avlcat.o \
	${OBJECTDIR}/_ext/1360937237/avl.o \
	${OBJECTDIR}/_ext/1360937237/arbren.o \
	${OBJECTDIR}/_ext/1360937237/mathemat.o \
	${OBJECTDIR}/_ext/1360937237/pls.o \
	${OBJECTDIR}/_ext/1360937237/traitement.o \
	${OBJECTDIR}/_ext/1360937237/chainefr.o \
	${OBJECTDIR}/_ext/1360937237/vecteur.o \
	${OBJECTDIR}/_ext/1360937237/xiplibxml.o \
	${OBJECTDIR}/_ext/1360937237/kifraw.o \
	${OBJECTDIR}/_ext/1360937237/xmlxip.o \
	${OBJECTDIR}/_ext/1360937237/main.o \
	${OBJECTDIR}/_ext/1360937237/transduct.o \
	${OBJECTDIR}/_ext/1360937237/applique.o \
	${OBJECTDIR}/_ext/1360937237/createaut.o \
	${OBJECTDIR}/_ext/1360937237/indexation.o \
	${OBJECTDIR}/_ext/1360937237/testutf8.o \
	${OBJECTDIR}/_ext/1360937237/serveur.o \
	${OBJECTDIR}/_ext/1360937237/analyse.o \
	${OBJECTDIR}/_ext/1360937237/parseur.o \
	${OBJECTDIR}/_ext/1360937237/avlfonc.o \
	${OBJECTDIR}/_ext/1360937237/stlresul.o \
	${OBJECTDIR}/_ext/1360937237/noeud.o \
	${OBJECTDIR}/_ext/1360937237/kifn.o \
	${OBJECTDIR}/_ext/1360937237/sinput.o \
	${OBJECTDIR}/_ext/1360937237/automate.o \
	${OBJECTDIR}/_ext/1425642219/ems.o \
	${OBJECTDIR}/_ext/1360937237/avlreg.o \
	${OBJECTDIR}/_ext/1360937237/lecttest.o \
	${OBJECTDIR}/_ext/1425642219/text-io.o \
	${OBJECTDIR}/_ext/1360937237/xipaxip.o \
	${OBJECTDIR}/_ext/1360937237/lire.o


# C Compiler Flags
CFLAGS=-fPIC -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -fno-common

# CC Compiler Flags
CCFLAGS=-fPIC -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -fno-common
CXXFLAGS=-fPIC -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -fno-common

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../fst/dev/lib/mac-i386/staticLib -L../../../xml/dev/lib/mac-i386/staticLib -L../../../ntm/dev/lib/mac-i386/staticLib -lxml -lntm -lfst -lz -lm -lpthread -mt -lstdc++ -ldl -lgcc_s.10.4

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-mac-release.mk xipMac-release

xipMac-release: ${OBJECTFILES}
	${LINK.cc} -o xipMac-release ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1360937237/generation.o: nbproject/Makefile-${CND_CONF}.mk ../src/generation.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/generation.o ../src/generation.cxx

${OBJECTDIR}/_ext/1360937237/readxml.o: nbproject/Makefile-${CND_CONF}.mk ../src/readxml.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/readxml.o ../src/readxml.cxx

${OBJECTDIR}/_ext/1425642219/networks.o: nbproject/Makefile-${CND_CONF}.mk ../src/tokLook/networks.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1425642219
	${RM} $@.d
	$(COMPILE.c) -O2 -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../../../xml/dev/libxml2-2.6.28/include -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../include/tokLook -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1425642219/networks.o ../src/tokLook/networks.c

${OBJECTDIR}/_ext/1425642219/tok-aux.o: nbproject/Makefile-${CND_CONF}.mk ../src/tokLook/tok-aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1425642219
	${RM} $@.d
	$(COMPILE.c) -O2 -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../../../xml/dev/libxml2-2.6.28/include -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../include/tokLook -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1425642219/tok-aux.o ../src/tokLook/tok-aux.c

${OBJECTDIR}/_ext/1360937237/graphe.o: nbproject/Makefile-${CND_CONF}.mk ../src/graphe.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/graphe.o ../src/graphe.cxx

${OBJECTDIR}/_ext/1360937237/tokenplus.o: nbproject/Makefile-${CND_CONF}.mk ../src/tokenplus.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/tokenplus.o ../src/tokenplus.cxx

${OBJECTDIR}/_ext/1360937237/avlvoc.o: nbproject/Makefile-${CND_CONF}.mk ../src/avlvoc.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/avlvoc.o ../src/avlvoc.cxx

${OBJECTDIR}/_ext/1360937237/chaineen.o: nbproject/Makefile-${CND_CONF}.mk ../src/chaineen.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/chaineen.o ../src/chaineen.cxx

${OBJECTDIR}/_ext/1360937237/affiche.o: nbproject/Makefile-${CND_CONF}.mk ../src/affiche.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/affiche.o ../src/affiche.cxx

${OBJECTDIR}/_ext/1360937237/lliste.o: nbproject/Makefile-${CND_CONF}.mk ../src/lliste.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/lliste.o ../src/lliste.cxx

${OBJECTDIR}/_ext/1360937237/varlibxml.o: nbproject/Makefile-${CND_CONF}.mk ../src/varlibxml.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/varlibxml.o ../src/varlibxml.cxx

${OBJECTDIR}/_ext/1360937237/disamb.o: nbproject/Makefile-${CND_CONF}.mk ../src/disamb.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/disamb.o ../src/disamb.cxx

${OBJECTDIR}/_ext/1360937237/ntm.o: nbproject/Makefile-${CND_CONF}.mk ../src/ntm.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/ntm.o ../src/ntm.cxx

${OBJECTDIR}/_ext/1360937237/divers.o: nbproject/Makefile-${CND_CONF}.mk ../src/divers.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/divers.o ../src/divers.cxx

${OBJECTDIR}/_ext/1360937237/avltrait.o: nbproject/Makefile-${CND_CONF}.mk ../src/avltrait.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/avltrait.o ../src/avltrait.cxx

${OBJECTDIR}/_ext/1360937237/xipkif.o: nbproject/Makefile-${CND_CONF}.mk ../src/xipkif.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/xipkif.o ../src/xipkif.cxx

${OBJECTDIR}/_ext/1425642219/readscript.o: nbproject/Makefile-${CND_CONF}.mk ../src/tokLook/readscript.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1425642219
	${RM} $@.d
	$(COMPILE.c) -O2 -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../../../xml/dev/libxml2-2.6.28/include -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../include/tokLook -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1425642219/readscript.o ../src/tokLook/readscript.c

${OBJECTDIR}/_ext/1360937237/result.o: nbproject/Makefile-${CND_CONF}.mk ../src/result.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/result.o ../src/result.cxx

${OBJECTDIR}/_ext/1425642219/lookup.o: nbproject/Makefile-${CND_CONF}.mk ../src/tokLook/lookup.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1425642219
	${RM} $@.d
	$(COMPILE.c) -O2 -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../../../xml/dev/libxml2-2.6.28/include -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../include/tokLook -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1425642219/lookup.o ../src/tokLook/lookup.c

${OBJECTDIR}/_ext/1360937237/initial.o: nbproject/Makefile-${CND_CONF}.mk ../src/initial.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/initial.o ../src/initial.cxx

${OBJECTDIR}/_ext/1360937237/kif.o: nbproject/Makefile-${CND_CONF}.mk ../src/kif.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/kif.o ../src/kif.cxx

${OBJECTDIR}/_ext/1360937237/ontologies.o: nbproject/Makefile-${CND_CONF}.mk ../src/ontologies.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/ontologies.o ../src/ontologies.cxx

${OBJECTDIR}/_ext/1360937237/avlcat.o: nbproject/Makefile-${CND_CONF}.mk ../src/avlcat.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/avlcat.o ../src/avlcat.cxx

${OBJECTDIR}/_ext/1360937237/avl.o: nbproject/Makefile-${CND_CONF}.mk ../src/avl.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/avl.o ../src/avl.cxx

${OBJECTDIR}/_ext/1360937237/arbren.o: nbproject/Makefile-${CND_CONF}.mk ../src/arbren.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/arbren.o ../src/arbren.cxx

${OBJECTDIR}/_ext/1360937237/mathemat.o: nbproject/Makefile-${CND_CONF}.mk ../src/mathemat.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/mathemat.o ../src/mathemat.cxx

${OBJECTDIR}/_ext/1360937237/pls.o: nbproject/Makefile-${CND_CONF}.mk ../src/pls.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/pls.o ../src/pls.cxx

${OBJECTDIR}/_ext/1360937237/traitement.o: nbproject/Makefile-${CND_CONF}.mk ../src/traitement.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/traitement.o ../src/traitement.cxx

${OBJECTDIR}/_ext/1360937237/chainefr.o: nbproject/Makefile-${CND_CONF}.mk ../src/chainefr.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/chainefr.o ../src/chainefr.cxx

${OBJECTDIR}/_ext/1360937237/vecteur.o: nbproject/Makefile-${CND_CONF}.mk ../src/vecteur.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/vecteur.o ../src/vecteur.cxx

${OBJECTDIR}/_ext/1360937237/xiplibxml.o: nbproject/Makefile-${CND_CONF}.mk ../src/xiplibxml.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/xiplibxml.o ../src/xiplibxml.cxx

${OBJECTDIR}/_ext/1360937237/kifraw.o: nbproject/Makefile-${CND_CONF}.mk ../src/kifraw.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/kifraw.o ../src/kifraw.cxx

${OBJECTDIR}/_ext/1360937237/xmlxip.o: nbproject/Makefile-${CND_CONF}.mk ../src/xmlxip.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/xmlxip.o ../src/xmlxip.cxx

${OBJECTDIR}/_ext/1360937237/main.o: nbproject/Makefile-${CND_CONF}.mk ../src/main.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.cxx

${OBJECTDIR}/_ext/1360937237/transduct.o: nbproject/Makefile-${CND_CONF}.mk ../src/transduct.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/transduct.o ../src/transduct.cxx

${OBJECTDIR}/_ext/1360937237/applique.o: nbproject/Makefile-${CND_CONF}.mk ../src/applique.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/applique.o ../src/applique.cxx

${OBJECTDIR}/_ext/1360937237/createaut.o: nbproject/Makefile-${CND_CONF}.mk ../src/createaut.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/createaut.o ../src/createaut.cxx

${OBJECTDIR}/_ext/1360937237/indexation.o: nbproject/Makefile-${CND_CONF}.mk ../src/indexation.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/indexation.o ../src/indexation.cxx

${OBJECTDIR}/_ext/1360937237/testutf8.o: nbproject/Makefile-${CND_CONF}.mk ../src/testutf8.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/testutf8.o ../src/testutf8.cxx

${OBJECTDIR}/_ext/1360937237/serveur.o: nbproject/Makefile-${CND_CONF}.mk ../src/serveur.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/serveur.o ../src/serveur.cxx

${OBJECTDIR}/_ext/1360937237/analyse.o: nbproject/Makefile-${CND_CONF}.mk ../src/analyse.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/analyse.o ../src/analyse.cxx

${OBJECTDIR}/_ext/1360937237/parseur.o: nbproject/Makefile-${CND_CONF}.mk ../src/parseur.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/parseur.o ../src/parseur.cxx

${OBJECTDIR}/_ext/1360937237/avlfonc.o: nbproject/Makefile-${CND_CONF}.mk ../src/avlfonc.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/avlfonc.o ../src/avlfonc.cxx

${OBJECTDIR}/_ext/1360937237/stlresul.o: nbproject/Makefile-${CND_CONF}.mk ../src/stlresul.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/stlresul.o ../src/stlresul.cxx

${OBJECTDIR}/_ext/1360937237/noeud.o: nbproject/Makefile-${CND_CONF}.mk ../src/noeud.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/noeud.o ../src/noeud.cxx

${OBJECTDIR}/_ext/1360937237/kifn.o: nbproject/Makefile-${CND_CONF}.mk ../src/kifn.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/kifn.o ../src/kifn.cxx

${OBJECTDIR}/_ext/1360937237/sinput.o: nbproject/Makefile-${CND_CONF}.mk ../src/sinput.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/sinput.o ../src/sinput.cxx

${OBJECTDIR}/_ext/1360937237/automate.o: nbproject/Makefile-${CND_CONF}.mk ../src/automate.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/automate.o ../src/automate.cxx

${OBJECTDIR}/_ext/1425642219/ems.o: nbproject/Makefile-${CND_CONF}.mk ../src/tokLook/ems.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1425642219
	${RM} $@.d
	$(COMPILE.c) -O2 -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../../../xml/dev/libxml2-2.6.28/include -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../include/tokLook -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1425642219/ems.o ../src/tokLook/ems.c

${OBJECTDIR}/_ext/1360937237/avlreg.o: nbproject/Makefile-${CND_CONF}.mk ../src/avlreg.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/avlreg.o ../src/avlreg.cxx

${OBJECTDIR}/_ext/1360937237/lecttest.o: nbproject/Makefile-${CND_CONF}.mk ../src/lecttest.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/lecttest.o ../src/lecttest.cxx

${OBJECTDIR}/_ext/1425642219/text-io.o: nbproject/Makefile-${CND_CONF}.mk ../src/tokLook/text-io.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1425642219
	${RM} $@.d
	$(COMPILE.c) -O2 -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../../../xml/dev/libxml2-2.6.28/include -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../include/tokLook -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1425642219/text-io.o ../src/tokLook/text-io.c

${OBJECTDIR}/_ext/1360937237/xipaxip.o: nbproject/Makefile-${CND_CONF}.mk ../src/xipaxip.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/xipaxip.o ../src/xipaxip.cxx

${OBJECTDIR}/_ext/1360937237/lire.o: nbproject/Makefile-${CND_CONF}.mk ../src/lire.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -w -DXMLINPUT -DXIPSTLUSE -DXIPLIBXML -DXIPSTLAPI -DSQLITE_ENABLE_COLUMN_METADATA -Dlinuxpure -DNTMCONNECTION -DUNIX -DXIPSTDC -DAPPLE -DNOUNSIGNEDINT -I../include -I../include/tokLook -I../../../fst/dev/src -I../../../fst/dev/src/fst -I../../../ntm/dev/include -I../../../xml/dev/libxml2-2.6.28/include -I../include/kifsqlite -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/lire.o ../src/lire.cxx

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/mac-release
	${RM} xipMac-release

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

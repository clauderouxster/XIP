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
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran.exe
AS=as.exe

# Macros
CND_PLATFORM=Cygwin_4.x_1-Windows
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1161280936/JXipImpl.o \
	${OBJECTDIR}/_ext/1161280936/JXipRuleBuilderImpl.o \
	${OBJECTDIR}/_ext/1161280936/JXipImpl2.o \
	${OBJECTDIR}/_ext/1161280936/generate.o \
	${OBJECTDIR}/_ext/1161280936/JXipResultManagerImpl.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-DXIPLIBXML -DNOUNSIGNEDINT -DFORJAVA
CXXFLAGS=-DXIPLIBXML -DNOUNSIGNEDINT -DFORJAVA

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../lib/windows/sharedLib -lxip

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjXip.dll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjXip.dll: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++.exe -shared -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjXip.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1161280936/JXipImpl.o: ../../src/cxx/JXipImpl.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1161280936
	${RM} $@.d
	$(COMPILE.cc) -g -w -I../../build/windows/includeGenerated -I../../src/cxx -I../../../include  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1161280936/JXipImpl.o ../../src/cxx/JXipImpl.cxx

${OBJECTDIR}/_ext/1161280936/JXipRuleBuilderImpl.o: ../../src/cxx/JXipRuleBuilderImpl.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1161280936
	${RM} $@.d
	$(COMPILE.cc) -g -w -I../../build/windows/includeGenerated -I../../src/cxx -I../../../include  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1161280936/JXipRuleBuilderImpl.o ../../src/cxx/JXipRuleBuilderImpl.cxx

${OBJECTDIR}/_ext/1161280936/JXipImpl2.o: ../../src/cxx/JXipImpl2.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1161280936
	${RM} $@.d
	$(COMPILE.cc) -g -w -I../../build/windows/includeGenerated -I../../src/cxx -I../../../include  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1161280936/JXipImpl2.o ../../src/cxx/JXipImpl2.cxx

${OBJECTDIR}/_ext/1161280936/generate.o: ../../src/cxx/generate.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1161280936
	${RM} $@.d
	$(COMPILE.cc) -g -w -I../../build/windows/includeGenerated -I../../src/cxx -I../../../include  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1161280936/generate.o ../../src/cxx/generate.cxx

${OBJECTDIR}/_ext/1161280936/JXipResultManagerImpl.o: ../../src/cxx/JXipResultManagerImpl.cxx 
	${MKDIR} -p ${OBJECTDIR}/_ext/1161280936
	${RM} $@.d
	$(COMPILE.cc) -g -w -I../../build/windows/includeGenerated -I../../src/cxx -I../../../include  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1161280936/JXipResultManagerImpl.o ../../src/cxx/JXipResultManagerImpl.cxx

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjXip.dll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

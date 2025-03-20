# $Id: Makefile,v 1.9 2003/04/23 11:45:39 jmcoursi Exp $
# Makefile for NTM files

AR              = ar
EXEC_NAME       = ntm
LIB_NAME        = libntm.a

# MUST be set before including Makefile.config
SHARED_LIB_VERSION = 2.1

# Where is the config dir.
CONFIG_DIR	= ../../config
#CONFIG_DIR = $(HOME)/c-fsm/config

# include the site dep configuration file
include $(CONFIG_DIR)/Makefile.config

# name of the package (for symbolic names and tar file)
PACKAGE	= ntm

# define whether you want to use the installed version of fsm (in /opt/Mltt/lib) or the development one (in ../fst)
USE_LIB=INSTALLED
INSTALLED_FST_TOPDIR = $(FSM_DIR)
INSTALLED_FST_INCDIR = $(INSTALLED_FST_TOPDIR)/include
INSTALLED_FST_LIBDIR = $(INSTALLED_FST_TOPDIR)/lib
DEV_FST_TOPDIR       = ..
DEV_FST_INCDIR       = $(DEV_FST_TOPDIR)
DEV_FST_LIBDIR       = $(DEV_FST_TOPDIR)/fst

FST_TOPDIR = $($(USE_LIB)_FST_TOPDIR)
FST_INCDIR = $($(USE_LIB)_FST_INCDIR)
FST_LIBDIR = $($(USE_LIB)_FST_LIBDIR)

NTM_LIB_INCLUDE = ntmtypes.h ntmmemory.h ntmlevels.h ntmtoken.h ntm.h ntmextern.h ntmstdinputobject.h utf8_fatstr_util.h utf8_util.h ConvertUTF.h
NTM_LIB_SOURCE  = ntmmemory.c ntmlevels.c ntmtoken.c ntm.c ntmtypo.c ntmload.c ntmmorpho.c ntmstdinputobject.c
NTM_EXEC_INCLUDE = ntmapp.h
NTM_EXEC_SOURCE  = ntmapp.c

NTM_LIB_OBJ	    = $(NTM_LIB_SOURCE:%.c=$(OBJS_TOPDIR)/%.o)
NTM_LIB_OBJ_D   = $(NTM_LIB_SOURCE:%.c=$(OBJS_TOPDIR)/$(DEBUG_OBJS_SUBDIR)/%$(DEBUG_OBJ_SUFFIX).o) 
NTM_EXEC_OBJ	    = $(NTM_EXEC_SOURCE:%.c=$(OBJS_TOPDIR)/%.o) $(NTMLIB_OBJ)
NTM_EXEC_OBJ_D   = $(NTM_EXEC_SOURCE:%.c=$(OBJS_TOPDIR)/$(DEBUG_OBJS_SUBDIR)/%$(DEBUG_OBJ_SUFFIX).o)

VERSION_FILE		= version.h
MODULE  		= ntm
ALL_LIB_SRC		= $(NTM_LIB_SOURCE) $(NTM_LIB_INCLUDE)
ALL_EXEC_SRC		= $(NTM_EXEC_SOURCE) $(NTM_EXEC_INCLUDE)
ALL_SRC			= $(NTM_LIB_SOURCE) $(NTM_EXEC_SOURCE)

# CFLAGS and LDFLAGS
# When making the libraries, don't forget to add X=Y
APPLY_TRANSDUCER_API_FLAGS=$(X:Y=-DAPPLY_TRANSDUCER_API)
LOCAL_CFLAGS  	=  -I$(FST_INCDIR) $(APPLY_TRANSDUCER_API_FLAGS)
LOCAL_CFLAGS_SHARED	= $(LOCAL_CFLAGS)
LOCAL_CFLAGS_D	= -I$(FST_INCDIR) -DDEBUG $(APPLY_TRANSDUCER_API_FLAGS)
LOCAL_CFLAGS_SHARED_D	= ${LOCAL_CFLAGS_D}
# force link with the non-shared version

LOCAL_LDFLAGS	= $(STATIC_LINK_OPTION) $(FST_LIBDIR)/liballfsp.$(STATIC_LIB_EXT) $(DYNAMIC_LINK_OPTION) -L../lxrt -llxrt_fake
LOCAL_LDFLAGS_SHARED = -L$(FST_LIBDIR) -R$(FST_LIBDIR) -lallfsp

# force link with the non-shared version

LOCAL_LDFLAGS_D	=  $(STATIC_LINK_OPTION) $(FST_LIBDIR)/libfsmcalc_d.$(STATIC_LIB_EXT) $(FST_LIBDIR)/libfsmmaint_d.$(STATIC_LIB_EXT)  $(FST_LIBDIR)/libfsmbase_d.$(STATIC_LIB_EXT) $(FST_LIBDIR)/libfsmmaint_d.$(STATIC_LIB_EXT) $(DYNAMIC_LINK_OPTION) -L../lxrt -llxrt_fake

LOCAL_LDFLAGS_SHARED_D= -L$(FST_LIBDIR) -R$(FST_LIBDIR) -lfsmmaint_d -lfsmbase_d -lfsmmaint_d -lfsmbase_d 

# targets
all:	lib $(EXEC_NAME)$(APP_EXT)

debug: lib_d $(EXEC_NAME)_d$(APP_EXT)

lib:    $(NTM_LIB_OBJ)
	$(AR) -r $(LIB_NAME) $(NTM_LIB_OBJ)

lib_d: $(NTM_LIB_OBJ_D)
	$(AR) -r $(LIB_NAME)_d $(NTM_LIB_OBJ_D)

install-bin: $(INSTALL_BIN_DIR)/install-bin-ntm
	echo "Installation of Executable Version $(VERSION) done."

install-lib: $(INSTALL_LIB_DIR)/install-lib-libntm.a
	echo "Installation of Library Version $(VERSION) done."

install-include: $(NTM_LIB_INCLUDE:%=$(INSTALL_INCLUDE_DIR)/install-include-file-%)
	echo "Installation of Include files Version $(VERSION) done."

install: install-lib install-bin install-include

$(EXEC_NAME)$(APP_EXT): ALWAYS $(NTM_EXEC_OBJ) $(NTM_LIB_OBJ)
	$(LD) $(CFLAGS) $(NTM_EXEC_OBJ) $(NTM_LIB_OBJ) $(LDFLAGS) $(LD_OPTION_O)$@

$(EXEC_NAME)_d$(APP_EXT): ALWAYS $(NTM_EXEC_OBJ_D) $(NTM_LIB_OBJ_D)
	$(LD) $(CFLAGS_D) $(NTM_EXEC_OBJ_D) $(NTM_LIB_OBJ_D) $(LDFLAGS_D) $(LD_OPTION_O)$@

clean:

	$(RM) $(LIB_NAME) $(LIB_NAME_D) $(EXEC_NAME) $(EXEC_NAME)_d objs/*.o objs/debugger/*.o

empty: clean
	$(RM)	*~ core *.[chyl] Makefile* makefile* ChangeLog

ALWAYS:
	@if [ ! -d $(OBJS_TOPDIR) ];	\
	then mkdir $(OBJS_TOPDIR);	\
	fi
	@if [ ! -d $(OBJS_TOPDIR)/$(DEBUG_OBJS_SUBDIR) ];	\
	then mkdir $(OBJS_TOPDIR)/$(DEBUG_OBJS_SUBDIR);		\
	fi

depend: $(ALL_C_SRC)
	$(RM) $(DEPENDENCY_FILE)
	makedepend -f - -DMAKEDEPEND $(CFLAGS) $^ | $(POSTPROCESS_SCRIPT)
	makedepend -f - -DMAKEDEPEND $(CFLAGS_D) -o '$$(DEBUG_OBJ_SUFFIX).o' $^ | $(POSTPROCESS_SCRIPT_D)

# Don't add anything after this line

include $(DEPENDENCY_FILE)


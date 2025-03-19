compilateurForte 	= /opt/FORTE/SUNWspro/bin/CC
compilateurForte2	= /opt/FORTE2/bin/CC
compilateur		= /opt/SUNWspro/bin/CC
compilateurLinux	= g++

rapide = -fast -xarch=v8
parametres = -DUNIX -DSVR4 -DSOLARIS=2005 -Dsparc -Dsun -DREGEX_TYPE=REGEX_TYPE_POSIX -DUNIX_VALUE

vers = `cd ../../scripts;./getVersion`
includes =  -I../../../delivery/xip_$(vers)/include

sources = example.cxx

solarisObjDir = objs/solaris
solarisExeDir = bin/solaris
solarisObj = $(sources:%.cxx=$(solarisObjDir)/%.o)

linuxObjDir = objs/linux
linuxExeDir = bin/linux
linuxObj = $(sources:%.cxx=$(linuxObjDir)/%.o)

#-----------------Solaris -------------------

$(solarisObjDir)/%.o: src/%.cxx
	@mkdir -p $(solarisObjDir)
	$(compilateurForte2) -DNTMCONNECTION -DXMLINPUT  $(rapide) -c $(parametres) $(includes) $< -o $@

$(solarisExeDir)/ex: $(solarisObj)
	@mkdir -p $(solarisExeDir)
	$(compilateurForte2) -o $(solarisExeDir)/ex -L ../../../delivery/xip_$(vers)/lib/solaris -lSTLnhXip $(solarisObj)

solaris: $(solarisExeDir)/ex

#-----------------Linux -------------------

$(linuxObjDir)/%.o: src/%.cxx
	@mkdir -p $(linuxObjDir)
	$(compilateurLinux) -DLINUX -c $(parametres) $(includes) $< -o $@

$(linuxExeDir)/ex: $(linuxObj)
	@mkdir -p $(linuxExeDir)
	$(compilateurLinux) -o $(linuxExeDir)/ex -L ../../../delivery/xip_$(vers)/lib/linux -lSTLnhXipL $(linuxObj)

linux: $(linuxExeDir)/ex

clean:
	rm -rf $(linuxObj) $(solarisObj) $(linuxExeDir)/ex $(solarisExeDir)/ex


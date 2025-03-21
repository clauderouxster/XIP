
<project name="JXip" default="all" basedir=".">
    <property name="env" environment="env" 	value="env"/>
    <property name="parSem.dir"     location="${basedir}/../../.." />
    <property name="xip.dev.dir"    location="${basedir}/.." />
    <property name="common.dir"     location="${basedir}/../../../common" />
    <import file="${common.dir}/buildInclude.xml" />
    <property name="log.level"              value="verbose" />
    <property name="xip.home"               location=".." />
    <property name="dist.dir"          		location="${basedir}/dist" />
    <property name="cxx.source.dir"   	 	location="${basedir}/src/cxx" />
    <property name="java.source.dir"   		location="${basedir}/src/java" />
    <property name="Retrotranslator.dir"   	location="/opt/ParSem/tools/Retrotranslator/Retrotranslator-1.2.3-bin" />
    <property name="jxip.package" value="com.xerox.jxip"/>
       
    <target name="set" depends="platformSetting">
        <property name="jar1" location="dist/jxip.jar" />
        <property name="jar2" location="dist/jxipPy.jar" />
        <property name="jar3" location="dist/jxip_debug.jar" />
        <property name="jar"  location="${jar1}" />
        <property name="root_xip.dir" location="../GRAMMARS" />
        <property name="gram1" location="${root_xip.dir}/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm" />
        <property name="gram2" location="${root_xip.dir}/GERMAN/GRMFILES/german.grm" />
        <property name="gram3" location="${basedir}/toyGram/english.grm" />
        <property name="gram4" location="${root_xip.dir}/SPANISH/GRMFILES/spanish.grm" />
        <property name="gram5" location="${root_xip.dir}/ENGLISH/APPLICATIONS/XipTriplets/GRMFILES_trip/gram_norm_trip.grm" />
        <property name="gram6" location="${root_xip.dir}/FRENCH/BASIC/french.grm" />
        <property name="gram7" location="${root_xip.dir}/FRENCH/BASIC/french_entity.grm" />
        <property name="gram8" location="${root_xip.dir}/PORTUGUESE/GRMFILES/portuguese.grm" />
        <property name="gram" location="${gram1}" />
        
        <!-- french -->
        <property name="sent1" value="Peter is happy." />
        <property name="sent2" value="Le chat noir est sur le toit." />
        <!-- german -->
        <property name="sent3" value="Die Vereinigten Staaten senden sieben Abgeordnete nach Berlin." />
        <property name="sent4" value="Er kommt an." />
        <!-- spanish -->
        <property name="sent7" value="Ahmed le esperaba en el mismo sitio con su  coche." />
        <!-- portuguese -->
        <property name="sent8" value="formalizou judiciais no Tribunal." />
        <property name="sent9" value="" />
        <!-- select sentence or file-->
        <property name="f1" value="${basedir}/input" />
        <property name="f2" value="${basedir}/input.xml" />
        <property name="sentence" value="${sent1}" />
        <property name="class1" value="JXip" />
        <property name="class2" value="XipResultManager" />
        <property name="class3" value="RuleBuilder" />
        <property name="class4" value="RuleManager" />
        <property name="class5" value="JXip2" />
        <property name="class6" value="XipResultManager" />
        <property name="class7" value="RuleLoadedManager" />
        <property name="class" value="${class2}" />
    </target>
    
    <target name="test" depends="jarjxip, run" />
    
    <target name="run" depends="set" >
        <java classpath="${jar}" classname="${jxip.package}.${class}"
              fork="true" failonerror="true" maxmemory="512m">
            <arg value="${gram}"/>
            <arg value="${sentence}"/>
        </java>
    </target>
    
    <!-- generate jar file for xip on the WEB => compiled with jdk1.4 because this is
        the version installed on the linux web server -->
    <target name="jarForWeb" depends="jarjxip" description="generate jar file for xip on the WEB serveur" >
        <antcall target="jarForWeb.common" >
            <param name="srcJar" value="${dist.dir}/jxip.jar" />
        </antcall>
    </target>
    
    <target name="jarForWeb.debug" depends="jarjxipDebug"
            description="debug mode: generate jar file for xip on the WEB serveur" >
        <antcall target="jarForWeb.common" >
            <param name="srcJar" value="${dist.dir}/jxip_debug.jar" />
        </antcall>
    </target>
    
    <target name="jarForWeb.common">
        <delete file="${dist.dir}/jxipJdk14.jar" />
        <path id="classpath">
            <fileset dir="${Retrotranslator.dir}" includes="**/*.jar"/>
        </path>
        <taskdef name="retrotranslator" classpathref="classpath"
                 classname="net.sf.retrotranslator.transformer.RetrotranslatorTask" />
        <retrotranslator srcjar="${srcJar}" destjar="${dist.dir}/jxipJdk14.jar" verify="true">
            <classpath location="/opt/java/j2sdk1.4.2_07/jre/lib/rt.jar"/>
            <classpath refid="classpath"/>
        </retrotranslator>
    </target>

    <target name="all" depends="initAll" >
      <!-- taskSync lockName="${lockJavaTaskSync.name}" -->
            <antcall target="jar" />
            <antcall target="testReg" />
            <antcall target="doc" />
      <!--/taskSync -->
    </target>

    <target name="jar" >
        <antcall target="jarjxip" />
        <antcall target="jarjxippy" />
    </target>

    <target name="jarDebug" depends="setDebugMode, jarjxipDebug"/>

    <target name="testReg" >
        <ant antfile="build.xml" dir="testReg"  />
    </target>
    
    <target name="update" >
        <ant dir="testReg" target="update" />
    </target>
    
    <target name="setDebugMode" >
        <property name="debugMode" value="true" />
    </target>
    
    <target name="jarjxip" depends="initAll, compileJxip">
        <record name="${log.dir}/jarjxip" loglevel="${log.level}" action="start"/>
        <antcall target="createJar" >
            <param name="platformArch"             value="mac-x86_64" />
            <param name="python"             value="false" />
            <param name="jar.file"           location="${dist.dir}/jxip.jar" />
            <param name="build.libElem.dir"  location="${build.dir}/lib/jXip"  />
        </antcall>
        <record name="${log.dir}/jarjxip" action="stop"/>
    </target>
     
    <target name="jarjxipDebug" depends="initAll, compileJxip_debug">
        <record name="${log.dir}/jarjxipDebug" loglevel="${log.level}" action="start"/>
        <antcall target="createJar" >
            <param name="python"             value="false" />
            <param name="jar.file"           location="${dist.dir}/jxip.jar" />
            <param name="build.libElem.dir"  location="${build.dir}/lib/jXip_debug"  />
        </antcall>
        <record name="${log.dir}/jarjxipDebug" action="stop"/>
    </target>
    
    <target name="jarjxippy" depends="initAll, compileJxipPy"  >
        <record name="${log.dir}/jarjxippy" loglevel="${log.level}" action="start"/>
        <if>
            <or>
                <equals arg1="${platform}" arg2="windows" />
                <equals arg1="${platform}" arg2="windows64" />
            </or>
            <then>
                <!-- the name must be python25.dll and not python.dll: required on system without python installed -->
                <copy file="${python.lib.dir}/${python.lib.name}.dll"
                      tofile="${build.dir}/lib/jXipPy/${platform-arch}/sharedLib/${python.lib.name}.dll"/>
            </then>
            <else>
                <if>
                    <equals arg1="${platform}" arg2="mac" />
                    <then>
                        <copy file="${python.lib.dir}/lib${python.lib.name}.dylib"
                              tofile="${build.dir}/lib/jXipPy/${platform-arch}/sharedLib/libpython.so"/>
                    </then>
                    <else>
                        <if>
                            <equals arg1="${platform}" arg2="solaris" />
                            <then>
                            </then>
                            <else>
                                <copy file="${python.lib.dir}/lib${python.lib.name}.so"
                                      todir="${build.dir}/lib/jXipPy/${platform-arch}/sharedLib"/>
                            </else>
                        </if>
                    </else>
                </if>
            </else>
        </if>
        <antcall target="createJar" >
            <param name="python"             value="true" />
            <param name="jar.file"           location="${dist.dir}/jxipPy.jar" />
            <param name="build.libElem.dir"  location="${build.dir}/lib/jXipPy"  />
        </antcall>
        <record name="${log.dir}/jarjxippy" action="stop"/>
    </target>
    
    <target name="createJar" depends="initAll" description="" >
        <echo message="${build.libElem.dir}"/>

        <record name="${log.dir}/createJar" action="start"/>
        <delete file="${jar.file}" />
        <!-- init.properties used by DynLoaderFromJar -->
        <if>
            <istrue value="${python}"/>
            <then>
                <echo message="python=true" file="${build.classes.dir}/init.properties" />
            </then>
            <else>
                <echo message="python=false" file="${build.classes.dir}/init.properties" />
            </else>
        </if>
        <!-- create jar file -->
        <jar jarfile="${jar.file}" update="no">
            <fileset file="${build.classes.dir}/init.properties"/>
            <fileset dir="${build.classes.dir}"/>
            <fileset dir="${build.libElem.dir}" excludes="**/*.lib">
                <include name="**/*.dll"/>
                <include name="**/*.so"/>
            </fileset>
            <manifest>
                <attribute name="Built-By" value="${env.USER}"/>
                <attribute name="Main-class" value="${jxip.package}.JXip"/>
                <attribute name="Class-path" value="."/>
            </manifest>
        </jar>
        <record name="${log.dir}/createJar" action="stop"/>
    </target>
    
    <!-- compile and link C++ and java code -->
    <target name="compile" depends="compileJxip, compileJxipPy" />
    <target name="compile.debug" depends="compileJxip_debug" />
    
    <target name="compileJxip" depends="initAll" description="" >
        <record name="${log.dir}/compileJxip" loglevel="${log.level}" action="start"/>
        <antcall target="javaCompile" >
            <param name="java_debug"    value="off" />
            <param name="java_optimize" value="on" />
        </antcall>
        <antcall target="nativeHeader" />
        <antcall target="cxxCompile" >
            <param name="platformArch"             value="mac-x86_64" />
            <param name="modeName"       value="jXip" />
            <param name="libName"        value="jXip" />
            <param name="xip.lib.dir"    location="${xip.root.lib}/${platform-arch}/staticLib" />
            <param name="xip.lib.name"   value="xip" />
            <param name="cc.debug"       value="${cc.debug.common}" />
            <param name="lg.shared.obj.dir"      location="${lg.dir}/obj/${platform-arch}/sharedLib" />
            <param name="fst.shared.obj.dir"     location="${fst.dir}/obj/${platform-arch}/sharedLib" />
            <param name="xml.shared.obj.dir"     location="${xml.dir}/obj/${platform-arch}/sharedLib" />
            <param name="ntm.shared.obj.dir"     location="${ntm.dir}/obj/${platform-arch}/sharedLib" />
            <param name="xip.shared.obj.dir"     location="${xip.obj.dir}/sharedLib" />
        </antcall>
        <record name="${log.dir}/compileJxip"  action="stop"/>
    </target>
    
    <target name="compileJxip_debug" depends="initAll" description="" >
        <record name="${log.dir}/compileJxip_debug" loglevel="${log.level}" action="start"/>
        <antcall target="javaCompile"  >
            <param name="java_debug"    value="on" />
            <param name="java_optimize" value="off" />
        </antcall>
        <antcall target="nativeHeader" />
        <antcall target="cxxCompile" >
            <param name="platformArch"             value="mac-x86_64" />
            <param name="modeName"       value="jXip_debug" />
            <param name="libName"        value="jXip" />
            <param name="xip.lib.dir"    location="${xip.root.lib}/${platform-arch}.debug/staticLib" />
            <param name="xip.lib.name"   value="xip" />
            <param name="cc.debug"       value="true" />
            <param name="lg.shared.obj.dir"      location="${lg.dir}/obj/${platform-arch}.debug/sharedLib" />
            <param name="fst.shared.obj.dir"     location="${fst.dir}/obj/${platform-arch}.debug/sharedLib" />
            <param name="xml.shared.obj.dir"     location="${xml.dir}/obj/${platform-arch}.debug/sharedLib" />
            <param name="ntm.shared.obj.dir"     location="${ntm.dir}/obj/${platform-arch}.debug/sharedLib" />
            <param name="xip.shared.obj.dir"     location="${xip.obj.dir}/sharedLib" />
        </antcall>
        <record name="${log.dir}/compileJxip_debug"  action="stop"/>
    </target>
     
    <target name="compileJxipPy" depends="initAll" description=""   >
        <record name="${log.dir}/compileJxipPy" loglevel="${log.level}" action="start"/>
        <antcall target="javaCompile" >
            <param name="java_debug"        value="off" />
            <param name="java_optimize"     value="on" />
        </antcall>
        <antcall target="nativeHeader" />
        <if>
            <equals arg1="${platform}" arg2="solaris" />
            <then>
                <!-- solaris -->
                <antcall target="cxxCompile" >
                    <param name="platformArch"             value="mac-x86_64" />
                    <param name="modeName"       value="jXipPy" />
                    <param name="libName"        value="jXipPy" />
                    <param name="xip.lib.dir"    location="${xip.root.lib}/${platform-arch}/staticLib" />
                    <param name="xip.lib.name"   value="xip" />
                    <param name="cc.debug"       value="${cc.debug.common}" />
                    <param name="lg.shared.obj.dir"      location="${lg.dir}/obj/${platform-arch}/sharedLib" />
                    <param name="fst.shared.obj.dir"     location="${fst.dir}/obj/${platform-arch}/sharedLib" />
                    <param name="xml.shared.obj.dir"     location="${xml.dir}/obj/${platform-arch}/sharedLib" />
                    <param name="ntm.shared.obj.dir"     location="${ntm.dir}/obj/${platform-arch}/sharedLib" />
                    <param name="xip.shared.obj.dir"     location="${xip.obj.dir}/sharedLib" />
                </antcall>
            </then>
            <else>
                <!-- not solaris -->
                <antcall target="cxxCompile" >
                    <param name="platformArch"             value="mac-x86_64" />
                    <param name="modeName"       value="jXipPy" />
                    <param name="libName"        value="jXipPy" />
                    <param name="xip.lib.dir"    location="${xip.root.lib}/${platform-arch}/staticLibPy" />
                    <param name="xip.lib.name"   value="xips" />
                    <param name="cc.debug"       value="${cc.debug.common}" />
                    <param name="python"         value="defined" />
                    <param name="lg.shared.obj.dir"      location="${lg.dir}/obj/${platform-arch}/sharedLib" />
                    <param name="fst.shared.obj.dir"     location="${fst.dir}/obj/${platform-arch}/sharedLib" />
                    <param name="xml.shared.obj.dir"     location="${xml.dir}/obj/${platform-arch}/sharedLib" />
                    <param name="ntm.shared.obj.dir"     location="${ntm.dir}/obj/${platform-arch}/sharedLib" />
                    <param name="xip.shared.obj.dir"     location="${xip.obj.dir}/sharedLibPy" />
                </antcall>
            </else>
        </if>
        <record name="${log.dir}/compileJxipPy"  action="stop"/>
    </target>

    <!--  -->
    <target name="cxxCompile" depends=" copyGenerate, xipCompilerLinkerSetting"
            description="">
        <record name="${log.dir}/cxxCompile.${modeName}" loglevel="${log.level}" action="start"/>
        <property name="lib.dir" location="${build.lib.dir}/${modeName}/${platformArch}/sharedLib" />
        <mkdir dir="${lib.dir}" />
        <property name="output.lib.name" location="${lib.dir}/${libName}" />
        <property name="obj.dir" location="${build.obj.dir}/${modeName}" />
        <mkdir dir="${obj.dir}" />
        <cc name="${compilerCplus.name}" warnings="none" optimize ="extreme" subsystem="console"
            exceptions="true" multithreaded="true" runtime="dynamic"
            objdir="${obj.dir}" outtype="shared"  outfile="${output.lib.name}">
            <compiler extends="compilerSettingsDyn" name="${compilerCplus.name}" >
                <fileset dir="${cxx.source.dir}" includes="*.cxx"/>
                <!--compilerarg value="-WI,add-stdcall-alias" location="end"/-->
                <includepath if="mac">
                    <pathelement location="/System/Library/Frameworks/JavaVM.framework/Headers"/>
                </includepath>
                <includepath unless="mac">
                    <pathelement location="${java.home}/../include"/>
                </includepath>
                <includepath>
                    <pathelement location="${build.include.dir}"/>
                    <pathelement location="${xip.home}/include"/>
                </includepath>
                <includepath if="windows">
                    <pathelement location="${java.home}/../include/win32"/>
                </includepath>
                <includepath if="mac">
                    <pathelement location="${java.home}/../include/darwin"/>
                </includepath>
                <includepath if="linux">
                    <pathelement location="${java.home}/../include/${platform}"/>
                </includepath>
                <includepath if="linux64">
                    <pathelement location="${java.home}/../include/${platform}"/>
                </includepath>
                <includepath if="solaris">
                    <pathelement location="${java.home}/../include/${platform}"/>
                </includepath>
                <defineset define="XIPLIBXML NOUNSIGNEDINT FORJAVA"/>
                <defineset define="WINDOWS WIN32 XIPDLL_EXPORT" if="windows"/>
                <defineset define="APPLE" if="mac"/>
            </compiler>
            <linker extends="linkerShared" name="${compilerCplus.name}" if="allSrcXipCompiled">
                <!-- 
                 - I want only libjXip.so, without other lib dyn required to avoid dynamic link a runtime 
                        (otherwise it will required to set LD_LIBRARY_PATH)
                 - I cant't use the libxip.a (static lib) to build libjXip.so in the delivery (because *.so must 
                        be build with object files compiled with the -fPIC option (it's not the case with libxip.a)
                 - To build this shared lib on linux and solaris the object files of the libraries like fst
                    must be compile for a shared lib (with -fPIC) but if we use the shared lib(fsp.so) it will
                    be linked dynamically at runtime: so to avoid this, we include directly these object files
                    in this link.
                -->  
                <fileset dir="${fst.shared.obj.dir}" includes="*.${obj.suffixe}"  unless="transMode"/>
                <fileset dir="${xml.shared.obj.dir}" includes="*.${obj.suffixe}"  />
                <fileset dir="${ntm.shared.obj.dir}" includes="*.${obj.suffixe}"  unless="transMode"/>
                <linkerarg value="-liconv" location="start" if="mac"/>

                <fileset dir="${xip.shared.obj.dir}" includes="*.${obj.suffixe}"  />
                <!-- It's required to used the dynamic lib to be sure than python works fine, mainly with java + 
                            python script which imports a module (import math) -->
                <libset dir="${python.lib.dir}" libs="${python.lib.name}"  if="python" />
                <!-- to monitor memory usage: JXipResultManager::PrintMemoryInfo() -->
                <libset libs="psapi" if="windows" />
            </linker>
            <linker extends="linkerShared" name="${compilerCplus.name}" unless="allSrcXipCompiled" >
                <!-- used by someone without the source of xml, fst, ntm, xip compiled -->
                <libset dir="${xip.lib.dir}" libs="${xip.lib.name}" />
                <libset dir="${python.lib.dir}" libs="${python.lib.name}"  if="python" />
            </linker>
        </cc>
        <antcall target="staticLib" inheritall="true"/>
        <antcall target="genVS2005Manifest" >
            <param name="manifest.fileName" location="${lib.dir}/${libName}.dll.manifest" />
            <param name="libOrExe" value="${output.lib.name}.dll" />
            <param name="libOrExeDir" location="${lib.dir}" />
            <!-- #1 for an exe, #2 for a dll -->
            <param name="typeFile" value="#2" />
        </antcall>
        <record name="${log.dir}/cxxCompile.${modeName}" action="stop"/>
    </target>

    <!-- this static library will be build with files compiled with -fPIC so this library can be used to build
    a specific jxipLib.so or jXipLibPy.so and linked with some specific derivation code: see sample/derivation/derivation.cxx
    -->
    <target name="staticLib"  if="transMode" >
        <property name="libStatic.dir" location="${dist.dir}/staticLib/${platform-arch}" />
        <property name="libStatic" location="${libStatic.dir}/${libName}" />
        <mkdir dir="${libStatic.dir}" />
        <delete file="${libStatic}" />
        <cc name="${compilerCplus.name}" objdir="${obj.dir}" outtype="static"  outfile="${libStatic}" >
            <linker  name="${compilerCplus.name}"   >
                <fileset dir="${xml.shared.obj.dir}" includes="*.${obj.suffixe}"  />
                <fileset dir="${xip.shared.obj.dir}" includes="*.${obj.suffixe}" excludes="derivation.${obj.suffixe}" />
                <fileset dir="${obj.dir}" includes="*.${obj.suffixe}"  />
            </linker>
        </cc>
    </target>
 
    <target name="copyGenerate" depends="initAll" >
        <available file="${xip.home}/src/xipwin/generate.cpp" type="file" property="generate.cxx.isPresent"/>
        <if>
            <istrue value="${generate.cxx.isPresent}"/>
            <then>
                <copy file="${xip.home}/src/xipwin/generate.cpp" toFile="${cxx.source.dir}/generate.cxx" />
                <copy file="${xip.home}/src/xipwin/generate.h" toFile="${cxx.source.dir}/generate.h" />
                <!-- must be commited for other user (like Salah) who want to compile only java code,
                without make some updates from the xip sources -->
                <if>
                    <equals arg1="${platform}" arg2="linux" />
                    <then>
                        <cvs output="${log.dir}/cvs">
                            <commandline>
                                <argument value="commit"/>
                                <argument value="-m"/>
                                <argument value="autoCommit"/>
                                <argument value="${cxx.source.dir}/generate.cxx"/>
                                <argument value="${cxx.source.dir}/generate.h"/>
                            </commandline>
                        </cvs>
                    </then>
                </if>
            </then>
            <else>
                <!-- for a user who has not checkout xip/dev/src directory -->
                <cvs output="${log.dir}/cvs">
                    <commandline>
                        <argument value="-q"/>
                        <argument value="up"/>
                        <argument value="-d"/>
                        <!-- the path must be relative: absolute path are not working -->
                        <!-- generate.cxx generate.h,... are required -->
                        <argument value="src/cxx"/>
                        <!-- the xip include are also required -->
                        <argument value="../include"/>
                    </commandline>
                </cvs>
            </else>
        </if>
    </target>
    
    <!-- generate the native header file from the Java class -->
    <target name="nativeHeader" depends="initAll">
        <mkdir dir="${build.include.dir}" />
        <javah destdir="${build.include.dir}" classpath="${build.classes.dir}">
            <class name="${jxip.package}.JXip"/>
            <class name="${jxip.package}.JXip2"/>
            <class name="${jxip.package}.XipResultManager"/>
            <class name="${jxip.package}.RuleBuilder"/>
            <class name="${jxip.package}.RuleLoadedManager"/>
            <class name="${jxip.package}.RuleLoaded"/>
        </javah>
    </target>
    
    <!-- compile java source file -->
    <target name="javaCompile" depends="initAll">
        <record name="${log.dir}/javaCompile" loglevel="${log.level}" action="start"/>
        <mkdir dir="${build.classes.dir}" />
        <javac srcdir="${java.source.dir}/com/xerox/jxip" destdir="${build.classes.dir}"
               debug="${java_debug}" deprecation="on" optimize="${java_optimize}" >
            <include name="*.java"/>
            <compilerarg value="-Xlint"/>
        </javac>
        <record name="${log.dir}/javaCompile" loglevel="${log.level}" action="stop"/>
    </target>
    
    <target name="valgrind" depends="clean, jarDebug, runValgrind" />
        
    <target name="runValgrind" depends="initAll"
            description="compile mode debug  + launch valgrind + apply valgrind on xip java application (with callBack)   ">
        <echo message="*****************************************************************" />
        <echo message="run (dans .../xip/dev) ant compileAll.debug to compile xip,jxip,fst,xml,ntm in debug mode" />
        <echo message="*****************************************************************" />
        <record name="${log.dir}/valgrind" loglevel="${log.level}" action="start"/>
        <property name="log" value="res-memcheck-${platform-arch}-java.val" />
        <property name="grm" value="../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm" />
        <property name="input" value="input-1000" />
        <property name="cmdLine" value="java -Xmx512m -cp dist/jxip.jar:testAna/testMem1/build testMem1.Test ${grm} ${input}" />
        <property name="valgrindOptions1" 
                  value="-v --tool=memcheck --num-callers=20 --leak-check=full --show-reachable=yes --track-origins=yes --trace-children=yes" />
        <property name="valgrindOptions2" 
                  value ="--log-file=${log} --suppressions=supression.supp --error-limit=no " />
        <property name="valgrindOptions3" 
                  value="--gen-suppressions=all --demangle=yes" />
        <property name="valgrindOptions" 
                  value="${valgrindOptions1} ${valgrindOptions2} ${valgrindOptions3}" />
        <delete file="${log}" />
        <ant antfile="build.xml" dir="testAna/testMem1" target="compile" inheritAll="false" />
        <exec executable="/opt/ParSem/tools/valgrind/${valgrind.version}/${platform-arch}/bin/valgrind" dir="${basedir}">
            <arg line="${valgrindOptions}"/>
            <arg line="${cmdLine}"/>
        </exec>
        <record name="${log.dir}/valgrind" action="stop"/>
        <echo message="result in: ${log}" />
    </target>
        
    <!-- generate java doc -->
    <target name="doc" depends="initAll">
        <delete dir="${build.doc.dir}"/>
        <mkdir dir="${build.doc.dir}"/>
        <javadoc packagenames="${jxip.package}.JXip"
                 sourcepath="${java.source.dir}"
                 sourcefiles="
                 ${java.source.dir}/com/xerox/jxip/JXip.java,
                 ${java.source.dir}/com/xerox/jxip/Const.java,
                 ${java.source.dir}/com/xerox/jxip/XipResultManager.java,
                 ${java.source.dir}/com/xerox/jxip/XipResult.java,
                 ${java.source.dir}/com/xerox/jxip/XipUnit.java,
                 ${java.source.dir}/com/xerox/jxip/XipNode.java,
                 ${java.source.dir}/com/xerox/jxip/XipLeaf.java,
                 ${java.source.dir}/com/xerox/jxip/XipDependency.java,
                 ${java.source.dir}/com/xerox/jxip/XipFeature.java
                 "
                 destdir="${build.doc.dir}"
                 author="false"
                 version="xip-${xip.version}-${xip.build}"
                 use="false"
                 windowtitle="XIP java API"
        >
            <doctitle><![CDATA[<h1>XIP java API</h1>]]>
            </doctitle>
            <bottom><![CDATA[<i>Copyright &#169; 2008 Xerox All Rights Reserved.</i>]]>
            </bottom>
        </javadoc>
    </target>
    
    <target name="initAll" depends="setCommonProperties" unless="initAllSet">
        <property name="initAllSet"    value="true" />
        <mkdir dir="${dist.dir}" />
        <mkdir dir="${build.dir}" />
        <mkdir dir="${log.dir}" />
    </target>
        
    <target name="setCommonProperties" depends="platformSetting, getXipVersionAnBuild" unless="commonPropertiesSet">
        <property name="commonPropertiesSet"    value="true" />
        <property name="build.dir"         	location="${basedir}/build" />
        <property name="build.lib.dir"          location="${build.dir}/lib" />
        <property name="build.dir.platform"     location="${build.dir}/${platform-arch}" />
        <property name="fst.dir"                location="${basedir}/../../../fst/dev" />
        <property name="ntm.dir"                location="${basedir}/../../../ntm/dev" />
        <property name="lg.dir"                	location="${basedir}/../../../languageGuesser/dev" />
        <property name="xml.dir"               	location="${basedir}/../../../xml/dev" />
        <if>
            <istrue value="${debugMode}"/>
            <then>
                <property name="build.classes.dir"      location="${build.dir.platform}/classes.debug" />
                <property name="build.include.dir"      location="${build.dir.platform}/includeGenerated.debug" />
                <property name="xip.obj.dir"            location="${xip.home}/${fstOrTrans.dir}/obj/${platform-arch}.debug" />
            </then>
            <else>
                <property name="build.classes.dir"      location="${build.dir.platform}/classes" />
                <property name="build.include.dir"      location="${build.dir.platform}/includeGenerated" />
                <property name="xip.obj.dir"            location="${xip.home}/${fstOrTrans.dir}/obj/${platform-arch}" />
            </else>
        </if>
        <property name="build.doc.dir"	      	location="${build.dir}/doc" />
        <property name="build.obj.dir"          location="${build.dir.platform}/obj" />
        <property name="log.dir" 		location="${build.dir.platform}/log" />
        <if>
            <available file="${xip.obj.dir}" />
            <then>
                <property name="allSrcXipCompiled" value="true" />
            </then>
            <else>
                <!-- pour qu'un autre que GR puisse faire le build de jxip.jar en utilisant les libs xip deja construites
		Solution qui ne peut pas etre utilise pour la delivery (ne marche pas pour linux) -->
                <property name="xip.root.lib" location="/opt/ParSem/bin/xip-${xip.version}-${xip.build}/lib" />
                <echo message="+++++++++++++++ do not exist: ${xip.obj.dir}" />
                <echo message="+++++++++++++++ libUsed: ${xip.root.lib}" />
            </else>
        </if>
        <property name="transModeFile" location="${basedir}/../.transMode" />
        <if>
            <available file="${transModeFile}" />
            <then>
                <loadfile property="transModeF" srcFile="${transModeFile}"/>
                <if>
                    <istrue value="${transModeF}"/>
                    <then>
                        <!-- transMode is only set to true, or undefined (and never set to false) -->
                        <property name="transMode" value="true" />
                    </then>
                    <else>
                        <property name="fstMode" value="true" />
                    </else>
                </if>
            </then>
            <else>
                <property name="fstMode" value="true" />
            </else>
        </if>
    </target>
    
    <target name="localClean" depends="setCommonProperties" >
        <delete dir="${build.dir.platform}" />
        <delete dir="${build.lib.dir}/jXip/${platform-arch}" />
        <delete dir="${build.lib.dir}/jXip_debug/${platform-arch}" />
        <delete dir="${build.lib.dir}/jXipPy/${platform-arch}" />
    </target>
    
    <target name="localCleanAll" depends="setCommonProperties" >
        <delete dir="${build.dir}"/>
        <delete dir="${dist.dir}"/>
    </target>


    <!-- Install maven snapshot in local repository. (The task for
         deploying a release to the shared repository on lautaret is
         mvn.deploy in ../build.xml.) -->

    <property name="mvn.group.id" value="com.xerox.jxip"/>

    <target name="mvn.install" depends="jar">
        <property name="mvn.artifact.version" value="${xip.version}-${xip.build}-SNAPSHOT"/>
        <installfile file="dist/jxip.jar" id="jxip" packaging="jar"/>
        <installfile file="dist/jxipPy.jar" id="jxipPy" packaging="jar"/>
	<!-- would like to handle javadoc and src as well, so that
	     IDEs can find them automatically.  First I'd
	     have to jar them up. I see no clean way to have jxip.jar
	     and jxipPy.jar share the same src and doc jars. -->
    </target>

    <macrodef name="installfile">
        <attribute name="file"/>
        <attribute name="id"/>
        <attribute name="packaging"/>
        <sequential>
            <echo>file=@{file} id=@{id} packaging=@{packaging}</echo>
            <exec executable="mvn">
                <arg value="-B"/>
                <arg value="install:install-file"/>
                <arg value="-Dfile=@{file}"/>
                <arg value="-DgroupId=${mvn.group.id}"/>
                <arg value="-DartifactId=@{id}"/>
                <arg value="-Dversion=${mvn.artifact.version}"/>
                <arg value="-Dpackaging=@{packaging}"/>
            </exec>
        </sequential>
    </macrodef>

</project>


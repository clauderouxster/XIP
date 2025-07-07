echo on

del -rf Test.class output.xml errors.err

set GRMDIR=..\..\..\grammar\english\entity
set GRAM=%GRMDIR%\englishEntity.grm
set GKLSFILES=..\gklsFiles
set DB=%GKLSFILES%\references.xml
set SCRIPTSPATH=%GKLSFILES%\scripts
set INPUTDIR=%GKLSFILES%\input 
set JAR=..\..\dist\jxip.jar

javac -classpath %JAR% Test.java 

set SENTENCE="Peter is happy."
java  -classpath %JAR%;. Test %GRAM% %SCRIPTSPATH% %DB% %INPUTDIR% %SENTENCE%


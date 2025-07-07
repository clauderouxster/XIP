
set GRMDIR=../../../grammar/english/entity
set GRAM=%GRMDIR%\englishEntity.grm
set GKLSFILES=..\gklsFiles
set INPUTDIR=%GKLSFILES%\input
set JAR=..\..\dist\jxip.jar

java -classpath %JAR% com.xerox.jxip.InputFile %GRAM% %INPUTDIR%


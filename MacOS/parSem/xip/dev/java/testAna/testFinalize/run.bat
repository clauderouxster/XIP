@echo on
mkdir build
set jar=..\..\dist\jxip.jar;..\libExt\commons-io-2.0.1.jar
javac -cp %jar%  -d build Parser.java Test.java
java -Xmx512m -cp %jar%;build testFinalize.Test 

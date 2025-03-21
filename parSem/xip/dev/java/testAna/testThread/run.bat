echo on
mkdir build
del build\test\*.class
set jar=..\..\dist\jxip.jar;libExt\commons-io-2.0.1.jar
javac -cp %jar%  -d build src\test\ThreadT1.java
java -Xmx512m -cp %jar%;build test.ThreadT1 


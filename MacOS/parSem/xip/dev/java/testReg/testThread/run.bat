echo on
mkdir build
del build\testThread\*.class
set jar=../../dist/jxip.jar;../libExt/commons-io-2.0.1.jar
javac -cp %jar%  -d build Test.java
java -Xmx512m -cp %jar%;build testThread.Test 


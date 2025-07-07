echo on
mkdir build
del build\Test.class
javac -cp ../../dist/jxip.jar  -d build Test.java
java -Xmx512m -cp ../../dist/jxip.jar;build testDuration.Test 


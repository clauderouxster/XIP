echo on
mkdir build
if exist build\ldXipParser.class del build\XipParser.class
javac -d build -classpath lib\jxip.jar src\XipParser.java 
java -mx128m  -classpath lib\jxip.jar;build XipParser input.txt

#!/usr/bin/perl -w

open(F,$ARGV[0]);

print("<html>\n<head><title> th&egrave;mes </title></head>\n") ;

print("<BODY style=  \"line-height:160%\" BGCOLOR=\"#FFFFFF\" LINK=\"#A62234\">") ;
print("<FONT FACE=\"VERDANA\" SIZE=6><A NAME=\"in\">Les th&egrave;mes de chaque phrase</FONT>\n <BR><BR> <FONT FACE=\"VERDANA\" SIZE=4>");


while($line=<F>){

$line=~s/\n/<BR>/g;

$line=~s/<agenttopic>/<span style="background-color:#99FFFF"><font color=000099>/g;
$line=~s/<\/agenttopic>/<\/font><\/span>/g;

$line=~s/<tempstopic>/ <span style="background-color:#FFFF99"><font color=#FF6633>/g;
$line=~s/<\/tempstopic>/<\/font><\/span>/g;

$line=~s/<sourcetopic>/<span style="background-color:#CCCCCC">/g;
$line=~s/<\/sourcetopic>/<\/span>/g;

#$line=~s/<noyauinf>/<span style="border-bottom:solid #000000">/g;
#$line=~s/<\/noyauinf>/<\/span>/g;

$line=~s/<noyautopic>/<b><span style="background-color:#FFCCCC"><font color=#FF0000>/g;
$line=~s/<\/noyautopic>/<\/font><\/span><\/b>/g;

$line=~s/<extensiontopic>/<span style="background-color:#FFCCCC"><font color=#FF0000>/g;
$line=~s/<\/extensiontopic>/<\/font><\/span>/g;

$line=~s/<lieutopic>/<span style="background-color:#CCFFCC"><font color=#006600>/g;
$line=~s/<\/lieutopic>/<\/font><\/span>/g;

$line=~s/<cibletopic>/<span style="background-color:#DDAAFF"><font color=#6600CC>/g;
$line=~s/<\/cibletopic>/<\/font><\/span>/g;


$line=~s/�/&Agrave;/g;
$line=~s/�/&agrave;/g;
$line=~s/�/&Aacute;/g;
$line=~s/�/&aacute;/g;
$line=~s/�/&Acirc;/g;
$line=~s/�/&acirc;/g;
$line=~s/�/&Atilde;/g;
$line=~s/�/&atilde;/g;
$line=~s/�/&Auml;/g;
$line=~s/�/&auml;/g;
$line=~s/�/&Aring;/g;
$line=~s/�/&aring;/g;
$line=~s/�/&Aelig;/g;
$line=~s/�/&aelig;/g;
$line=~s/�/&Ccedil;/g;
$line=~s/�/&ccedil;/g;
$line=~s/�/&Egrave;/g;
$line=~s/�/&egrave;/g;
$line=~s/�/&Eacute;/g;
$line=~s/�/&eacute;/g;
$line=~s/�/&Ecirc;/g;
$line=~s/�/&ecirc;/g;
$line=~s/�/&Euml;/g;
$line=~s/�/&euml;/g;
$line=~s/�/&Igrave;/g;
$line=~s/�/&igrave;/g;
$line=~s/�/&Iacute;/g;
$line=~s/�/&iacute;/g;
$line=~s/�/&Icirc;/g;
$line=~s/�/&icirc;/g;
$line=~s/�/&Iuml;/g;
$line=~s/�/&iuml;/g;
$line=~s/�/&ETH;/g;
$line=~s/�/&eth;/g;
$line=~s/�/&Ntilde;/g;
$line=~s/�/&ntilde;/g;
$line=~s/�/&Ograve;/g;
$line=~s/�/&ograve;/g;
$line=~s/�/&Oacute;/g;
$line=~s/�/&oacute;/g;
$line=~s/�/&Ocirc;/g;
$line=~s/�/&ocirc;/g;
$line=~s/�/&Otilde;/g;
$line=~s/�/&otilde;/g;
$line=~s/�/&Ouml;/g;
$line=~s/�/&ouml;/g;
$line=~s/�/&times;/g;
$line=~s/�/&divide;/g;
$line=~s/�/&Oslash;/g;
$line=~s/�/&oslash;/g;
$line=~s/�/&Ugrave;/g;
$line=~s/�/&ugrave;/g;
$line=~s/�/&Uacute;/g;
$line=~s/�/&uacute;/g;
$line=~s/�/&Ucirc;/g;
$line=~s/�/&ucirc;/g;
$line=~s/�/&Uuml;/g;
$line=~s/�/&uuml;/g;
$line=~s/�/&Yacute;/g;
$line=~s/�/&yacute;/g;
$line=~s/�/&THORN;/g;
$line=~s/�/&thorn;/g;
$line=~s/�/&szlig;/g;
$line=~s/�/&yuml;/g;

print $line ;
}
print("</html>\n\n") ;


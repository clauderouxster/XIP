#!/usr/bin/perl -w

open(F,$ARGV[0]);

print("<html>\n<head><title> &eacute;v&eacute;nements </title></head>\n") ;

print("<BODY style=  \"line-height:160%\" BGCOLOR=\"#FFFFFF\" LINK=\"#A62234\">") ;
print("<FONT FACE=\"VERDANA\" SIZE=6><A NAME=\"in\">Tous les &eacute;v&eacute;nements</FONT>\n <BR><BR> <FONT FACE=\"VERDANA\" SIZE=4>");

while($line=<F>){

$line=~s/\n/<BR>/g;

# à décommenter si on veut surligner aussi les extensions
#$line=~s/<extension>/<span style="background-color:#FFCCCC"><font color=#FF0000>/g;
#$line=~s/<\/extension>/<\/font><\/span>/g;

$line=~s/<noyaueve>/<b><span style="background-color:#FFCCCC"><font color=#FF0000>/g;
$line=~s/<\/noyaueve>/<\/font><\/span><\/b>/g;


$line=~s/À/&Agrave;/g;
$line=~s/à/&agrave;/g;
$line=~s/Á/&Aacute;/g;
$line=~s/á/&aacute;/g;
$line=~s/Â/&Acirc;/g;
$line=~s/â/&acirc;/g;
$line=~s/Ã/&Atilde;/g;
$line=~s/ã/&atilde;/g;
$line=~s/Ä/&Auml;/g;
$line=~s/ä/&auml;/g;
$line=~s/Å/&Aring;/g;
$line=~s/å/&aring;/g;
$line=~s/Æ/&Aelig;/g;
$line=~s/æ/&aelig;/g;
$line=~s/Ç/&Ccedil;/g;
$line=~s/ç/&ccedil;/g;
$line=~s/È/&Egrave;/g;
$line=~s/è/&egrave;/g;
$line=~s/É/&Eacute;/g;
$line=~s/é/&eacute;/g;
$line=~s/Ê/&Ecirc;/g;
$line=~s/ê/&ecirc;/g;
$line=~s/Ë/&Euml;/g;
$line=~s/ë/&euml;/g;
$line=~s/Ì/&Igrave;/g;
$line=~s/ì/&igrave;/g;
$line=~s/Í/&Iacute;/g;
$line=~s/í/&iacute;/g;
$line=~s/Î/&Icirc;/g;
$line=~s/î/&icirc;/g;
$line=~s/Ï/&Iuml;/g;
$line=~s/ï/&iuml;/g;
$line=~s/Ð/&ETH;/g;
$line=~s/ð/&eth;/g;
$line=~s/Ñ/&Ntilde;/g;
$line=~s/ñ/&ntilde;/g;
$line=~s/Ò/&Ograve;/g;
$line=~s/ò/&ograve;/g;
$line=~s/Ó/&Oacute;/g;
$line=~s/ó/&oacute;/g;
$line=~s/Ô/&Ocirc;/g;
$line=~s/ô/&ocirc;/g;
$line=~s/Õ/&Otilde;/g;
$line=~s/õ/&otilde;/g;
$line=~s/Ö/&Ouml;/g;
$line=~s/ö/&ouml;/g;
$line=~s/×/&times;/g;
$line=~s/÷/&divide;/g;
$line=~s/Ø/&Oslash;/g;
$line=~s/ø/&oslash;/g;
$line=~s/Ù/&Ugrave;/g;
$line=~s/ù/&ugrave;/g;
$line=~s/Ú/&Uacute;/g;
$line=~s/ú/&uacute;/g;
$line=~s/Û/&Ucirc;/g;
$line=~s/û/&ucirc;/g;
$line=~s/Ü/&Uuml;/g;
$line=~s/ü/&uuml;/g;
$line=~s/Ý/&Yacute;/g;
$line=~s/ý/&yacute;/g;
$line=~s/Þ/&THORN;/g;
$line=~s/þ/&thorn;/g;
$line=~s/ß/&szlig;/g;
$line=~s/ÿ/&yuml;/g;

print $line ;
}
print("</html>\n\n") ;


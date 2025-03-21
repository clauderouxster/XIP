#!/opt/STools/bin/perl -w

open(F,$ARGV[0]);

print("<html>\n<head><title> EVENTS </title></head>\n") ;

print("<BODY style=  \"line-height:160%\" BGCOLOR=\"#FFFFFF\" LINK=\"#A62234\">") ;
print("<FONT FACE=\"VERDANA\" SIZE=6><A NAME=\"in\">Events</FONT>\n <BR><BR> <FONT FACE=\"VERDANA\" SIZE=4>");


while($line=<F>){

$line=~s/\n/<BR>/g;

$line=~s/<agent>/<span style="background-color:#99FFFF"><font color=000099>/g;
$line=~s/<\/agent>/<\/font><\/span>/g;

$line=~s/<temps>/ <span style="background-color:#FFFF99"><font color=#FF6633>/g;
$line=~s/<\/temps>/<\/font><\/span>/g;

$line=~s/<source>/<span style="background-color:#CCCCCC">/g;
$line=~s/<\/source>/<\/span>/g;

$line=~s/<noyauinf>/<span style="border-bottom:solid #000000">/g;
$line=~s/<\/noyauinf>/<\/span>/g;

$line=~s/<noyautopic>/<b><span style="background-color:#FFCCCC"><font color=#FF0000>/g;
$line=~s/<\/noyautopic>/<\/font><\/span><\/b>/g;

$line=~s/<extension>/<span style="background-color:#FFCCCC"><font color=#FF0000>/g;
$line=~s/<\/extension>/<\/font><\/span>/g;

$line=~s/<lieu>/<span style="background-color:#CCFFCC"><font color=#006600>/g;
$line=~s/<\/lieu>/<\/font><\/span>/g;

$line=~s/<cible>/<span style="background-color:#DDAAFF"><font color=#6600CC>/g;
$line=~s/<\/cible>/<\/font><\/span>/g;


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
$line=~s/Ğ/&ETH;/g;
$line=~s/ğ/&eth;/g;
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
$line=~s/İ/&Yacute;/g;
$line=~s/ı/&yacute;/g;
$line=~s/Ş/&THORN;/g;
$line=~s/ş/&thorn;/g;
$line=~s/ß/&szlig;/g;
$line=~s/ÿ/&yuml;/g;

print $line ;
}
print("</html>\n\n") ;


#!/opt/STools/bin/perl -w

open(F,$ARGV[0]) ;

print ("<html>\n<head><title> tableau </title></head>\n") ;

print ("<BODY style=  \"line-height:160%\" BGCOLOR=\"#FFFFFF\" LINK=\"#A62234\">") ;


$i=0;
#lecture du fichier sortie de xip
while($line=<F>)
{
if($line=~ /TABLEAU.*\(([^,]+),([^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+),*(.*)\)/)
	{
#on récupère la source, le noyau, l'acteur, la cible, les lieux, la date et l'offset du noyau de chaque événement
$fact=$1;
$coordonnees=$2;
$reste=$3;

@coord=split(/,/,$coordonnees);
$source=$coord[0];
$noyau=$coord[2];
$acteur=$coord[3];
$typeacteur=$coord[4];
$cible=$coord[5];
$typecible=$coord[6];
$lieu1=$coord[7];
$lieu2=$coord[8];
$date=$coord[9];

if ($noyau=~ /:\[([\d]+)-[\d]+\]/)
	{$positionNOY=$1; }

$fact=~s/:\[[-,\d]+\]//g;
$source=~s/:\[[-,\d]+\]//g;
$acteur=~s/:\[[-,\d]+\]//g;
$cible=~s/:\[[-,\d]+\]//g;
$lieu1=~s/:\[[-,\d]+\]//g;
$lieu2=~s/:\[[-,\d]+\]//g;
$date=~s/:\[[-,\d]+\]//g;
$typecible=~s/:\[[-,\d]+\]//g;
$typeacteur=~s/:\[[-,\d]+\]//g;

if ($lieu2 eq "0")
	{
$lieu = $lieu1;
	}
else
	{
$lieu = $lieu1.", ".$lieu2;
	}

#on récupère les extensions et on les intègre au noyau
@ext=split(/,/,$reste);
#print "\n"."l'ensemble des extensions : ".$reste."\n";
#print "la liste : ".$#ext."\n";
if ($reste ne "")
{
#print "la première extension : ".$ext[0]."\n";
if ($#ext==0)
		{
	$extension=$ext[0];
	if ($extension=~ /:\[([\d]+)-[\d]+\]/)
		{$positionEXT=$1; 

		if ($positionEXT<$positionNOY)
		{$noyau=$extension." ".$noyau;}
		elsif ($positionEXT>$positionNOY)
		{$noyau=$noyau." ".$extension;}

		}

	}

elsif ($#ext==1)
	{
	$extension1=$ext[0];
	$extension2=$ext[1];
	if ($extension1=~ /:\[([\d]+)-[\d]+\]/)
		{$position1EXT=$1; }
	if ($extension2=~ /:\[([\d]+)-[\d]+\]/)
		{$position2EXT=$1; }
	if ($position1EXT<$positionNOY)
		{
		if($positionNOY<$position2EXT)
			{$noyau=$extension1." ".$noyau." ".$extension2;}
		elsif($positionNOY>$position2EXT)
			{
			if($position1EXT<$position2EXT)
				{$noyau=$extension1." ".$extension2." ".$noyau;}
			elsif($position1EXT>$position2EXT)
				{$noyau=$extension2." ".$extension1." ".$noyau;}
			}
		}
	elsif ($positionNOY<$position1EXT)
		{
		if($positionNOY>$position2EXT)
			{$noyau=$extension2." ".$noyau." ".$extension1;}
		elsif($positionNOY<$position2EXT)
			{
			if($position1EXT<$position2EXT)
				{$noyau=$noyau." ".$extension1." ".$extension2;}
			elsif($position1EXT>$position2EXT)
				{$noyau=$noyau." ".$extension2." ".$extension1;}
			}
		}
	}
}
$noyau=~s/:\[[-,\d]+\]//g;

#on met l'événement dans la liste des événements
%evenement=("source"=>$source,"noyau"=>$noyau,"acteur"=>$acteur,"cible"=>$cible,"lieu"=>$lieu,"date"=>$date,"offset"=>$positionNOY,"factualite"=>$fact,"typeActeur"=>$typeacteur,"typeCible"=>$typecible);

$i++;
if($i==1)
	{@liste=({%evenement});}
else
	{push(@liste,{%evenement});}


}	
}

#affichage du tableau

if($i>0)
	{
print "\n<table border=1>\n";
print "\n<b><tr align=center><th>source</th><th>factualit&eacute;</th><th>acteur</th><th>noyau</th><th>cible</th><th>lieu</th><th>date</th></tr></b>";

for($j=0;$j<=$#liste;$j++){
$couleur="";
if ($liste[$j]{factualite} eq "F")
{$couleur=" bgcolor=#CCEECC"}
print "\n<tr align=center".$couleur."><td bgcolor=#AAAAAA>".$liste[$j]{source}."</td><td>".$liste[$j]{factualite}."</td><td>".$liste[$j]{acteur}."</td><td>".$liste[$j]{noyau}."</td><td>".$liste[$j]{cible}."</td><td>".$liste[$j]{lieu}."</td><td>".$liste[$j]{date}."</td></tr>";


			}
	

print "\n</table>";
	}


print ("\n</body></html>\n\n") ;


close F;



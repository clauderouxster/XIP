#!/opt/STools/bin/perl -w

open(F,$ARGV[0]) ;
$evetableau=$ARGV[1];

$adresse=creer();
$file=$ARGV[2]."/tableau_$evetableau.html";
open(F4, ">$file") ;
#open(F4, ">tableau_$evetableau.html") ;
print F4 "<html>\n<head><title> tableau </title></head>\n" ;
print F4 "<BODY style=  \"line-height:160%\" BGCOLOR=\"#FFFFFF\" LINK=\"#A62234\">" ;

if($adresse ne "0")
	{
print F4 "\n<table border=1>\n";
print F4 "\n<b><tr align=center><th>locuteur</th><th>factualit&eacute;</th><th>acteur</th><th>noyau</th><th>cible</th><th>lieu</th><th>date</th><th>source</th><th>date du document</th></tr></b>";

for($j=0;$j<=$#tableau;$j++){
$couleur="";
if ($tableau[$j]{factualite} eq "F")
{$couleur=" bgcolor=#CCEECC"}
print F4 "\n<tr align=center".$couleur."><td bgcolor=#AAAAAA>".$tableau[$j]{locuteur}."</td><td>".$tableau[$j]{factualite}."</td><td>".$tableau[$j]{acteur}."</td><td>".$tableau[$j]{noyau}."</td><td>".$tableau[$j]{cible}."</td><td>".$tableau[$j]{lieu}."</td><td>".$tableau[$j]{date}."</td><td>".$tableau[$j]{source}."</td><td>".$tableau[$j]{datedocument}."</td></tr>";
			}
	

print F4 "\n</table>";
	}

print F4 "\n</body></html>\n\n" ;
close F4;


close F;


################################

sub creer{
    if ($evetableau eq "topic"){$trait="_TOPIC";}
   elsif ($evetableau eq "factual"){$trait="_FACTUAL";}
   elsif ($evetableau eq "information"){$trait="_INFORMATION";}
   elsif ($evetableau eq "all"){$trait="";}
    else {$trait="";}
$i=0;
#lecture du fichier sortie de xip
while($line=<F>)
{
if($line=~ /TABLEAU[^\(]*$trait[^\(]*\(([^,]+),([^,]+),([^,]+),([^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+),*(.*)\)/)
	{
#on récupère la source, le noyau, l'acteur, la cible, les lieux, la date et l'offset du noyau de chaque événement
$source=$1;
$datedocument=$2;
$fact=$3;
$coordonnees=$4;
$reste=$5;

@coord=split(/,/,$coordonnees);
$locuteur=$coord[0];
$noyau=$coord[2];
$acteur=$coord[3];
#$typeacteur=$coord[4];
$cible=$coord[4];
#$typecible=$coord[6];
$lieu=$coord[5];
#$lieu2=$coord[8];
$date=$coord[6];

if ($noyau=~ /:\[([\d]+)-[\d]+\]/)
	{$positionNOY=$1; }

$source=~s/:\[[-,\d]+\]//g;
$datedocument=~s/:\[[-,\d]+\]//g;
$fact=~s/:\[[-,\d]+\]//g;
$locuteur=~s/:\[[-,\d]+\]//g;
$acteur=~s/:\[[-,\d]+\]//g;
$cible=~s/:\[[-,\d]+\]//g;
$lieu=~s/:\[[-,\d]+\]//g;
#$lieu2=~s/:\[[-,\d]+\]//g;
$date=~s/:\[[-,\d]+\]//g;
#$typecible=~s/:\[[-,\d]+\]//g;
#$typeacteur=~s/:\[[-,\d]+\]//g;

#if ($lieu2 eq "0")
#	{
#$lieu = $lieu1;
#	}
#else
#	{
#$lieu = $lieu1.", ".$lieu2;
#	}

#on récupère les extensions et on les intègre au noyau
@ext=split(/,/,$reste);
if ($reste ne "")
{
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
%evenement=("locuteur"=>$locuteur,"noyau"=>$noyau,"acteur"=>$acteur,"cible"=>$cible,"lieu"=>$lieu,"date"=>$date,"offset"=>$positionNOY,"factualite"=>$fact,"source"=>$source,"datedocument"=>$datedocument);
$i++;
if($i==1)
	{@tableau=({%evenement});}
else
	{push(@tableau,{%evenement});}


}	
}
if ($i==0)
{return 0;}
else {return \%tableau;}
}



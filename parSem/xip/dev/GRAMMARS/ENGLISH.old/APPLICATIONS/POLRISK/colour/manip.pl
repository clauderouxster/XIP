#!/usr/bin/perl -w

open(F,$ARGV[0]) ;
open(F2,$ARGV[1]) ;
$evetableau=$ARGV[2];

$file=$ARGV[3]."/tableaux_$evetableau.xml";
open(F3, ">$file") ;

$cloc=0 ; $toTreat=0 ;
$tag="" ;
while($line=<F>){

  if($line=~"NOYAU_INFORMATION") {
    $tag = "noyauinf" ;
    $toTreat=1 ;
  }
  if(($line=~"AGENT_TOPIC")) {
    $tag = "agenttopic" ; 
    $toTreat=1 ;
  }
  if($line=~"CIBLE_TOPIC") { 
    $tag = "cibletopic" ; 
    $toTreat=1 ;
  }              
  if($line=~"NOYAU_TOPIC") {
    $tag = "noyautopic" ;
    $toTreat=1 ;
  }
  if($line=~"LIEU_TOPIC") {
    $tag = "lieutopic" ;
    $toTreat=1 ;
  }
  if($line=~"TEMPS_TOPIC") {
    $tag = "tempstopic" ;
    $toTreat=1 ;
  }
  if($line=~"SOURCE_TOPIC") {
   $tag = "sourcetopic" ;
    $toTreat=1 ;
  }
  if($line=~"EXTENSION_TOPIC") {
    $tag = "extensiontopic" ;
    $toTreat=1 ;
  }
  if(($line=~"AGENT_FACTUAL")) { 
    $tag = "agentfactual" ; 
    $toTreat=1 ;
  }
  if($line=~"CIBLE_FACTUAL") { 
    $tag = "ciblefactual" ; 
    $toTreat=1 ;
  }              
  if($line=~"NOYAU_FACTUAL") {
    $tag = "noyaufactual" ;
    $toTreat=1 ;
  }
  if($line=~"LIEU_FACTUAL") {
    $tag = "lieufactual" ;
    $toTreat=1 ;
  }
  if($line=~"TEMPS_FACTUAL") {
    $tag = "tempsfactual" ;
    $toTreat=1 ;
  }
  if($line=~"SOURCE_FACTUAL") {
   $tag = "sourcefactual" ;
    $toTreat=1 ;
  }
  if($line=~"EXTENSION_FACTUAL") {
    $tag = "extensionfactual" ;
    $toTreat=1 ;
  }
   if($line=~"CORE-VERB") {
    $tag = "noyaueve" ;
    $toTreat=1 ;
  }
   if($line=~"CORE-NOUN") {
    $tag = "noyaueve" ;
    $toTreat=1 ;
  }
    if($line=~"CORE-ADJ") {
    $tag = "noyaueve" ;
    $toTreat=1 ;
  }                  
  if($toTreat==1){
    @tmp1 = split(/\[/,$line)  ;
    $offset1= $tmp1[1] ;
    @tmp2= split(/]/,$line)  ;
  if($#tmp2>1){
    $offset2= $tmp2[$#tmp2-1] ;}else{$offset2= $tmp2[0] ;}
    @coco1=split(/-/,$offset1);
    $init=$coco1[0] ;
    @coco2=split(/-/,$offset2);
    $end=$coco2[$#coco2] ;
    $triple =  $init . "-" .  $end  . "-" . $tag  ;
    $offset[$cloc++] = $triple ;
}
$toTreat=0 ;
$tag="" ;
}

$total="";
while($line=<F2>){
  $total = $total . $line ;
}
 @allchar = split(//,$total) ;
$compte=0;

for($i=1;$i<$#allchar;$i++)
	{
	if ($allchar[$i-1] eq "\n" && $allchar[$i] eq "\n")
  		{
		if ($compte==0)
		{@paragraphes=($i);
		$compte=1;}
		else
			{if ($allchar[$i-2] ne "\n")
				{push(@paragraphes,$i);}
			}
		}
	}
if (not($#paragraphes==$i-1) && not($#paragraphes==$i))
{push(@paragraphes,$i);}
close F;
open(F,$ARGV[0]) ;

$adresse=creer();

$toprint = "" ;
foreach $elem (@offset){
@separate =split(/-/,$elem);
$init=$separate[0] ;
$fin=$separate[1] - 1 ;
$tag= $separate[2];

#print STDOUT "\ntriple : ".$elem."_n";
#print STDOUT "\ntag : ".$tag." = ".$init."-".$fin."\n";

for($i=0;$i<=$#allchar;$i++){
  if($i==$init) {
    $toprint = " <" .$tag . ">" . $allchar[$i] ;
    $allchar[$i] =$toprint ;

  }
  if($i==$fin) {
     $toprint =  $allchar[$i]  . "</" . $tag . ">"  ;
     $allchar[$i] =$toprint ;
   }
}
}
foreach $elem (@allchar)
	{
  print $elem ;
	}


# ajouter les tableaux dans le texte : créer tableaux.xml
if ($adresse ne "0")
{
$debut=0;
$premiere=1;
$ajout="";
for($i=0;$i<=$#paragraphes;$i++)
	{
$finparagraphe=$paragraphes[$i];
for($j=0;$j<=$#tableau;$j++)
		{$couleur="";
		if ($tableau[$j]{offset}>$finparagraphe)
		{last;}
		if ($tableau[$j]{offset}>$debut && $tableau[$j]{offset}<$paragraphes[$i])
			{
			if ($premiere==1)
				{
				$ajout="<table border=1>\n<b><tr align=center><th>locuteur</th><th>factualit&eacute;</th><th>acteur</th><th>type d'&eacute;v&eacute;nement</th><th>cible</th><th>lieu</th><th>date</th></tr></b>";
				$premiere=0;
				}

			if ($tableau[$j]{factualite} eq "F")
			{$couleur=" bgcolor=#CCEECC"}
			$ajout=$ajout."<tr align=center".$couleur."><td bgcolor=#AAAAAA>".$tableau[$j]{locuteur}."</td><td>".$tableau[$j]{factualite}."</td><td>".$tableau[$j]{acteur}."</td><td>".$tableau[$j]{noyau}."</td><td>".$tableau[$j]{cible}."</td><td>".$tableau[$j]{lieu}."</td><td>".$tableau[$j]{date}."</td></tr>";
	}
		}

if ($premiere==0)
	{$ajout=$ajout."\n</table>";}
$allchar[$finparagraphe-1]=$allchar[$finparagraphe-1].$ajout."<BR>";
$debut=$paragraphes[$i];
$ajout="";
$premiere=1;
	}
	
foreach $elem (@allchar)
	{
print F3 $elem ;
	}
}

close F3;

#créer le tableau seul = créer tableau.html
$file1=$ARGV[3]."/tableau_$evetableau.html";
open(F4, ">$file1") ;
print F4 "<html>\n<head><title> tableau </title></head>\n" ;
print F4 "<BODY style=  \"line-height:160%\" BGCOLOR=\"#FFFFFF\" LINK=\"#A62234\">" ;

if($adresse ne "0")
	{
print F4 "\n<table border=1>\n";
print F4 "\n<b><tr align=center><th>locuteur</th><th>factualit&eacute;</th><th>acteur</th><th>noyau</th><th>cible</th><th>lieu</th><th>date</th></tr></b>";

for($j=0;$j<=$#tableau;$j++){
$couleur="";
if ($tableau[$j]{factualite} eq "F")
{$couleur=" bgcolor=#CCEECC"}
print F4 "\n<tr align=center".$couleur."><td bgcolor=#AAAAAA>".$tableau[$j]{locuteur}."</td><td>".$tableau[$j]{factualite}."</td><td>".$tableau[$j]{acteur}."</td><td>".$tableau[$j]{noyau}."</td><td>".$tableau[$j]{cible}."</td><td>".$tableau[$j]{lieu}."</td><td>".$tableau[$j]{date}."</td></tr>";
			}
	

print F4 "\n</table>";
	}

print F4 "\n</body></html>\n\n" ;
close F4;

############################
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
$cible=$coord[4];
$lieu=$coord[5];
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
$date=~s/:\[[-,\d]+\]//g;


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




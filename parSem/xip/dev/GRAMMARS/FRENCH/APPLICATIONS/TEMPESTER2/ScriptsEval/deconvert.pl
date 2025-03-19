#!/opt/gnu/bin/perl -w

## Auteur Caroline Brun
## ESTER: deconversion d'un fichier .stm  en format stm-ne 
## d'un fichier stm(.xml) analyse par XIP
## le format de sortie est celui de l'evaluation
## Les balises XML des EN sont mises au format ESTER
## Les balises intermédiaires crées pour l'analyse
## sont supprimées


open(F,$ARGV[0]);

while($line=<F>){
	# conversion des balises
	$line=~s/<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>//;
  	$line=~s/<\?xml.*>\n//;
	$line=~s/<stm>//;
	$line=~s/<\/stm>//;

	$line=~s/<personne-hum>/\[pers.hum /g;
	$line=~s/<\/personne-hum>/ \]/g;

	$line=~s/<personne-animal>/\[pers.anim /g;
	$line=~s/<\/personne-animal>/ \]/g;

	$line=~s/<prod-award>/\[prod.award /g;
	$line=~s/<\/prod-award>/ \]/g;

	$line=~s/<prod-vehicule>/\[prod.vehicule /g;
	$line=~s/<\/prod-vehicule>/ \]/g;
	$line=~s/<prod-doc>/\[prod.doc /g;
	$line=~s/<\/prod-doc>/ \]/g;
	$line=~s/<prod-art>/\[prod.art /g;
	$line=~s/<\/prod-art>/ \]/g;
	$line=~s/<prod>/\[prod /g;
	$line=~s/<\/prod>/ \]/g;
	

	### loc

	$line=~s/<pays>/\[loc.admi /g;
	$line=~s/<\/pays>/ \]/g;
	$line=~s/<region>/\[loc.admi /g;
	$line=~s/<\/region>/ \]/g;
	$line=~s/<continent>/\[loc.admi /g;
	$line=~s/<\/continent>/ \]/g;
	$line=~s/<ville>/\[loc.admi /g;
	$line=~s/<\/ville>/ \]/g;

	$line=~s/<lieu-batiment>/\[loc.fac /g;
	$line=~s/<\/lieu-batiment>/ \]/g;

	$line=~s/<lieu-geo>/\[loc.geo /g;
	$line=~s/<\/lieu-geo>/ \]/g;

	$line=~s/<lieu-addr-elec>/\[loc.addr.elec /g;
	$line=~s/<\/lieu-addr-elec>/ \]/g;

	$line=~s/<loc-tel>/\[loc.addr.tel /g;
	$line=~s/<\/loc-tel>/ \]/g;

	$line=~s/<loc-line>/\[loc.line /g;
	$line=~s/<\/loc-line>/ \]/g;

	$line=~s/<loc-adrpost>/\[loc.addr.post /g;
	$line=~s/<\/loc-adrpost>/ \]/g;

	$line=~s/<loc-addr-elec>/\[loc.addr.elec /g;
	$line=~s/<\/loc-addr-elec>/ \]/g;

	$line=~s/<lieu>/\[loc /g;
	$line=~s/<\/lieu>/ \]/g;

	## orgs

	$line=~s/<org-com>/\[org.com /g;
	$line=~s/<\/org-com>/ \]/g;

	$line=~s/<org-edu>/\[org.edu /g;
	$line=~s/<\/org-edu>/ \]/g;

	$line=~s/<org-pol>/\[org.pol /g;
	$line=~s/<\/org-pol>/ \]/g;

 	$line=~s/<org-non-profit>/\[org.non-profit /g;
	$line=~s/<\/org-non-profit>/ \]/g;

	$line=~s/<org-gsp>/\[org.gsp /g;
	$line=~s/<\/org-gsp>/ \]/g;

	$line=~s/<org-div>/\[org.div /g;
	$line=~s/<\/org-div>/ \]/g;

	$line=~s/<org>/\[org /g;
	$line=~s/<\/org>/ \]/g;

	## amounts

	$line=~s/<amount-phy-age>/\[amount.phy.age /g;
	$line=~s/<\/amount-phy-age>/ \]/g;
	$line=~s/<amount-phy-len>/\[amount.phy.len /g;
	$line=~s/<\/amount-phy-len>/ \]/g;
	$line=~s/<amount-phy-weight>/\[amount.phy.wei /g;
	$line=~s/<\/amount-phy-weight>/ \]/g;
	$line=~s/<amount-phy-surf>/\[amount.phy.area /g;
	$line=~s/<\/amount-phy-surf>/ \]/g;
	$line=~s/<amount-phy-vol>/\[amount.phy.vol /g;
	$line=~s/<\/amount-phy-vol>/ \]/g;
	$line=~s/<amount-phy-vit>/\[amount.phy.spd /g;
	$line=~s/<\/amount-phy-vit>/ \]/g;
	$line=~s/<amount-phy-temp>/\[amount.phy.temp /g;
	$line=~s/<\/amount-phy-temp>/ \]/g;
	$line=~s/<amount-money>/\[amount.cur /g;
	$line=~s/<\/amount-money>/ \]/g;

	## fonctions 

	$line=~s/<fonc-mil>/\[fonc.mil /g;
	$line=~s/<\/fonc-mil>/ \]/g;
	$line=~s/<fonc-rel>/\[fonc.rel /g;
	$line=~s/<\/fonc-rel>/ \]/g;
	$line=~s/<fonc-pol>/\[fonc.pol /g;
	$line=~s/<\/fonc-pol>/ \]/g;
	$line=~s/<fonc-admi>/\[fonc.admi /g;
	$line=~s/<\/fonc-admi>/ \]/g;
	$line=~s/<fonc-ari>/\[fonc.ari /g;
	$line=~s/<\/fonc-ari>/ \]/g;
	$line=~s/<fonc>/\[fonc /g;
	$line=~s/<\/fonc>/ \]/g;

	## expressions temporelles 
	
	$line=~s/<duree>/\[amount.phy.dur /g;
	$line=~s/<\/duree>/ \]/g;
	
	$line=~s/<heure>/\[time.hour /g;
	$line=~s/<\/heure>/ \]/g;

	$line=~s/<heurepot>/\[time.hour /g;
	$line=~s/<\/heurepot>/ \]/g;

	$line=~s/<date-absolue>/\[time.date.abs /g;
	$line=~s/<\/date-absolue>/ \]/g;

	$line=~s/<date-relative>/\[time.date.rel /g;
	$line=~s/<\/date-relative>/ \]/g;

	$line=~s/<datepot-absolue>/\[time.date.abs /g;
	$line=~s/<\/datepot-absolue>/ \]/g;

	$line=~s/<datepot-relative>/\[time.date.rel /g;
	$line=~s/<\/datepot-relative>/ \]/g;

	$line=~s/<date>/\[time.date /g;
	$line=~s/<\/date>/ \]/g;

	$line=~s/<datepot>/\[time.date /g;
	$line=~s/<\/datepot>/ \]/g;

	## non couvertes et a effacer
	
	$line=~s/<pheno-meteo>//g;
	$line=~s/<\/pheno-meteo>//g;

	$line=~s/<dureemod>//g;
	$line=~s/<\/dureemod>//g;
	$line=~s/<datemod>//g;
	$line=~s/<\/datemod>//g;

	$line=~s/<evenement[^>]*>//g;
	$line=~s/<\/evenement[^>]*>//g;

	$line=~s/<percent>//g;
	$line=~s/<\/percent>*>//g;

	## effacage du balisage intermediaire
	$line=~s/ <euh\/> / euh /g;
	$line=~s/<star\/> /\*/g;

	$line=~s/<mico\/>/\[mic-\]/g;
	$line=~s/<micf\/>/\[-mic\]/g;
	$line=~s/<mic\/>/\[-mic\]/g;
	$line=~s/<i\/>/\[i\]/g;
	$line=~s/<n\/>/\[n\]/g;
	$line=~s/<b\/>/\[b\]/g;
	$line=~s/<bb\/>/\[bb\]/g;
	$line=~s/<bf\/>/\[-b\]/g;
	$line=~s/<bi\/>/\[b-\]/g;
	$line=~s/<tx\/>/\[tx\]/g;
	$line=~s/<top>/\[top\]/g;
	$line=~s/<pap\/>/\[pap\]/g;
	$line=~s/<papf\/>/\[-pap\]/g;
	$line=~s/<papd\/>/\[pap-\]/g;
	$line=~s/<convf\/>/\[-conv\]/g;
	$line=~s/<convo\/>/\[conv-\]/g;
	$line=~s/<topf\/>/\[-top\]/g;
	$line=~s/<topo\/>/\[top-\]/g;
	$line=~s/<riref\/>/\[-rire\]/g;
	$line=~s/<rireo\/>/\[rire-\]/g;
	$line=~s/<rire\/>/\[rire\]/g;
	$line=~s/<overlapf\/>/\[-overlap\]/g;
	$line=~s/<overlapo\/>/\[overlap-\]/g;
	$line=~s/<ouio\/>/\[-\"oui"\]/g;
	$line=~s/<ouif\/>/\[\"oui"-\]/g;
	$line=~s/<r\/>/\[r\]/g;



	$line=~s/@@<\/nontext>/>/g;
	$line=~s/@/</g;
	$line=~s/<nontext>//g;
	$line=~s/<\/nontext>//g;
	$line=~s/<text>//g;
	$line=~s/<\/text>//g;
	


	$line=~s/<i\/>/\[i\]/g;
	$line=~s/<n\/>/\[n\]/g;
	$line=~s/<bi\/>/\[b-\]/g;
	$line=~s/<bf\/>/\[-b\]/g;
	$line=~s/<b\/>/\[b\]/g;
	$line=~s/<bb\/>/\[bb\]/g;
	$line=~s/<papd\/>/\[pap-\]/g;
	$line=~s/<papf\/>/\[-pap\]/g;
	$line=~s/<pap\/>/\[pap\]/g;


	##effacage des doubles
	$line=~s/<loc-addr-elec><loc-addr-elec>/<loc-addr-elec>/g ;
#	$line=~s/<org-gsp><org>/<org-gsp>/g ;
#	$line=~s/<\/org><\/org-gsp>/<\/org-gsp>/g ;
	print $line ;

	
	
} ;

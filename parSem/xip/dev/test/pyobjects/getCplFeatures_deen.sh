#DSC: Produces coupling features for each input file passed in the argument (need to run createNbestTranslations.sh before running this). 
#To call : sh getCplFeatures.sh /opt/ParSem_fast/GALATEAS/CLIR/rerankingExp_forID/*.src

lngSrc=en
lngTgt=de
workingDir=/opt/ParSem/GALATEAS/CLIR/NLE_journal/
scriptsDir=/opt/MLDAT_fast/USERS/vnikouli/GALATEAS/new_reranking_exp/scripts_documented/
#$(dirname $(readlink -f $0))/../
mkdir $workingDir/reranking_data
#rm cpl
case $lngSrc in
    en)	
	case $lngTgt in
	    fr)
		dictST=/opt/MLDAT_fast/DATA_FROZEN/MOSES_MODELS/WMT08-news/fr-en/outputs/model.$lngSrc-$lngTgt/lex.0-0.n2f
		dictTS=/opt/MLDAT_fast/DATA_FROZEN/MOSES_MODELS/WMT08-news/fr-en/outputs/model.$lngSrc-$lngTgt/lex.0-0.f2n
		;;
	    de)
		dictST=/opt/MLDAT_db/Users/Vassilina/Organic.Lingua/Baseline_models/work/en-de/training/model/lex.e2f
		dictTS=/opt/MLDAT_db/Users/Vassilina/Organic.Lingua/Baseline_models/work/en-de/training/model/lex.f2e
		;;
	esac
	;;
    fr)
	case $lngTgt in
	    en)
		dictST=/opt/CLT_fast/DATA_FROZEN/MOSES_MODELS/WMT08-news/fr-en/outputs/model.$lngSrc-$lngTgt/lex.0-0.n2f
		dictTS=/opt/CLT_fast/DATA_FROZEN/MOSES_MODELS/WMT08-news/fr-en/outputs/model.$lngSrc-$lngTgt/lex.0-0.f2n
		;;
	    de)
		dictST=/opt/ParSem/GALATEAS/CLIR/dictionaries/de-fr/lex.f2e
		dictTS=/opt/ParSem/GALATEAS/CLIR/dictionaries/de-fr/lex.e2f
		;;
	esac
	;;

    de)
	case $lngTgt in
	    fr)
		dictST=
		;;
	    en)
		dictTS=/opt/MLDAT_db/Users/Vassilina/Organic.Lingua/Baseline_models/work/en-de/training/model/lex.e2f
		dictST=/opt/MLDAT_db/Users/Vassilina/Organic.Lingua/Baseline_models/work/en-de/training/model/lex.f2e
		;;
	esac
	;;
esac
for task in GIRT2004 GIRT2005 GIRT2006 DS2007 DS2008
#AH2001 AH2002 AH2003 AH-TEL2008 AH-TEL2009 GC2006 GC2007
#
#
#AH2000
do
    mosesDir=/opt/CLT_fast/EXTERNAL_TOOLS/moses
    #perl $mosesDir/scripts/tokenizer.perl -l $lngSrc </opt/ParSem/GALATEAS/CLIR/NLE_journal/sourceTopics/$task.$lngSrc > /opt/ParSem/GALATEAS/CLIR/NLE_journal/sourceTopics/$task.$lngSrc.tok
done
#python /opt/ParSem/GALATEAS/CLIR/NLE_journal/scripts/filterDict.py $dictST 1 /opt/ParSem/GALATEAS/CLIR/NLE_journal/sourceTopics/*.tok.$lngSrc > /opt/ParSem/GALATEAS/CLIR/NLE_journal/nle_journal/dict.$lngSrc$lngTgt
#python  /opt/ParSem/GALATEAS/CLIR/NLE_journal/scripts/filterDict.py $dictTS 0 /opt/ParSem/GALATEAS/CLIR/NLE_journal/sourceTopics/*.tok.$lngSrc > /opt/ParSem/GALATEAS/CLIR/NLE_journal/nle_journal/dict.$lngTgt$lngSrc
dictST=/opt/ParSem/GALATEAS/CLIR/NLE_journal/nle_journal/dict.$lngSrc$lngTgt
dictTS=/opt/ParSem/GALATEAS/CLIR/NLE_journal/nle_journal/dict.$lngTgt$lngSrc
#exit

for task in GIRT2004 GIRT2005 GIRT2006 DS2007 DS2008
#AH2001 AH2002 AH2003 AH-TEL2008 AH-TEL2009 GC2006 GC2007
#
#
#AH2000
do    
    echo $task

    #ff=${f%.src}
    #echo $ff
    for model in augmented_lm euparl_lm  
    do
	newT=`echo $task | tr [A-Z] [a-z]`
	nbestDir=/opt/ParSem/GALATEAS/CLIR/NLE_journal/nle_journal/$model/tuned_euparl/test_$newT/$lngSrc$lngTgt/translation/
	echo $nbestDir
	
	#exit
	sh ./run_bayard $scriptsDir/reranking_scripts/getCouplingFF2NbestList_chunkbased.py $lngSrc $lngTgt $nbestDir/nbest.out /opt/ParSem/GALATEAS/CLIR/NLE_journal/sourceTopics/cap_$task\_baseline.tok.$lngSrc $workingDir/reranking_data/ $task\_$model.$lngTgt $dictST $dictTS  
	exit
    done
done

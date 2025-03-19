#!/bin/bash

path1=/home/fnoorala/Desktop/hmmComparison/wsj/
path2=/opt/ParSem_db/Corpora/MapHMM2treebank2/wsj/
path3=/home/fnoorala/parSem/Sources/difference/
rm temp1.txt
rm temp2.txt
for i in {00..01}
do
	l1=$(find $path1$i"/std/" -maxdepth 1 -name "*.pos.txt" | sort)
	l2=$(find $path2$i"/mapping2/" -maxdepth 1 -name "*.hmm.map" | sort)
	outpath=$(echo $path3$i"/")
	mkdir $outpath


#declare -a array
#space_sep_string="aa bb cc dd"
#array=(${space_sep_string// / })

#for i in ${array[@]}; do
 #   echo $i
#done

	j1=0
	for i in $l1
	do
		arr1[ $j1 ]=$i
		j1=$(echo $j1+1 | bc)
	done

	j2=0
	for i in $l2
	do
		arr2[ $j2 ]=$i
		j2=$(echo $j2+1 | bc)
	done

	for i in $(eval echo {0..$j1})
	do
#split each 

	while IFS=$'/' read -r -a myArray1
	do
 	echo ${myArray1[0]} >> temp1.txt

	done < ${arr1[ $i ]}

	while IFS=$'/' read -r -a myArray2
	do
 	echo ${myArray2[0]} >> temp2.txt
	done < ${arr2[ $i ]}

		

#Find difference and write in file		
		#diff -b -B -w -I '``.''' temp1.txt temp2.txt > $outpath"/"$i".diff" 
		#diff -b -B -w <( sed s/\"//g  temp1.txt) <(sed s/\"//g  temp2.txt) > $outpath"/"$i".diff" 
	diff -b -B -w  temp1.txt  temp2.txt > $outpath"/"$i".diff"
	if [ -s $outpath"/"$i".diff" ] ; then 
		echo $outpath"/"$i".diff" has difference
	else
		rm   $outpath"/"$i".diff"
		echo $outpath"/"$i".diff"  deleted
			
	fi
		
	rm temp1.txt
	rm temp2.txt
	done
done




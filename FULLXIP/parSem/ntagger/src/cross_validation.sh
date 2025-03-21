#!/bin/bash
path1=/home/fnoorala/parSem/wapiti-1.4.0/tweets/B1+B2+C+Xip
n=10
for i in 0 1 2 3 4 5 6 7 8
do
	#create folds for (train, dev,test)
	 /home/roux/ANTKIF/kif/dev/KIF/kilauea/bin/kif  n-folds.kif $n $i
	#train model
	./wapiti train -p $path1$"/pattern"  -d $path1$"/nfolds/dev.nfoldcv" -c -1 0.25 $path1$"/nfolds/train.nfoldcv"   $path1$"/nfolds/model"
	#test model
	./wapiti label -m $path1$"/nfolds/model" -c   $path1$"/nfolds/test.nfoldcv" $path1$"/nfolds/sequence.cv" >> $path1$"/nfolds/results.cv"
done




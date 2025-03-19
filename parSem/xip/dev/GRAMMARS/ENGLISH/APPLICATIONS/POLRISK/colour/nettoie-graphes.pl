#!/opt/STools/bin/perl -w

open(F,$ARGV[0]);

while($line=<F>){
	if ($line=~/^Name=/ || $line=~/^phrase \d/)
	{$line="";}
	elsif ($line=~/TOP{/)
	{
	$line="\n		********************\n";
	}
	else
	{
	$line=~s/\d\> //g;
	$line=~s/_ins_ins_dup=event/                  /g;
	$line=~s/\(\d+:/(/g;
	$line=~s/_\d+//g;
	$line=~s/\(\d\)/  /g;
	$line=~s/\(\d+\)/   /g;
	}

	print $line;

}

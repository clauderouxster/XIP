#!/bin/sh

# Ce shell - prend le fichier donne en premier parametre, 
#          - ne garde que le n premieres lignes, n donné par le second parametre
#          - ecrit un fichier d'entree avec comme suffixe le nb de lignes
#          - Le passe a xip 
#          - sauvegarde le fichier de sortie avec comme suffixe le nb de lignes suivi de .out
#          - Verifie l'encodage UTF-8 de tous les fichiers ci dessous.

# Avec le fichier d'entree x_00001, il y a un pb lorsqu'on passe de 132 ligne a 133.
# De plus, s'il y a un caractere incorrect qqpart, toutes les lignes, y compris precedentes, sont differentes si elles ont des caracteres non-iso

ORIG_FILE=$1
SIZE=$2

INFILE=$ORIG_FILE.$SIZE

echo "Testing the UTF-8 encoding of the input file"
isutf8 $ORIG_FILE

head -n $SIZE $ORIG_FILE >$INFILE

echo "Testing the UTF-8 encoding of the input file reduced to the first $SIZE lines"
isutf8 $ORIG_FILE

/opt/ParSem/bin/xip-10.01-77/bin/linux/xip -grm /home/jmcoursi/workspace/parSem/xip/dev/GRAMMARS/SPANISH/GRMFILES/spanish_entit.grm  -inpututf8 -outpututf8 -text $INFILE > $INFILE.out

echo "Testing the UTF-8 encoding of the output file"
isutf8 $INFILE.out

echo $?


#!/bin/bash

INPUT=$1
TF=tmp.txt

matlab -nojvm -nodesktop -nosplash -r "fprintf(2, '%s\n%s\n%s',which('startup'), which('finish'), userpath() ); exit" 2> $TF
STARTUPFILE=$(sed -n '1p' < $TF)
FINISHFILE=$(sed -n '2p' < $TF)
USERPATH=$(sed -n '3p' < $TF | sed "s/://")
rm $TF

if [[ -z "$STARTUPFILE" ]]; then
	STARTUPFILE="$USERPATH/startup.m"
fi

if [[ -z "$FINISHFILE" ]]; then
	FINISHFILE="$USERPATH/finish.m"
fi

LINESTART=$(sed -n -e "/'Init'/=" $STARTUPFILE)
LINEFINISH=$(sed -n -e "/'Init'/=" $FINISHFILE)

if [[ ${INPUT:0:1} != "u" ]]; then
	
	THEDIRECTORY=$(realpath $INPUT)
	INITIALISE="[~,~] = system(['$THEDIRECTORY/','Init'],'-echo');"
	
	mv ProjectIt $THEDIRECTORY/ProjectIt
	ln -s $THEDIRECTORY/ProjectIt $THEDIRECTORY/Init
	cat myProjection.m | sed "s:REPLACE_PATH:$THEDIRECTORY:" > $THEDIRECTORY/myProjection.m


	if [[ -z "$LINESTART" ]]; then
		echo $INITIALISE >> $STARTUPFILE
	fi
	
	if [[ -z "$LINEFINISH" ]]; then
		echo $INITIALISE >> $FINISHFILE
	fi
		
else
	
	THEDIRECTORY=$(realpath ${INPUT:1:$((${#INPUT}-1))})
	rm -f $THEDIRECTORY/ProjectIt $THEDIRECTORY/Init $THEDIRECTORY/myProjection.m

	if [[ -n "$LINESTART" ]]; then
		sed "$LINESTART d" $STARTUPFILE >> $TF
		cat $TF > $STARTUPFILE
		rm $TF
	fi
	
	if [[ -n "$LINEFINISH" ]]; then
		sed "$LINEFINISH d" $FINISHFILE >> $TF
		cat $TF > $FINISHFILE
		rm $TF
	fi

fi
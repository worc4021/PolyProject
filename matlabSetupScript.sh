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

LINESTART=$(sed -n -e "/mainExec/=" $STARTUPFILE)
LINEFINISH=$(sed -n -e "/mainExec/=" $FINISHFILE)

if [[ ${INPUT:0:1} != "u" ]]; then
	
	THEDIRECTORY=$(realpath $INPUT)
	INITIALISE="[~,~] = system(['$THEDIRECTORY/','mainExec init'],'-echo');"
	
	mv mainExec $THEDIRECTORY/mainExec
	
	cat rawFunction.m | sed \
			-e "s:REPLACE_PATH:$THEDIRECTORY:" \
			-e "s:FUNCTION_NAME:polyProject:" \
			-e "s:ARGUMENT_LIST:Ain,bin,dim:" \
			-e "s:LAST_ARGUMENT:dim:" \
			-e "s:FUNCTION_CALL:mainExec project:" > $THEDIRECTORY/polyProject.m
	cat rawFunction.m | sed \
			-e "s:REPLACE_PATH:$THEDIRECTORY:" \
			-e "s:FUNCTION_NAME:rowReduce:" \
			-e "s:ARGUMENT_LIST:Ain,bin:" \
			-e "s:LAST_ARGUMENT:0:" \
			-e "s:FUNCTION_CALL:mainExec reduce:" > $THEDIRECTORY/rowReduce.m
	cat rawFunction.m | sed \
			-e "s:REPLACE_PATH:$THEDIRECTORY:" \
			-e "s:FUNCTION_NAME:vertexCompute:" \
			-e "s:ARGUMENT_LIST:Ain,bin:" \
			-e "s:LAST_ARGUMENT:0:" \
			-e "s:FUNCTION_CALL:mainExec vertex:" > $THEDIRECTORY/vertexCompute.m

	if [[ -z "$LINESTART" ]]; then
		echo $INITIALISE >> $STARTUPFILE
	fi
	
	if [[ -z "$LINEFINISH" ]]; then
		echo $INITIALISE >> $FINISHFILE
	fi
		
else
	
	THEDIRECTORY=$(realpath ${INPUT:1:$((${#INPUT}-1))})
	rm -f $THEDIRECTORY/mainExec $THEDIRECTORY/Init $THEDIRECTORY/polyProject.m $THEDIRECTORY/vertexCompute.m $THEDIRECTORY/rowReduce.m

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
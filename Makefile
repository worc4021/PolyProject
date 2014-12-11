CFLAGS = -DLRSLIB -DGMP -DTIMES -DSIGNALS -DB64 
# -DDBG
LFLAGS = -Wl,-no_pie -lgmp

# This is the path to your Matlab distribution, in particular where 
# tmwtypes.h is located.
MATLABINCLUDEDIR = /Applications/MATLAB_R2014b.app/extern/include/

# Change this one to your desired path where your executable should be.
# (Somewhere on your Matlab's search path.)
FUNCTIONPATH = ../../MATLAB/Funktionen/

COMPILER = cc

all: ProjectIt cleanUp moveToPath

ProjectIt: myProjectionMain.o myFunctions.o lrslib.o lrsgmp.o
	$(COMPILER) $(LFLAGS) myProjectionMain.o myFunctions.o lrslib.o lrsgmp.o -o ProjectIt

myProjectionMain.o: myProjectionMain.c
	$(COMPILER) -c $(CFLAGS) -I$(MATLABINCLUDEDIR) myProjectionMain.c -o myProjectionMain.o

myFunctions.o: myFunctions.c myFunctions.h
	$(COMPILER) -c $(CFLAGS) -I$(MATLABINCLUDEDIR) myFunctions.c -o myFunctions.o

lrslib.o: ./lrslib/lrslib.c ./lrslib/lrslib.h
	$(COMPILER) -c $(CFLAGS) -I$(MATLABINCLUDEDIR) ./lrslib/lrslib.c -o lrslib.o

lrsgmp.o: ./lrslib/lrsgmp.c ./lrslib/lrsgmp.h
	$(COMPILER) -c $(CFLAGS) -I$(MATLABINCLUDEDIR) ./lrslib/lrsgmp.c -o lrsgmp.o

lrslong.o: ./lrslib/lrslong.c ./lrslib/lrslong.h
	$(COMPILER) -c $(CFLAGS) -I$(MATLABINCLUDEDIR) ./lrslib/lrslong.c -o lrslong.o

cleanUp:
	rm *.o

moveToPath:
	mv ProjectIt $(FUNCTIONPATH)ProjectIt && \
	cp myProjection.m $(FUNCTIONPATH)myProjection.m
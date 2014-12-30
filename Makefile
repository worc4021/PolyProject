CFLAGS := -DLRSLIB -DGMP -DTIMES -DSIGNALS -DB64 
#-DDBG
LFLAGS = -Wl,-no_pie -lgmp

# This is the path to your Matlab distribution, in particular where 
# tmwtypes.h is located.
MATLABINCLUDEDIR = /Applications/MATLAB_R2014b.app/extern/include/

# Change this one to your desired path where your executable should be.
# (Somewhere on your Matlab's search path.)
FUNCTIONPATH = ../../MATLAB/Funktionen

OBJECTS = mainFile.o GMPmat.o dMat.o projection.o\
		translation.o lrslib/lrslib.o lrslib/lrsgmp.o

all: ProjectIt clean install

ProjectIt: $(OBJECTS)
	$(CC) $(LFLAGS) $^ -o ProjectIt

.c.o:
	$(CC) $(CFLAGS) -I$(MATLABINCLUDEDIR) $< -o $@ -c

clean:
	rm -f $(OBJECTS)

install:
	bash -c "source matlabSetupScript.sh $(FUNCTIONPATH)"

uninstall:
	bash -c "source matlabSetupScript.sh u$(FUNCTIONPATH)"

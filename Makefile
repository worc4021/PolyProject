CFLAGS := -DLRSLIB -DGMP -DTIMES -DSIGNALS -DB64 -DLRS_QUIET
# DFLAG = -g
LFLAGS = -Wl,-no_pie -lgmp

# This is the path to your Matlab distribution, in particular where 
# tmwtypes.h is located.
MATLABINCLUDEDIR = /Applications/MATLAB_R2014b.app/extern/include/

# Change this one to your desired path where your executable should be.
# (Somewhere on your Matlab's search path.)
FUNCTIONPATH = ../../MATLAB/Funktionen

OBJECTS = mainFile.o GMPmat.o dMat.o projection.o\
		translation.o lrslib/lrslib.o lrslib/lrsgmp.o

all: mainExec clean install

mainExec: $(OBJECTS)
	@ $(CC) $(LFLAGS) $^ -o mainExec 
	@echo Linking $@.

.c.o:
	@ $(CC) $(DFLAG) $(CFLAGS) -I$(MATLABINCLUDEDIR) $< -o $@ -c 
	@echo Building $@.

clean:
	@rm -f $(OBJECTS) 
	@echo Cleaned up

install:
	@bash -c "source matlabSetupScript.sh $(FUNCTIONPATH)"
	@echo Installing function to $(FUNCTIONPATH) and setting up search path.

uninstall:
	@bash -c "source matlabSetupScript.sh u$(FUNCTIONPATH)"
	@echo Uninstalling function.


outdir               = ./bin
outdir_rel           = ./bin/release
appfile              = ./bin/fmm3d_debug
appfile_rel          = ./bin/fmm3d_release
SUPERGLUE_DIR        = /pica/h1/afshin/sg/superglue/include
SUPERGLUE_FLAGS      = -pthread -I$(SUPERGLUE_DIR)  #-pedantic -Wno-long-long -Wno-format
DUCTTEIP_DIR         = /pica/h1/afshin/Damavand/D4/DuctTeip/ResStudio/Prototype
DUCTTEIP_INC         = $(DUCTTEIP_DIR)/include
DUCTTEIP_LIBPATH     = $(DUCTTEIP_DIR)/lib/
DUCTTEIP_LIBPATH_REL = $(DUCTTEIP_DIR)/lib/release
DUCTTEIP_LIB         = ductteip
SOURCE_DIR           = ./src
HEADER_DIR           = ./include
DUCTTEIP_FLAGS       = -DWITH_MPI=1 -I$(DUCTTEIP_INC)
SUPERGLUE_FLAGS      = -I$(SUPERGLUE_DIR) 
COMP_FLAGS           = $(SUPERGLUE_FLAGS)
COMP_FLAGS          += $(DUCTTEIP_FLAGS) 
COMP_FLAGS          += -I$(HEADER_DIR) 

ifeq ($(CXX),icpc)
#-----------------------Intel Compiler set---------------------
	CPP  = mpiicpc
	LINK_FLAGS=-lm -lrt -pthread -L$(DUCTTEIP_LIBPATH) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition 
	LINK_FLAGS_REL=-lm -lrt -pthread -L$(DUCTTEIP_LIBPATH_REL) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition 
#	LINK_FLAGS=-lm -lrt -pthread -Wl,--allow-multiple-definition 
	COMP_FLAGS += -std=c++11 -qopt-prefetch -O3  -xCORE-AVX2 -g
else
#-----------------------GCC Compiler set---------------------
	CPP  = mpicxx
	GCOV_FLAGS=-fprofile-arcs -ftest-coverage
	OPTIM_FLAGS=-mavx -march=broadwell -mfma -O3 -Wwrite-strings
	SPECIAL_FLAGS=$(OPTIM_FLAGS)     
	LINK_FLAGS=-lm -lrt -lpthread -L$(DUCTTEIP_LIBPATH) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition -Wl,--whole-archive
	LINK_FLAGS_REL=-lm -lrt -lpthread -L$(DUCTTEIP_LIBPATH_REL) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition -Wl,--whole-archive
	COMP_FLAGS += -std=c++11 $(SPECIAL_FLAGS) 
endif
#########################################################
source:=$(notdir $(shell ls -Sr $(SOURCE_DIR)/*.cpp))
objnames:=$(source:%.cpp=%.o)
objects:=$(addprefix $(outdir)/,$(objnames))
objects_rel:=$(addprefix $(outdir_rel)/,$(objnames))
RED=\033[0;31m
GREEN=\033[1;32m
NC=\033[0m

all: $(appfile) $(appfile_rel)

$(appfile) : $(appfile)($(outdir)/*.o)
	@echo -e "$(GREEN) generate output exe $(NC) $(RED)$(appfile)$(NC)."

$(appfile)($(outdir)/*.o) : $(objects)
	@echo -e "$(GREEN) *** link objects *** $(NC)"
	$(CPP) -o $(appfile) $? $(LINK_FLAGS)

$(objects):  $(outdir)/%.o:  $(SOURCE_DIR)/%.cpp 
	@echo -e "$(GREEN) compile $(notdir $<) $(NC)"
	@$(CPP) -c -o $@ $< $(COMP_FLAGS) -g
#===================================================

$(appfile_rel) : $(appfile_rel)($(outdir_rel)/*.o)
	@echo -e "$(GREEN) generate output release exe $(NC) $(RED)$(appfile_rel)$(NC)."

$(appfile_rel)($(outdir_rel)/*.o) : $(objects_rel)
	@echo -e "$(GREEN) *** link objects *** $(NC)"
	$(CPP) -o $(appfile_rel) $? $(LINK_FLAGS_REL)

$(objects_rel):  $(outdir_rel)/%.o:  $(SOURCE_DIR)/%.cpp 
	@echo -e "$(GREEN) compile $(notdir $<) $(NC)"
	@$(CPP) -c -o $@ $< $(COMP_FLAGS) -DLOG_FMMM=0

clean: 
	rm $(outdir)/*.o $(appfile) $(outdir_rel)/*.o $(appfile_rel)
copy:
	cp $(appfile) /crex1/proj/snic2017-7-18/nobackup/FMM3D/bin
	cp $(appfile_rel) /crex1/proj/snic2017-7-18/nobackup/FMM3D/bin

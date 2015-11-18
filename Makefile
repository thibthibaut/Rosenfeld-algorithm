# -------------------- #
# - ECC_Par Makefile - #
# -------------------- #

# -- Lile list ----------
FILE =  Region32.cpp ImageEtiquette.cpp LabelRosenfeld.cpp main.cpp
C_OPTION = -lpthread

# -- Paths ----------
SRC_PATH = src
OBJ_PATH = obj
EXE_PATH = bin
INC_PATH = include

LIB_LIB_PATH =
LIB_INC_PATH =

# -- OS ----------
OS = LINUX

# -- Macros ----------
CC = g++

# -- Flags ----------
C_DEBUG_FLAGS = -O0
C_OPTIMISATION_FLAGS = -O3
C_ARCH_FLAGS =
C_OS_FLAGS = -D$(OS)
C_INC_FLAGS = -I$(INC_PATH)

CFLAGS = $(C_OPTION) $(C_OPTIMISATION_FLAGS) $(C_ARCH_FLAGS) $(C_OS_FLAGS) $(C_INC_FLAGS) $(LIB_INC_PATH)
LDFLAGS = $(C_OPTION) $(C_OPTIMISATION_FLAGS) $(C_ARCH_FLAGS) $(C_OS_FLAGS) $(C_INC_FLAGS) $(LIB_LIB_PATH)

# -- Final product ----------
PRODUCT = SystemParallelProg

# -- src and obj List ----------
SRC = $(addprefix ${SRC_PATH}/, $(FILE))
OBJ = $(addprefix ${OBJ_PATH}/, $(addsuffix .o, $(basename $(FILE))))

# -- Base rules ----------
$(OBJ_PATH)/%.o : $(SRC_PATH)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

#-----Main rule ----------
Release : $(EXE_PATH)/$(PRODUCT)

$(EXE_PATH)/$(PRODUCT): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIB_INC_PATH) $(LIB_LIB_PATH)

# -- Other stuff ----------
depend:
	makedepend $(CFLAGS) -Y $(SRC)

cleanRelease : clean
clean:
	rm -f $(OBJ)
	rm -f ${EXE_PATH}/${PRODUCT}

tar:
	tar -cvf poi.tar Makefile src include
	gzip -9 poi.tar

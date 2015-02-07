# ENV
CC = g++
#LINK = g++

VLROOT = /home/bingqingqu/user-libs/vlfeat/vlfeat-0.9.19
VLLIB = /home/bingqingqu/user-lib/vlfeat/vlfeat-0.9.19/bin/glnxa64
EIGENROOT = /home/bingqingqu/user-libs/eigen-3.2.4
BIN = bin/
SRC = src/
LIB = lib/
INC = include/
OBJ = obj/
SRCEXT = cpp

SOURCES = $(shell find $(SRC) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRC)%,$(OBJ)%,$(SOURCES:.$(SRCEXT)=.o))
# FLAGS
CPPFLAGS = -g -Wall -DOS_LINUX -std=c++0x -O3
INCLUDES = -I$(VLROOT) -I$(EIGENROOT) -I$(INC)
LDFLAGS =  -L$(LIB) -L$(VLLIB) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl


# Target
TARGET = $(BIN)$/LLCcoder

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN)
	@echo "	Linking..."
	@echo "	$(CC) $^ -o $(TARGET) $(LDFLAGS)"; $(CC) $^ -o $(TARGET) $(LDFLAGS)

$(OBJ)%.o: $(SRC)%.$(SRCEXT)
	@mkdir -p $(OBJ)
	@echo "	$(CC) $(CPPFLAGS) $(INCLUDES) -c -o $@ $<"; $(CC) $(CPPFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	@echo "	Cleaning..."
	@echo "	$(RM) -r $(OBJ) $(TARGET)"; $(RM) -r $(OBJ) $(TARGET)

.PHONY: clean

# # Directory
# CREATE_DIR:
# 	mkdir -p $(BIN)

# # Sources
# # SRCS_C1 = $(wildcard $(SRC)*.c)
# SRCS_CPP = $(wildcard $(SRC)main.cpp $(SRC)file_utility.cpp $(SRC)image_feature_extract.cpp)


# # Objects
# # OBJS_C = $(patsubst $(SRC)%.c, $(TEMP)%.o, $(SRCS_C))
# OBJS_CPP = $(patsubst $(SRC)%.cpp, $(TEMP)%.o, $(SRCS_CPP))


# # Command
# CMD_COMPILE = $(CC) -c $(CPPFLAGS) $< -o $@ $(INCLUDES) $(DEFS)
# CMD_LINK = $(LINK) -o $@ $^ $(LDFLAGS)

# # Make targets
# $(PROGS): $(OBJS_CPP)
# 	$(CMD_LINK)

# # Make .o files
# # $(TEMP)%.o: $(SRC)%.c
# # 	$(CMD_COMPILE)

# $(TEMP)%.o: $(SRC)%.cpp
# 	$(CMD_COMPILE)

# # clean
# .PHONY: clean
# clean:
# 	$(RM) $(PROGS) $(OBJS_CPP)
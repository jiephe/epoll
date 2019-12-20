CFLAGS  += -std=c99 -Wall -O2 
CXXFLAGS += -std=c++11 -Wall -O2 
LIBS    := -Wl,-Bstatic -lssl -lcrypto -lmysqlcppconn-static  -Wl,-Bdynamic -lstdc++ -lpthread

TARGET  := $(shell uname -s | tr '[A-Z]' '[a-z]' 2>/dev/null || echo unknown)

LIBS    += -ldl -lz
LDFLAGS += -Wl,-E

ODIR := obj

SRC  :=
SRC1 := 
OBJ := $(patsubst %.c,$(ODIR)/%.o,$(SRC1))
SRC += $(SRC1)

SRC2 := epoll.cpp 
OBJ += $(patsubst %.cpp,$(ODIR)/%.o,$(SRC2))
SRC += $(SRC2) 

SRC3 := 
OBJ += $(patsubst %.cc,$(ODIR)/%.o,$(SRC3))
SRC += $(SRC3)
			
BIN  := EpollOut

DEPS    := 

all: $(BIN)

clean:
	$(RM) -rf $(BIN) obj

$(OBJ): Makefile $(DEPS) | $(ODIR)

$(BIN): $(OBJ)
	@echo LINK $(BIN)
	@$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

$(ODIR)/%.o: %.cpp
	@echo $(CXX) $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(ODIR)/%.o: %.cc
	@echo $(CXX) $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(ODIR)/%.o: %.c
	@echo $(CC) $<
	@$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR):
	@mkdir -p $@
	
.PHONY: all clean

.SUFFIXES:
.SUFFIXES: .c .cpp .cc .o

vpath %.c   	. 
vpath %.cpp   	. 
vpath %.h   	. 


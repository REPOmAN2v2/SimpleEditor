CC=gcc

IDIR =../include
ODIR=../obj
RDIR =../resources
BDIR=../bin

CFLAGS=-I$(IDIR) -c -Wall -O2 -lgdi32 -mwindows
LFLAGS=-I$(IDIR) -Wall -O2 -lgdi32 -mwindows

_DEPS = editor.h resources.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = editor.o editores.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: build $(BDIR)/editor

build:
	@test -d $(ODIR) || mkdir $(ODIR)
	@test -d $(BDIR) || mkdir $(BDIR)

$(ODIR)/editores.o: $(RDIR)/editor.rc 
	windres $(RDIR)/editor.rc -o $(ODIR)/editores.o

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/editor: $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

.PHONY: clean rebuild

clean:
	rm -f $(ODIR)/* $(BDIR)/* 

rebuild: clean all
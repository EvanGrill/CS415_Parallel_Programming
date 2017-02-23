CC = mpicc
SRCDIR := src
BUILDDIR := build
TARGET := ./build/main

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -Wall
LIB := -lpmi
INC := -I include

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $< $(INC)

clean:
	$(RM) -r $(BUILDDIR) $(TARGET) *.csv slurm-*
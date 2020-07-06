BINDIR := bin
SRCDIR := src
EXECUTABLE := main
LIBS := -lglfw -lGL
# LIBS := -lGL -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

all: $(BINDIR)/$(EXECUTABLE)
$(BINDIR)/$(EXECUTABLE): $(SRCDIR)/main.cpp $(SRCDIR)/fluid.cpp
	g++ -o $@ $^ $(LIBS)
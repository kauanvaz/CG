CPP    = g++
WINDRES= windres
RM     = rm -f
OBJS   = floodfill.o \
         AppResource.res

LIBS   = -static -lglut32 -lglu32 -lopengl32 -lwinmm -lgdi32
CFLAGS = -DGLUT_STATIC

.PHONY: floodfill.exe clean clean-after

all: floodfill.exe

clean:
	$(RM) $(OBJS) floodfill.exe

clean-after:
	$(RM) $(OBJS)

floodfill.exe: $(OBJS)
	$(CPP) -Wall -s -o $@ $(OBJS) $(LIBS)

floodfill.o: floodfill.cpp
	$(CPP) -Wall -s -c $< -o $@ $(CFLAGS)

AppResource.res: AppResource.rc
	$(WINDRES) -i AppResource.rc -J rc -o AppResource.res -O coff


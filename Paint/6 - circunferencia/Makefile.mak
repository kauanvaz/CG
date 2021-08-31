CPP    = g++
WINDRES= windres
RM     = rm -f
OBJS   = COMpoligono.o \
         circunferencia.o \
         AppResource.res

LIBS   = -static -lglut32 -lglu32 -lopengl32 -lwinmm -lgdi32
CFLAGS = -DGLUT_STATIC

.PHONY: circunferencia.exe clean clean-after

all: circunferencia.exe

clean:
	$(RM) $(OBJS) circunferencia.exe

clean-after:
	$(RM) $(OBJS)

circunferencia.exe: $(OBJS)
	$(CPP) -Wall -s -o $@ $(OBJS) $(LIBS)

COMpoligono.o: COMpoligono.c
	$(CPP) -Wall -s -c $< -o $@ $(CFLAGS)

circunferencia.o: circunferencia.cpp
	$(CPP) -Wall -s -c $< -o $@ $(CFLAGS)

AppResource.res: AppResource.rc
	$(WINDRES) -i AppResource.rc -J rc -o AppResource.res -O coff


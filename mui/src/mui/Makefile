CC=gcc

CFLAGS= -I../../src

AR = ar
RANLIB = ranlib

SRCS = gizmo.c glutmui.c displaylist.c uicolor.c miscui.c mui.c vslider.c textlist.c button.c pulldown.c hslider.c
OBJS = $(SRCS:.c=.o)

all	: libmui.a

libmui.a: $(OBJS) 
	-rm -f libmui.a
	$(AR) cru libmui.a $(OBJS)
	$(RANLIB) libmui.a
	
clean:
	rm *.o libmui.a


<$PLAN9/src/mkhdr

OBJS=main.$O buffer.$O
CFLAGS=-Wall -pedantic
LDFLAGS=

ALL=edit

all:V:$ALL

edit: $OBJS
	$LD $LDFLAGS -o $target $prereq

%.$O: %.c
	$CC $CFLAGS -c $stem.c

clean:V:
	rm -f $ALL *.[$OS]

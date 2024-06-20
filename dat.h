#include <u.h>
#include <libc.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>

#define STACK 65536

typedef struct Buffer Buffer;

struct Buffer {
	char *bob;
	char *bog;
	char *eob;
	char *eog;
	uint gapsize;
	uint size;
};

Buffer	*bufcreate(uint);
uint	 bufdeleteafter(Buffer *, uint);
uint	 bufdeleteruneafter(Buffer *, uint);
uint	 bufdeletebefore(Buffer *, uint);
uint	 bufdeleterunebefore(Buffer *, uint);
void	 buffree(Buffer *);
uint	 bufinsert(Buffer *, char *, uint);
uint	 bufmovebackwards(Buffer *, uint);
uint	 bufmoveforward(Buffer *, uint);
uint	 bufmoverunebackwards(Buffer *, uint);
uint	 bufmoveruneforward(Buffer *, uint);
uint	 bufresize(Buffer *);

Mousectl *mctl;
Keyboardctl *kctl;

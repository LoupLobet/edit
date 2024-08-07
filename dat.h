#include <u.h>
#include <libc.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>

#define STACK 65536

typedef struct Buffer Buffer;
typedef struct View View;

struct Buffer {
	char *bob;
	char *bog;
	char *eob;
	char *eog;
	uint gapsize;
	uint size;
};

#define BufInitSize 64

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
Buffer	*bufopen(char *);
uint	 bufresize(Buffer *);

struct View {
	Image *bg, *fg;
	Image *cursor;
	Buffer *buf;
	char *name;
};

View 	*viewcreate(char *);
int	 viewdraw(View *);

Mousectl *mctl;
Keyboardctl *kctl;


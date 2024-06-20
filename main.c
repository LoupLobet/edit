#include <u.h>
#include <libc.h>
#include <draw.h>
#include <cursor.h>
#include <keyboard.h>
#include <mouse.h>
#include <thread.h>

#include "fns.h"
#include "dat.h"
#include "config.h"

void	 keyboardthread(void *);
void	 mousethread(void *);

void
keyboardthread(void *v)
{
	enum { Ekeyboard };
	Rune r;
	Alt alts[] = {
		{ kctl->c, &r,  CHANRCV },
		{ nil,     nil, CHANNOP },
	};

	threadsetname("keyboardthread");
	for (;;) {
		switch (alt(alts)) {
		case Ekeyboard:
			break;
		}
	}
}

void
mousethread(void *v)
{
	enum { Emouse, Eresize };
	Mouse *m;
	Alt alts[] = {
		{ mctl->c,       &m,  CHANRCV },
		{ mctl->resizec, nil, CHANRCV },
		{ nil,           nil, CHANNOP },
	};
	
	threadsetname("mousethread");
	for (;;) {
		switch(alt(alts)) {
		case Emouse:
			break;
		case Eresize:
			resize();
			break;
		}
	}
}

void
resize(void) {

}

void
threadmain(int argc, char *argv[])
{
	Image *bg;

	ARGBEGIN{
	}ARGEND

	initdraw(nil, nil, "edit");
	if ((mctl = initmouse(nil, screen)) == nil) {
		fprint(2, "can't initialize mouse: %r\n");
		threadexitsall(nil);
	}
	if ((kctl = initkeyboard(nil)) == nil) {
		fprint(2, "can't initialize keyboard: %r\n");
		threadexitsall(nil);
	}

	flushimage(display, 1);
	
	if ((bg = allocimage(display, Rect(0, 0, 1, 1), RGBA32, 1, configcolors[BACKGROUND])) == nil) {
		fprint(2, "can't create background image: %r\n");
		threadexitsall(nil);
	}

	draw(screen, screen->r, bg, nil, ZP);

	threadcreate(keyboardthread, nil, STACK);
	threadcreate(mousethread, nil, STACK);
	flushimage(display, Refnone);

	for (;;);

	threadexitsall(nil);
}


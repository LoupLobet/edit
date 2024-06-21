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

void	 keyboardevent(Rune);

View mainvw;


void	 keyboardevent(Rune);

void
keyboardevent(Rune k)
{
	char utf8[UTFmax];
	int n;

	switch (k) {
	case Kleft:
		bufmoverunebackwards(mainvw.buf, 1);
		break;
	case Kright:
		bufmoveruneforward(mainvw.buf, 1);
		break;
	case Kup:
		break;
	case Kdown:
		break;
	case Kbs:
		bufdeleterunebefore(mainvw.buf, 1);
		break;
	default:
		// need to create a bufinsertrune() func
		n = runetochar(utf8, &k);
		bufinsert(mainvw.buf, utf8, n);
	}
	viewdraw(&mainvw);
}

int
viewdraw(View *vw) 
{
	Point p = { confighmargin, configvmargin };

	draw(screen, screen->r, vw->bg, nil, ZP);
	p = stringn(screen, p, vw->fg, ZP, display->defaultfont, vw->buf->bob, vw->buf->bog - vw->buf->bob);
	draw(screen, screen->r, vw->cursor, nil, Pt(-p.x, -p.y));
	stringn(screen, p, vw->fg, ZP, display->defaultfont, vw->buf->eog + 1, vw->buf->eob - vw->buf->eog);
	flushimage(display, 1);
	return 0;
}

void
threadmain(int argc, char *argv[])
{
	Rune r;
	char *fontname = nil;
	enum { Ekeyboard, Emouse, Eresize, NEALT };
	Alt alts[NEALT+1];

	ARGBEGIN{
	case 'f':
		fontname = ARGF();
		if (fontname == nil)
			goto Usage;
		break;
	default:
	Usage:
		fprint(2, "usage:\n");
	}ARGEND

	if (initdraw(nil, fontname, "edit") < 0) {
		fprint(2, "can't initialize display: %r\n");
		threadexitsall("initdraw");
	}
	if ((mctl = initmouse(nil, screen)) == nil) {
		fprint(2, "can't initialize mouse: %r\n");
		threadexitsall("initmouse");
	}
	if ((kctl = initkeyboard(nil)) == nil) {
		fprint(2, "can't initialize keyboard: %r\n");
		threadexitsall("initkeyboard");
	}

	flushimage(display, 1);

	Image *bg, *fg, *cursor;
	if ((bg = allocimage(display, Rect(0, 0, 1, 1), RGBA32, 1, configcolors[CBackground])) == nil) {
		fprint(2, "can't create background image: %r\n");
		threadexitsall("allocimage");
	}
	if ((fg = allocimage(display, Rect(0, 0, 1, 1), RGBA32, 1, configcolors[CForeground])) == nil) {
		fprint(2, "can't create foreground image: %r\n");
		threadexitsall("allocimage");
	}
	if ((cursor = allocimage(display, Rect(0, 0, 1, display->defaultfont->height), RGBA32, 0, configcolors[CForeground])) == nil) {
		fprint(2, "can't create cursor image: %r\n");
		threadexitsall("allocimage");
	}

	mainvw.bg = bg;
	mainvw.fg = fg;
	mainvw.cursor = cursor;
	mainvw.name = "main";
	if ((mainvw.buf = bufcreate(512)) == nil) {
		fprint(2, "can't create buffer: %d\n", 64);
		threadexitsall("bufcreate");
	}
	viewdraw(&mainvw);

	alts[Ekeyboard] = (Alt){ kctl->c,       &r,        CHANRCV };
	alts[Emouse]    = (Alt){ mctl->c,       &mctl->m,  CHANRCV };
	alts[Eresize]   = (Alt){ mctl->resizec, nil,       CHANRCV };
	alts[NEALT]     = (Alt){ nil,           nil,       CHANEND };

	for (;;) {
		switch (alt(alts)) {
		case Ekeyboard:
			keyboardevent(r);
			break;
		case Emouse:
			break;
		case Eresize:
			break;
		}
	}


	threadexitsall(nil);
}


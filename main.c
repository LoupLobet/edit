#include <u.h>
#include <libc.h>
#include <thread.h>

#include "fns.h"
#include "dat.h"

void	printbuffer(Buffer *);

void
threadmain(int argc, char *argv[])
{
	Buffer *buf;

	ARGBEGIN{
	}ARGEND
	if ((buf = bufcreate(32)) == nil) {
		fprint(2, "edit: can't create new buffer");
		threadexitsall("bufcreate");
	}
	bufinsert(buf, "0123789", 7);
	bufmovebackward(buf, 3);
	bufinsert(buf, "456", 3);
	printbuffer(buf);
	bufmoveforward(buf, 3);
	bufinsert(buf, "󰥓jes󰥓us", strlen("󰥓jes󰥓us"));
	printbuffer(buf);
	bufmovebackward(buf, 2);
	printbuffer(buf);
	bufdeleterunebefore(buf, 2);
	bufmovebackward(buf, 2);
	bufdeleterunebefore(buf, 2);
	printbuffer(buf);

	buffree(buf);

	threadexitsall(nil);
}

void
printbuffer(Buffer *buf)
{
	int i;
	int isgap;
	Rune r;

	isgap = 0;
	for (i  = 0; i < buf->size; i++) {
		if (buf->bob + i == buf->bog)
			isgap = 1;
		if (isgap)
			print("_");
		else {
			print("%c", buf->bob + i);
			i += chartorune(&r, buf->bob + i) - 1;
			if (r != Runeerror)
				print("%C", r);
		}
		if (buf->bob + i == buf->eog)
			isgap = 0;
	}
	print("\n");
}


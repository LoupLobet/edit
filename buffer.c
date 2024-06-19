#include <u.h>
#include <libc.h>

#include "dat.h"

Buffer *
bufcreate(uint size)
{
	Buffer *buf;
	
	if ((buf = malloc(sizeof(Buffer))) == nil)
		return nil;
	if ((buf->bob = malloc(size)) == nil) {
		free(buf);
		return nil;
	}
	buf->size = size;
	buf->gapsize = buf->size;
	buf->eob = buf->bob + buf->size - 1;
	buf->bog = buf->bob;
	buf->eog = buf->eob;
	return buf;
}

uint
bufmovebackward(Buffer *buf, uint n)
{
	uint moved;

	moved = 0;
	while (moved != n) {
		if (buf->bog == buf->bob)
			return moved;
		*buf->eog = *(buf->bog - 1);
		buf->bog--;
		buf->eog--;
		moved++;
	}
	return moved;
}

uint
bufmoveforward(Buffer *buf, uint n)
{
	uint moved;

	moved = 0;
	while (moved != n) {
		if (buf->eog == buf->eob)
			return moved;
		*buf->bog = *(buf->eog + 1);
		buf->bog++;
		buf->eog++;
		moved++;
	}
	return moved;
}

uint
bufinsert(Buffer *buf, char *s, uint n)
{
	int i;

	for (i = 0; i < n; i++) {
		*buf->bog = s[i];
		buf->bog++;
		buf->gapsize--;
		if (buf->gapsize <= 1) {
			if (!bufresize(buf))
				return i;
		}
	}
	return i;
}

uint
bufdeleteafter(Buffer *buf, uint n)
{
	int i;
	for (i = 0; i < n; i++) {
		if (buf->eog == buf->eob)
			return i;
		buf->eog++;
		buf->gapsize++;
	}
	return i;
}

uint
bufdeleteruneafter(Buffer *buf, uint n)
{
	int i, runelen;
	Rune r;

	for (i = 0; i < n; i++) {
		if (buf->eog == buf->eob)
			return i;
		runelen = chartorune(&r, buf->eog + 1);
		if (r == Runeerror)
			return i;
		buf->eog += runelen;
		buf->gapsize += runelen;
	}
	return i;
}

uint
bufdeletebefore(Buffer *buf, uint n)
{
	int i;
	for (i = 0; i < n; i++) {
		if (buf->bog == buf->bob)
			return i;
		buf->bog--;
		buf->gapsize++;
	}
	return i;
}

uint
bufdeleterunebefore(Buffer *buf, uint n)
{
	int i, j, runelen;
	Rune r;

	for (i = 0; i < n; i++) {
		if (buf->bog == buf->bob)
			return i;
		/* find the last rune before the gap */
		for (j = 0; j < 4; j++) {
			if (buf->bog - 1 - j == buf->bob)
				return i;
			runelen = chartorune(&r, buf->bog - 1 - j);
			if (r == Runeerror)
				continue;
			buf->bog -= runelen;
			buf->gapsize += runelen;
			break;
		}
		if (r == Runeerror) /* invalid utf8 codepoint */
			return i;
	}
	return i;
}

void
buffree(Buffer *buf)
{
	free(buf->bob);
	free(buf);
}

uint
bufresize(Buffer *buf)
{
	char *new;
	uint newsize;
	uint leftsize, gapsize, rightsize;

	newsize = (!buf->size ? 2 : buf->size * 2);
	if ((new = realloc(buf->bob, newsize)) == nil)
		return 0;
	leftsize = buf->bog - buf->bob;
	rightsize = buf->eob - buf->eog;
	gapsize = newsize - buf->size + buf->gapsize;
	if (leftsize > 0)
		memcpy(new, buf->bob, leftsize);
	if (rightsize > 0)
		memcpy(new + leftsize + gapsize, buf->eog + 1, rightsize);
	buf->bob = new;
	buf->bog = new + leftsize;
	buf->eog = new + leftsize + gapsize - 1;
	buf->eob = new + newsize;
	buf->size = newsize;
	buf->gapsize = gapsize;
	return newsize;
}


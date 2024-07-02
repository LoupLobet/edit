#include <u.h>
#include <libc.h>

View *
viewcreate(char *name)
{
	View *vw;

	if ((vw = malloc(sizeof(View))) == nil)
		return nil;
	vw->name = name;
}

#include <canlib.h>
#include "uin15765tp.h"
#include "uin15765.h"

int main(void)
{
	canInitializeLibrary();
	can_init();
	i15765_init();

	UIN15765_DEBUG_TRACE("%s OK!\n\r",  __FUNCTION__ );
	return 0;
}
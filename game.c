#include "main.h"

PlaydateAPI *pd = NULL;

void setPDPtr(PlaydateAPI *ppd)
{
pd= ppd;
}

void setupGame(void)
{

}

LCDBitmap *loadImageAtPath(const char *path)
{
	const char *outErr = NULL;
	LCDBitmap *img = pd->graphics->loadBitmap(path, &outErr);
	if (outErr != NULL) {
		pd->system->logToConsole("Error loading image at path '%s': %s", path, outErr);
	}
	return img;
}


int update(void *ud)
{

}
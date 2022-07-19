#include "pd_api.h"
#include "main.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
		if (event == kEventInit)
		{
			setPDPtr(playdate);
			playdate->display->setRefreshRate(60);
			playdate->system->setUpdateCallback(update, NULL);
			setupGame();
		}

	return 0;
}
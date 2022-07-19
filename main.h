#ifndef game_h
#define game_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pd_api.h"
void setupGame(void);
int update(void *ud);
void setPDPtr(PlaydateAPI *ppd);

#endif /* game_h */
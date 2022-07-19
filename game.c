#include "main.h"

PlaydateAPI *pd = NULL;

char *ballPatch = "images/ball";
char *rocketPatch ="images/rocket";

typedef struct CObject
{	
	LCDSprite *sprite;
	int postion[2];
	int size[2];
	int speed[2];
} TObject;
 
 TObject rocket;
 TObject ball;
 bool objectCollision(TObject obj1, TObject obj2);
 void moveObject(TObject *obj);
 void worldCollision(TObject *obj);
 //bool objectCollision(TObject *obj1, TObject *obj2);
void setPDPtr(PlaydateAPI *ppd)
{
pd= ppd;
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

void initObject(TObject *obj, char *patch, int xPos, int yPos)
{
	LCDBitmap *image = NULL;
	int scaleX = 0;
	int scaleY = 0;
	(*obj).sprite = pd->sprite->newSprite();
	image = loadImageAtPath(patch);
	
	pd->sprite->setImage((*obj).sprite, image, kBitmapUnflipped);

	pd->graphics->getBitmapData(image, &scaleX, &scaleY, NULL,NULL,NULL);
	(*obj).size[0]=scaleX;
	(*obj).size[1]=scaleY;
	(*obj).postion[0]=xPos;
	(*obj).postion[1]=yPos;
	(*obj).speed[0]=2;
	(*obj).speed[1]=2;
	
	pd->sprite->addSprite(obj->sprite);	

	moveObject(obj);
}



void setupGame(void)
{
 initObject(&ball, ballPatch,50,50);
 initObject(&rocket, rocketPatch,200,230);
}


void moveObject(TObject *obj)
{
	pd->sprite->moveTo((*obj).sprite, obj->postion[0],obj->postion[1]);
}

void flyBall(TObject *ball, TObject *rocket)
{
worldCollision(ball);
(*ball).postion[0] += ball->speed[0];
(*ball).postion[1] += ball->speed[1];
if (objectCollision(*ball, *rocket))
{
	(*ball).speed[1] = -1;
}
moveObject(ball);
}
void moveRocket(TObject *obj)
{
	PDButtons  press;
	pd->system->getButtonState(&press,NULL,NULL);
	(*obj).speed[0] = 0;
	if (press & kButtonLeft)
	{
	(*obj).speed[0] = -5;
	}
	if (press & kButtonRight)
	{
		(*obj).speed[0] = 5;
	}

	if (obj->postion[0] < 0 + (obj->size[0] / 2))// || (obj->postion[0] > 400 - (obj->scale[0] / 2)))
	{
		(*obj).postion[0] = 0 + (obj->size[0] / 2);
	}
	if (obj->postion[0] > 400 - (obj->size[0] / 2))
	{
		(*obj).postion[0] = 400 - (obj->size[0] / 2);
	}
	(*obj).postion[0] += obj->speed[0];
	moveObject(obj);
}

void worldCollision(TObject *obj)
{
	if ((obj->postion[0] <= 0+(obj->size[0]/2)) || (obj->postion[0] >= 400- (obj->size[0] / 2)))
	{
		(*obj).speed[0] *= -1;
	}
	if ((obj->postion[1] <= 0+ (obj->size[1] / 2)))// || (obj->postion[1] >= 240- (obj->size[1] / 2)))
	{
		(*obj).speed[1] *= -1;
	}
}

bool objectCollision(TObject obj1, TObject obj2)
{
return (obj1.postion[0]+(obj1.size[0]/2))>(obj2.postion[0]-(obj2.size[0]/2))&&(obj1.postion[1] + (obj1.size[1] / 2)) > (obj2.postion[1] - (obj2.size[1] / 2))&&
		(obj2.postion[0] + (obj2.size[0] / 2)) > (obj1.postion[0] - (obj1.size[0] / 2))&&	(obj2.postion[1] + (obj2.size[1] / 2)) > (obj1.postion[1] - (obj1.size[1] / 2));
}

int update(void *ud)
{	
	flyBall(&ball, &rocket);
	moveRocket(&rocket);
	pd->sprite->updateAndDrawSprites();
}
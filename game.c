#include "main.h"

PlaydateAPI *pd = NULL;

char *ballPatch = "images/ball";
char *rocketPatch ="images/rocket";
char *blockPatch ="images/block50";

typedef struct CObject
{	
	LCDSprite *sprite;
	int postion[2];
	int size[2];
	int speed[2];
	bool isStatic;
} TObject;

typedef struct CHudIco
{
	LCDBitmap *sprite;
	int position[2];
}THudIco;
 
 TObject rocket;
 TObject ballSprite;
 TObject bonus;
 TObject blocks[99];
 int blockCount = 0;
 

 THudIco liveIco[3];

 bool objectHorizontalCollisiion(TObject obj1, TObject obj2);
 bool objectVerticalCollision(TObject obj1, TObject obj2);
 void horizontalBounds(TObject *obj);
 void verticalBounds(TObject *obj);
 void moveObject(TObject *obj);
 void worldCollision(TObject *obj);
 void destroyBlock(TObject *obj, int index);
 void initBall();
void setPDPtr(PlaydateAPI *ppd)
{
pd= ppd;
}
void drawWindow()
{
	pd->graphics->drawLine(350, 0, 350, 400, 5, kColorBlack);
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
	(*obj).speed[0]=0;
	(*obj).speed[1]=0;
	
	pd->sprite->addSprite(obj->sprite);	

	(*obj).isStatic=true;
	moveObject(obj);
}

void createBlocks()
{
	int bW = 0;
	int bH = 0;
	LCDBitmap *blockTemplate;
	blockTemplate = loadImageAtPath(blockPatch);
	pd->graphics->getBitmapData(blockTemplate,&bW,&bH,NULL,NULL,NULL);
	int posX = bW/2;
	int posY = bH/2;
	int maxBlockInRow = 0;
	
	for (int i = 0; i < (sizeof(blocks)/sizeof(blocks[0])); i++)
	{
		if (maxBlockInRow > 175){break;}
		initObject(&blocks[i], blockPatch, posX, posY);
		posX += bW;
		if (posX > 350- maxBlockInRow)
		{
			maxBlockInRow += bW;
			posX = bW/2 + maxBlockInRow;
			posY += bH;			
		}
		blockCount ++;
	}
}

void initBall()
{
	ballSprite.postion[0] = 175;
	ballSprite.postion[1] = rocket.postion[1] - rocket.size[1] / 2 - ballSprite.size[1] / 2;
	ballSprite.postion[0]=rocket.postion[0];
	ballSprite.speed[0]=0;
	ballSprite.speed[1]=0;
	ballSprite.isStatic = true;
}

void startBall()
{
	ballSprite.speed[0]=3;
	ballSprite.speed[1]=3;

	ballSprite.isStatic=false;
}

void resetBallStatus()
{
	initBall();
}


void initHud(THudIco* hud[])
{
	for (int i = 0; i < 3; i++)
	{
		LCDBitmap* ball = NULL;
		(*hud)[i].sprite = pd->sprite->newSprite();
		ball = loadImageAtPath(ballPatch);
		pd->sprite->setImage((*hud[i]).sprite, ball, kBitmapUnflipped);
		pd->sprite->addSprite(hud[i]->sprite);
		hud[i]->position[0] = 375;
		hud[i]->position[1] = 50 + i * 25;
	}
}

void setupGame(void)
{
 initObject(&ballSprite, ballPatch,200,100);
 initObject(&rocket, rocketPatch,200,230); 
 //initObject(&bonus, ballPatch,0,0);
 createBlocks(); 
 initBall();
 initHud(&liveIco);
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
if (objectVerticalCollision(*ball, *rocket)&& (objectHorizontalCollisiion(*ball, *rocket)))
{
	//(*ball).speed[1] = -1;
	(*ball).postion[1]=rocket->postion[1]-(rocket->size[1]);
	verticalBounds(ball);
}

for (int i = 0; i < blockCount;i++)
{
	if (objectVerticalCollision(*ball, blocks[i])&& objectHorizontalCollisiion(*ball, blocks[i]))
	{

		if (ball->postion[1]>(blocks[i].postion[1]+(blocks[i].size[1]/2)))
		{
			verticalBounds(ball);
			//horizontalBounds(ball);
		}
		else
		{
			horizontalBounds(ball);
			//verticalBounds(ball);
		}
		destroyBlock(&blocks[i], i);
		break;
	}
}
if (ball->isStatic)
{
	(*ball).postion[0]=rocket->postion[0];
}
moveObject(ball);
}

void spawnBonus(int xPos, int yPos)
{
	if(rand() % 100 < 10)
	{
		initObject(&bonus, ballPatch,xPos, yPos);
		bonus.speed[1]=2;
		moveObject(&bonus);
		bonus.speed[1]=3;
	}
}

void dropBonus()
{


}
void destroyBlock(TObject *obj, int index)
{
	//TObject *BBlock = NULL;
	spawnBonus(obj->postion[0], obj->postion[1]);
	//BBlock = obj;
	pd->sprite->removeSprite((*obj).sprite);
	blocks[index]=blocks[blockCount-1];
	blockCount --;
	if(blockCount<0){blockCount=0;}
	
}

void controll(TObject *obj)
{
	PDButtons  press;
	pd->system->getButtonState(&press,NULL,NULL);
	(*obj).speed[0] = 0;
	if (press & kButtonLeft)
	{
	(*obj).speed[0] = -10;
	}
	if (press & kButtonRight)
	{
		(*obj).speed[0] = 10;
	}
	if (press & kButtonA)
	{
		startBall();
	}

	if (obj->postion[0] < 0 + (obj->size[0] / 2))// || (obj->postion[0] > 400 - (obj->scale[0] / 2)))
	{
		(*obj).postion[0] = 0 + (obj->size[0] / 2);//(obj->size[0] / 2);
		//(*obj).speed[0] = 0;
	}
	if (obj->postion[0] > 350 - (obj->size[0] / 2))
	{
		(*obj).postion[0] = 350 - (obj->size[0] / 2);//  (obj->size[0] / 2);
		//(*obj).speed[0] = 0;
	}
	(*obj).postion[0] += obj->speed[0];
	moveObject(obj);
}

void horizontalBounds(TObject *obj)
{
	(*obj).speed[0] *= -1;
}

void verticalBounds(TObject *obj)
{
	(*obj).speed[1] *= -1;
}

void worldCollision(TObject *obj)
{
	if ((obj->postion[0] <= 0+(obj->size[0]/2)) || (obj->postion[0] >= 350- (obj->size[0] / 2)))
	{
		horizontalBounds(obj);
	}
	if ((obj->postion[1] <= 0+ (obj->size[1] / 2)))// || (obj->postion[1] >= 240- (obj->size[1] / 2)))
	{
		verticalBounds(obj);
	}
	if ((obj->postion[1] >= 240 - (obj->size[1] / 2)))
	{
		resetBallStatus();
	}
}


bool objectVerticalCollision(TObject obj1, TObject obj2)
{
return
(obj1.postion[1] + (obj1.size[1] / 2)) > (obj2.postion[1] - (obj2.size[1] / 2))&& (obj2.postion[1] + (obj2.size[1] / 2)) > (obj1.postion[1] - (obj1.size[1] / 2));
		
}

bool objectHorizontalCollisiion(TObject obj1, TObject obj2)
{
	return (obj2.postion[0] + (obj2.size[0] / 2)) > (obj1.postion[0] - (obj1.size[0] / 2)) && (obj1.postion[0] + (obj1.size[0] / 2)) > (obj2.postion[0] - (obj2.size[0] / 2));
}


void drawHud()
{

}
int update(void *ud)
{	
	flyBall(&ballSprite, &rocket);
	controll(&rocket);
	pd->sprite->updateAndDrawSprites();
	drawWindow();
	drawHud();
	dropBonus();
}
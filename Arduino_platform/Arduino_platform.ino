#include "mega_mgr.h"

MegaMng megaMng;

void setup() 
{
    megaMng.initMega();
}

void loop() 
{
    megaMng.mainLoop();
}

#include "global.h"

globalInfo_t gInfo;

//constructor func
void __attribute__((constructor)) initializer_before_main(){

    bzero(&gInfo, sizeof(globalInfo_t));
}

void initGlobalInfo(int argc, char* argv[])
{
    gInfo.width = CMOS_INPUT_WIDTH;
    gInfo.height = CMOS_INPUT_HEIGHT;
    gInfo.rawBytesPerPixel = 2;
    gInfo.componentsForRGB = 3;
    gInfo.argc = argc;
    gInfo.argv = argv;
}

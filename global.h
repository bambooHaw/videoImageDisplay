#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define USE_OPENCV_LIB 1
#ifdef USE_OPENCV_LIB
#include <opencv2/opencv.hpp>
#endif
#define APP_VERSION "V0.2.2"

#define CMOS_INPUT_WIDTH 1920
#define CMOS_INPUT_HEIGHT 1080

typedef struct _globalInfo{
    //pthread_mutex mutex;
    int width;
    int height;
    char rawBytesPerPixel;
    char componentsForRGB;
    int argc;
    char** argv;
}globalInfo_t;

#ifdef __cplusplus
extern "C"{
#endif
    extern globalInfo_t gInfo;
    extern void __attribute__((constructor)) initializer_before_main();
    extern void initGlobalInfo(int argc, char* argv[]);
#ifdef __cplusplus
}
#endif

#endif // GLOBAL_H

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define USE_OPENCV_LIB 1
#ifdef USE_OPENCV_LIB
#include <opencv2/opencv.hpp>
#endif
#define APP_VERSION "V0.2.8"


#define DATA_TYPE_RAW unsigned short
#define VIDEO_DEVICE "/dev/video0"
#define CMOS_INPUT_WIDTH 1920   //sensor固定输出1920*1080的图像
#define CMOS_INPUT_HEIGHT 1080
#define IMAGE_SIZE (CMOS_INPUT_WIDTH * CMOS_INPUT_WIDTH * 2)
#define BUFFER_COUNT 5  //最多申请5个缓冲区

#define DBG_PRINT(fmt, args...) printf("Debug: %s(+ %d)" fmt "\n", __func__, __LINE__, ##args)
#define ERR_PRINT(fmt, args...) printf("Error: %s(+ %d)" fmt "\n", __func__, __LINE__, ##args)

typedef struct _globalInfo{
    //pthread_mutex mutex;
    int fdCam;
    unsigned char* vbufp[BUFFER_COUNT]; //video buffer pointer
    IplImage *pBayerData;
    IplImage *pRgbDataInt16;
    IplImage *pRgbDataInt8;
    int quality[3];
    //struct v4l2_buffer video_buffer[BUFFER_COUNT];
    int width;
    int height;
    int size;
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

    extern int _requestResourceCam(globalInfo_t* g);
    extern int _releaseResourceCam(globalInfo_t* g);
    extern int _getImageFrame(unsigned char* bufp, int* size, globalInfo_t* g);
#ifdef __cplusplus
}
#endif

#endif // GLOBAL_H

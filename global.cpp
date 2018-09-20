#include "global.h"

globalInfo_t gInfo;

//constructor func
void __attribute__((constructor)) initializer_before_main(){

    bzero(&gInfo, sizeof(globalInfo_t));
}

void initGlobalInfo(int argc, char* argv[])
{
    gInfo.fdCam = -1;
    for(unsigned char i=0; i< BUFFER_COUNT; i++) gInfo.vbufp[i] = NULL;
    gInfo.pBayerData = NULL;
    gInfo.pRgbDataInt16 = NULL;
    gInfo.pRgbDataInt8 = NULL;
    gInfo.width = CMOS_INPUT_WIDTH;
    gInfo.height = CMOS_INPUT_HEIGHT;
    gInfo.rawBytesPerPixel = 2;
    gInfo.componentsForRGB = 3;
    gInfo.argc = argc;
    gInfo.argv = argv;
}

int openCam(globalInfo_t* g)
{
    int ret = 0;
    if(!g)
    {
        ret = -EINVAL;
        ERR_PRINT("globalInfo is NULL");
    }else
    {
        g->fdCam = open(VIDEO_DEVICE, O_RDWR);   //||O_NONBLOCK
        if(g->fdCam < 0)
        {
            ret = -EAGAIN;
            ERR_PRINT("open failed.");
        }
    }
    return ret;
}

int closeCam(globalInfo_t* g)
{
    if((NULL != g) && (g->fdCam > 0))
    {
        close(g->fdCam);
        g->fdCam = -1;
    }

    return 0;
}

int selectCam(globalInfo_t* g, int input)
{
    int ret = 0;
    if(!g)
    {
        ret = -EINVAL;
        ERR_PRINT("globalInfo is Null");
    }else
    {
        ret = ioctl(g->fdCam, VIDIOC_S_INPUT, &input);//设置输入源
    }

    return ret;
}

int initCam(globalInfo_t* g)
{
    unsigned int i;
    int ret = 0;
    struct v4l2_format format;
    bzero(&format, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//帧类型，用于视频捕获设备
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR10;//10bit raw格式
    format.fmt.pix.width = CMOS_INPUT_WIDTH;//分辨率
    format.fmt.pix.height = CMOS_INPUT_HEIGHT;

    ret = ioctl(g->fdCam, VIDIOC_TRY_FMT, &format);//设置当前格式
    if (ret != 0)
    {
        ERR_PRINT("ioctl(VIDIOC_TRY_FMT) failed %d(%s)", errno, strerror(errno));
    }else
    {
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = ioctl(g->fdCam, VIDIOC_S_FMT, &format);//设置当前格式
        if (ret != 0)
        {
            ERR_PRINT("ioctl(VIDIOC_S_FMT) failed %d(%s)", errno, strerror(errno));
        }else
        {

            struct v4l2_requestbuffers req;
            req.count = BUFFER_COUNT;//缓冲帧个数
            req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//缓冲帧数据格式
            req.memory = V4L2_MEMORY_MMAP;//内存映射方式
            ret = ioctl(g->fdCam, VIDIOC_REQBUFS, &req);//申请缓冲区
            if (ret != 0)
            {
                ERR_PRINT("ioctl(VIDIOC_REQBUFS) failed %d(%s)", errno, strerror(errno));
            }else
            {
                DBG_PRINT("req.count: %d", req.count);
                if (req.count < BUFFER_COUNT)
                {
                    ret = -ENOMEM;
                    ERR_PRINT("request buffer failed");
                }else
                {
                    struct v4l2_buffer buffer;
                    memset(&buffer, 0, sizeof(buffer));
                    buffer.type = req.type;
                    buffer.memory = V4L2_MEMORY_MMAP;
                    for (i=0; i<req.count; i++)
                    {
                        buffer.index = i;
                        ret = ioctl(g->fdCam, VIDIOC_QUERYBUF, &buffer);//获取缓冲帧地址
                        if(ret != 0)
                        {
                            ERR_PRINT("ioctl(VIDIOC_QUERYBUF) failed %d(%s)", errno, strerror(errno));
                        }else
                        {
                            DBG_PRINT("buffer.length: %d", buffer.length);
                            DBG_PRINT("buffer.m.offset: %d", buffer.m.offset);
                            gInfo.vbufp[i] = (unsigned char*)mmap(NULL, buffer.length, PROT_READ|PROT_WRITE, MAP_SHARED, gInfo.fdCam, buffer.m.offset);//内存映射
                            if(gInfo.vbufp[i] == MAP_FAILED)
                            {
                                ret = -ENOMEM;
                                ERR_PRINT("mmap() failed %d(%s)", errno, strerror(errno));
                            }else
                            {
                                buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                buffer.memory = V4L2_MEMORY_MMAP;
                                buffer.index = i;
                                ret = ioctl(g->fdCam, VIDIOC_QBUF, &buffer);//把缓冲帧放入队列中
                                if(ret != 0)
                                {
                                    ERR_PRINT("ioctl(VIDIOC_QBUF) failed %d(%s)", errno, strerror(errno));
                                }
                            }
                        }
                    }
                    if(ret == 0)
                    {
                        int buffer_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        ret = ioctl(g->fdCam, VIDIOC_STREAMON, &buffer_type);//启动数据流
                        if (ret != 0)
                        {
                            ERR_PRINT("ioctl(VIDIOC_STREAMON) failed %d(%s)", errno, strerror(errno));
                        }else
                        {
                            DBG_PRINT("cam init done.");
                        }
                    }
                }
            }
        }
    }
    return ret;
}

int _requestResourceCam(globalInfo_t* g)
{
    int ret = 0;
    if(!g)
    {
        ret = -EINVAL;
        ERR_PRINT("gloabalInfo is NULL.");
    }else
    {
        ret = openCam(g);
        if(0 == ret)
        {
            ret = selectCam(g, 0);
            if(0 == ret)
            {
                ret = initCam(g);
                if(0 == ret)
                {
                    //get opencV img space
                    g->pBayerData = cvCreateImage(cvSize(gInfo.width,gInfo.height), 16, 1);
                    g->pRgbDataInt16 = cvCreateImage(cvSize(gInfo.width,gInfo.height), 16, 3);
                    g->pRgbDataInt8 = cvCreateImage(cvSize(gInfo.width,gInfo.height), 8, 3);

                    g->quality[0] = CV_IMWRITE_PNG_COMPRESSION;
                    g->quality[1] = 0;  //0~100
                    g->quality[2] = 0;

                }else
                {
                    closeCam(g);
                }
            }else
            {
                closeCam(g);
            }
        }else
        {
            g->fdCam = -1;
        }
    }
    return ret;
}

int _releaseResourceCam(globalInfo_t* g)
{
    if(g->pBayerData)
        cvReleaseImage(&(g->pBayerData));
    if(g->pRgbDataInt8)
        cvReleaseImage(&(g->pRgbDataInt8));
    if(g->pRgbDataInt16)
        cvReleaseImage(&(g->pRgbDataInt16));
    return closeCam(g);
}

int _getImageFrame(unsigned char* bufp, int* size, globalInfo_t* g)
{
    int ret = 0;
    struct v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = BUFFER_COUNT;
    if(!g || !bufp || !size)
    {
        ERR_PRINT("Some arguments is NULL.");
    }else
    {
        ret = ioctl(g->fdCam, VIDIOC_DQBUF, &buffer);//从队列中取出一帧
        if(ret != 0)
        {
            ERR_PRINT("ioctl(VIDIOC_DQBUF) failed %d(%s)", errno, strerror(errno));
        }else
        {
            if(buffer.index >= BUFFER_COUNT)
            {
                ret = -EINVAL;
                ERR_PRINT("invalid buffer index: %d", buffer.index);
            }else
            {
                DBG_PRINT("dequeue done, index: %d, length: %d", buffer.index, buffer.length);
                *size = buffer.length;
                memcpy(bufp, g->vbufp[buffer.index], *size);//缓冲帧数据拷贝出来
                DBG_PRINT("copy done.");
                ret = ioctl(g->fdCam, VIDIOC_QBUF, &buffer);//缓冲帧放入队列
                if(ret != 0)
                {
                    ERR_PRINT("ioctl(VIDIOC_QBUF) failed %d(%s)", errno, strerror(errno));
                }else
                {
                    DBG_PRINT("enqueue done.");
                }
            }
        }
    }
    return ret;
}

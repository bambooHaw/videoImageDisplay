#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initGlobalInfo(argc, argv); //gInfo must init at first

    ui->label_version->setText(QString(APP_VERSION));

    slotRaw2rgbWithCV();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::slotRaw2rgbWithCV(void)
{
    FILE *fp = NULL;
    int ret = 0;
#define DATA_TYPE_RAW unsigned short

    /*for imread 16bit raw image, calloc mem*/
    DATA_TYPE_RAW *pRawData = (DATA_TYPE_RAW *)calloc(gInfo.width*gInfo.height, sizeof(DATA_TYPE_RAW));

    if(NULL == pRawData)
    {
        qWarning()<<"Fail to calloc buf:";
        ret = -ENOMEM;
    }else
    {
        if(NULL == (fp=fopen(gInfo.argv[1], "rb")))
        {
            qWarning()<<"Fail to read";
            ret = -EIO;
        }else
        {
            qDebug() << gInfo.argv[1] << " opened!" << endl;

#ifdef USE_OPENCV_LIB
            ret = fread(pRawData, sizeof(DATA_TYPE_RAW)*gInfo.width*gInfo.height, 1, fp);
            if (ret != 1)
            {
                qWarning("Fail to read raw data(ret:%d).\n", ret);
                ret = -EIO;
            }else
            {
                IplImage *pBayerData = cvCreateImage(cvSize(gInfo.width,gInfo.height), 16, 1);
                IplImage *pRgbDataInt16 = cvCreateImage(cvSize(gInfo.width,gInfo.height), 16, 3);
                IplImage *pRgbDataInt8 = cvCreateImage(cvSize(gInfo.width,gInfo.height), 8, 3);

                memcpy(pBayerData->imageData, (char *)pRawData, gInfo.width*gInfo.height*sizeof(DATA_TYPE_RAW));
                cvCvtColor(pBayerData, pRgbDataInt16, CV_BayerBG2RGB);

                /*switch 16bit image data to 8bit*/
                cvConvertScale(pRgbDataInt16, pRgbDataInt8, 0.25, 0);

                int qua[3] = {CV_IMWRITE_PNG_COMPRESSION , 100, 0};
                system("rm /opt/cv.png");
                qDebug("w: %d, h: %d, size:%d.\n", pRgbDataInt8->width, pRgbDataInt8->height, pRgbDataInt8->nSize);

                cvSaveImage("/opt/cv.png", pRgbDataInt8, qua);
                qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);

#if 0
                cvNamedWindow("rgb", 1);
                cvShowImage("rgb", pRgbDataInt8);
                cvWaitKey(0);
                cvDestroyWindow("rgb");
                cvReleaseImage(&pBayerData);
                cvReleaseImage(&pRgbDataInt8);
                cvReleaseImage(&pRgbDataInt16);
#endif
            }
#endif
            fclose(fp);
        }
        free(pRawData);

        //show jpg
        QImage newImage("/opt/cv.png");  //(QString(gInfo.argv[1]));   //((uchar*)(pRgbDataInt8->imageData), gInfo.width, gInfo.height, QImage::Format_Indexed8);
        qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);
        QImage sclImage = newImage.scaled(ui->label_image->width(), ui->label_image->height());
        qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);
        if(!sclImage.isNull())
            ui->label_image->setPixmap(QPixmap::fromImage(sclImage));
        else
            ui->label_image->setText(QString("Not supported image!"));
        qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);
    }

    return ret;
}

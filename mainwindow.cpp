#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int ret = 0;
    ui->setupUi(this);
    initGlobalInfo(argc, argv); //gInfo must init at first

    ui->label_version->setText(QString(APP_VERSION));

    QObject::connect(&dspTimer, SIGNAL(timeout()), this, SLOT(slotOpencvRealtimeShowRaw()), Qt::QueuedConnection);

    ret = _requestResourceCam(&gInfo);
    if(0 == ret)
        dspTimer.start();    //200ms for Human Vision System, depretched
    else
        qWarning("Error requestResourceCam(&gInfo) failed.");
}

MainWindow::~MainWindow()
{

    _releaseResourceCam(&gInfo);

    delete ui;
}

int MainWindow::slotOpencvRealtimeShowRaw(void)
{
    int ret = 0;

    qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);
    ret = _getImageFrame((unsigned char*)(gInfo.pBayerData->imageData), &(gInfo.size), &gInfo);

    qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);
    cvCvtColor(gInfo.pBayerData, gInfo.pRgbDataInt16, CV_BayerBG2RGB);
    /*switch 16bit image data to 8bit*/
    qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);
    cvConvertScale(gInfo.pRgbDataInt16, gInfo.pRgbDataInt8, 0.25, 0);
    qDebug("w: %d, h: %d, size:%d.\n", gInfo.pRgbDataInt8->width, gInfo.pRgbDataInt8->height, gInfo.pRgbDataInt8->nSize);

    system("rm /opt/cv.png");
    qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);
    cvSaveImage("/opt/cv.png", gInfo.pRgbDataInt8, gInfo.quality);
    qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);

#if 0
                cvNamedWindow("png", 1);
                cvShowImage("png", gInfo.pRgbDataInt8);
                cvWaitKey(0);
                cvDestroyWindow("png");
#endif
    //show jpg
    QImage newImage("/opt/cv.png");  //(QString(gInfo.argv[1]));   //((uchar*)(pRgbDataInt8->imageData), gInfo.width, gInfo.height, QImage::Format_Indexed8);
    QImage sclImage = newImage.scaled(ui->label_image->width(), ui->label_image->height());
    qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);
    if(!sclImage.isNull())
        ui->label_image->setPixmap(QPixmap::fromImage(sclImage));
    else
        ui->label_image->setText(QString("Not supported image!"));


    qDebug("---DEBUG--%s(+ %d)\n", __func__, __LINE__);

    return ret;
}

#include "widget.h"
#include "ui_widget.h"
#include <QPixmap>
#include <QDebug>
#include <QTime>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->label_src->setAlignment(Qt::AlignCenter);
    ui->label_dog->setAlignment(Qt::AlignCenter);

    ui->label_src->setPixmap(QPixmap("dog.png"));

    CascadeClassifier face_cascade;
    //加载分类文件
    //
    if( !face_cascade.load("./cascade.xml") )
    {
        qDebug()<<"haarcascade_frontalface_alt.xml 分类器加载错误";
        return;
    }
    QImage qImage = ui->label_src->pixmap()->toImage();
    Mat frame=QImage2cvMat(qImage);
    cvtColor( frame, frame, COLOR_BGR2GRAY );//转换成灰度图像

    std::vector<Rect> faces;

    //正脸检测
    QTime time;
    time.start();
    face_cascade.detectMultiScale(frame,faces);

    qDebug()<<time.elapsed()<<"ms";
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        #if 1
        Point center(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2);
        ellipse(frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4 );
        rectangle(frame,
                  cvPoint(cvRound(faces[i].x), cvRound(faces[i].y)),
                  cvPoint(cvRound((faces[i].x + faces[i].width-1)),
                  cvRound((faces[i].y + faces[i].height-1))),
                  Scalar(255, 255, 255), 3, 8, 0);
        #endif

        //提取识别结果
        Mat frame1;
        for(size_t i=0;i<faces.size();i++)
        {
            Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
            frame1= frame(Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
        }
        /*在控件上显示识别结果*/
        ui->label_dog->setPixmap(QPixmap::fromImage(Mat2QImage(frame1)));
    }

    /*在控件上显示*/
    QImage display_image=Mat2QImage(frame);
    ui->label_src->setPixmap(QPixmap::fromImage(display_image));

}

Widget::~Widget()
{
    delete ui;
}

QImage Widget::Mat2QImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}
Mat Widget::QImage2cvMat(QImage image)
{
    Mat mat;
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        //cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}


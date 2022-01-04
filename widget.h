#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "opencv2/core/core.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


    QImage Mat2QImage(const Mat& mat);
    Mat QImage2cvMat(QImage image);
    QString dir;
    QString filename;

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/gapi.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>
#include <QScreen>
#include <iostream>
using namespace cv;
using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void drawGrid(Mat &input, int divisions, Scalar clr);
    void generateFisheye2(Mat &genFE, Mat &front, Mat &bottom, Mat &left, Mat &right);
    float cosatan(float x);
    float sinatan(float x);
    float hyp(float x, float y);
    double LW=1000;    // left front-wall *millileters
    double RW=1000;    // right front-wall *millileters
    double depth=2000; // side-wall *millileters
    double high=1000;
    int zoom = 1000;

    Mat *plane_left;
    Mat *plane_right;
    Mat *plane_bottom;
    Mat *plane_front;
    Mat *frame_fisheye;


private slots:
    void on_pushButton_apply_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

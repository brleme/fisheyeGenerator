#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    QScreen *screen;
    screen = QGuiApplication::screens()[0]; //1

    Size size_rect(2000,1000);
    Size size_square(1000,1000);
  //  Size size_fisheye(1680,1050);
    Size size_fisheye(1920,1080);
    plane_left = new Mat(size_square,CV_8UC3);
    plane_right = new Mat(size_square,CV_8UC3);
    plane_bottom = new Mat(size_rect,CV_8UC3);
    plane_front = new Mat(size_rect,CV_8UC3);
    frame_fisheye = new Mat(size_fisheye,CV_8UC3);





    drawGrid(*plane_front,6,Scalar(255,0,255));
    drawGrid(*plane_bottom,6,Scalar(0,255,0));
    drawGrid(*plane_left,6,Scalar(255,0,0));
    drawGrid(*plane_right,6,Scalar(0,0,255));

    generateFisheye2(*frame_fisheye,*plane_front,*plane_bottom,*plane_left, *plane_right);

    namedWindow("Projector",WINDOW_NORMAL);
    moveWindow("Projector",screen->geometry().x(),screen->geometry().y());
    setWindowProperty("Projector",WND_PROP_FULLSCREEN,WINDOW_FULLSCREEN);

    imshow("Projector",*frame_fisheye);
  //  imshow("Bottom",*plane_bottom);
//    imshow("Front",plane_front);
   // imshow("Left",plane_left);
   // imshow("Right",plane_right);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::generateFisheye2(Mat &genFE,Mat &front,Mat &bottom, Mat &left,Mat &right){
    genFE.setTo(Scalar(0,0,0));
    //Point cFE(genFE.cols/2,genFE.rows/2);
    Point cFE(genFE.cols/2,5);
    line(genFE,Point(cFE.x,0),Point(cFE.x,genFE.rows),Scalar(255,255,255),2);
    line(genFE,Point(0,cFE.y),Point(genFE.cols,cFE.y),Scalar(255,255,255),2);
    int limX = genFE.cols / 2;
    int limY = genFE.rows;

    int rightY = right.rows;
    int rightX = right.cols;
    int y = front.rows;
    int x = front.cols;
    int yy = bottom.rows;
    int xx = bottom.cols;

    //real-frame ratio
    double ratioX = x / (LW+RW);
    double ratioY = y / high;

    double ratioSideX = rightX / high;
    double ratioSideY = rightY / depth;

    double ratiobX = xx / (LW+RW);
    double ratiobY = yy / depth;

    //FRONT
    for (int x=1;x<LW;x++){
        for (int y=1;y<high;y++){
            float hp = hyp(depth,y);
            float hm = hyp(depth,x);
            float xx = zoom * cosatan(y/hm) * -sinatan(x/depth);
            float yy = zoom * cosatan(x/hp) * sinatan(y/depth);
            genFE.at<Vec3b>(Point(cFE.x+round(xx), cFE.y+round(yy))) = front.at<Vec3b>(y*ratioY,(LW-x)*ratioX);
        }
    }

    for (int x=1;x<RW;x++){
        for (int y=1;y<high;y++){
            float hp = hyp(depth,y);
            float hm = hyp(depth,x);
            float xx = zoom * cosatan(y/hm) * sinatan(x/depth);
            float yy = zoom * cosatan(x/hp) * sinatan(y/depth);
            genFE.at<Vec3b>(Point(cFE.x+round(xx), cFE.y+round(yy))) = front.at<Vec3b>(y*ratioY,(x+LW)*ratioX);
        }
    }

    //BOTTOM
    for (int x=1;x<LW;x++){
        for (int y=1;y<depth;y++){
            float h1 = hyp(x,high);
            float xx = zoom * sinatan(h1/y) * -cosatan(high/x);
            float yy = zoom * sinatan(h1/y) * sinatan(high/x);
            if (yy<(limY-10)){
                genFE.at<Vec3b>(Point(cFE.x+round(xx), cFE.y+round(yy))) = bottom.at<Vec3b>(y*ratiobY,(LW-x)*ratiobX);
            }
        }
    }

    for (int x=1;x<RW;x++){
        for (int y=1;y<depth;y++){
            float h1 = hyp(x,high);
            float xx = zoom * sinatan(h1/y) * cosatan(high/x);
            float yy = zoom * sinatan(h1/y) * sinatan(high/x);
            if (yy<(limY-10)){
                genFE.at<Vec3b>(Point(cFE.x+round(xx), cFE.y+round(yy))) = bottom.at<Vec3b>(y*ratiobY,(LW+x)*ratiobX);
            }
        }
    }

    //SIDE
    for (int x=1;x<high;x++){
        for (int y=1;y<depth;y++){
            //LEFT WALL
            float h2 = hyp(x,LW);
            float x2 = zoom * sinatan(h2/y) * -cosatan(x/LW);
            float y2 = zoom * sinatan(h2/y) * sinatan(x/LW);
            if (x2>-limX){
                genFE.at<Vec3b>(Point(cFE.x+round(x2), cFE.y+round(y2))) = right.at<Vec3b>(x*ratioSideX,y*ratioSideY);
            }

            //RIGHT WALL
            float h1 = hyp(x,RW);
            float xx = zoom * sinatan(h1/y) * cosatan(x/RW);
            float yy = zoom * sinatan(h1/y) * sinatan(x/RW);
            if (xx<limX){
                genFE.at<Vec3b>(Point(cFE.x+round(xx), cFE.y+round(yy))) = left.at<Vec3b>(x*ratioSideX,y*ratioSideY);
            }
        }
    }
}


void MainWindow::drawGrid(Mat &input, int divisions, Scalar clr){
    int LH = input.cols / divisions;
    int LV = input.rows / divisions;
    line(input,Point(0,0),Point(input.cols,input.rows),Scalar(255,255,255),8);
    line(input,Point(input.cols,0),Point(0,input.rows),Scalar(255,255,255),8);
    for (int i=0; i<divisions; i++){
        line(input,Point(0,LV*i),Point(input.cols,LV*i),clr,8);
        line(input,Point(LH*i,0),Point(LH*i,input.rows),clr,8);
    }
    line(input,Point(input.cols,0),Point(input.cols,input.rows),clr,8);
    line(input,Point(0,0),Point(0,input.rows),clr,8);
    line(input,Point(0,0),Point(input.cols,0),clr,8);
    line(input,Point(0,input.rows),Point(input.cols,input.rows),clr,8);
}

float MainWindow::sinatan(float x){
    return (x / sqrt(1+(x*x)));
}
float MainWindow::cosatan(float x){
    return (1 / sqrt(1+(x*x)));
}
float MainWindow::hyp(float x, float y){
    return sqrt(x*x+y*y);
}

void MainWindow::on_pushButton_apply_clicked()
{
    RW = ui->spinBox_RW->value();
    LW = ui->spinBox_LW->value();
    depth = ui->spinBox_depth->value();
    high = ui->spinBox_high->value();
    zoom = ui->spinBox_zoom->value();
    generateFisheye2(*frame_fisheye,*plane_front,*plane_bottom,*plane_left, *plane_right);

    namedWindow("Projector",WINDOW_NORMAL);
    //moveWindow("Projector",screen->geometry().x(),screen->geometry().y());
    setWindowProperty("Projector",WND_PROP_FULLSCREEN,WINDOW_FULLSCREEN);

    imshow("Projector",*frame_fisheye);
  //  imshow("Bottom",*plane_bottom);
}

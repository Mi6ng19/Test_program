#ifndef DIALOG_H
#define DIALOG_H

extern "C"{
    #include "mx6s_v4l2_capture_uyvy.h"
}

#include <QDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QString>
#include <QLabel>
#include <QDebug>
#include <QTextStream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QKeyEvent>
#include <QDebug>
#include <QWidget>
#include <sys/stat.h>
#include <QStorageInfo>
#include <QProcess>
#include <malloc.h>
#include "gpsdecode/UI/satellitewidget.h"
#include <QSlider>
#include <QSpinBox>
#include <iostream>

//#define Test


#ifdef Test
    //数字0-9 + -
    #define Key_Up    48
    #define Key_Down  49
    #define Key_Left  50
    #define Key_Right 51

    #define Key_Menu     52
    #define Key_return   53
    #define Key_Location 54
    #define Key_Ok       55
    #define Key_Add      56
    #define Key_Sub      57

    #define Key_Power 43
    #define Key_Find  45
#else
    #define Key_Up    16777235
    #define Key_Down  16777237
    #define Key_Left  16777234
    #define Key_Right 16777236

    #define Key_Menu     16777268
    #define Key_return   16777269
    #define Key_Location 16777274
    #define Key_Ok       16777275
    #define Key_Add      16777270
    #define Key_Sub      16777271

    #define Key_Power 16777399
    #define Key_Find  16777273
#endif

extern int exit_window7;
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    QSlider *pSlider;
    QSpinBox *pSpinBox;
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

protected:
        void paintEvent(QPaintEvent *event);

private slots:

    void timerEvent(QTimerEvent *);
    void timetostart();
    void sleep(unsigned int msec);

    void ReceiveData(QString data);
    void Get_Info(char *cmd,char *prefix);
    void Get_BluetoothInfo();
    void Get_DisInfo(float *Info);

    void keyPressEvent(QKeyEvent *keyev);
    void keyReleaseEvent(QKeyEvent *keyev);
    void mousePressEvent(QMouseEvent *event);

    void window_1();
    void window_2();
    void window_3();
    void window_4();
    void window_5();
    void window_6();
    void window_7();
    void window_8();
    void window_9();

    void Get_BatteryInfo();
    int Mag_calc_angle();
    int Calibration_algorithm(int *center_x, int *center_y, int *radius, QList<double> paramOne, QList<double> paramTwo);
    int CalibrateProces();
    void removeInvalidData();
    void setCalibrateFlag();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

    void move_slider();

private:
    Ui::Dialog *ui;
#ifndef Test
    SatelliteWidget *pSatelliteWidget;
#endif

signals:
    void setui();
    void sendData(QString);

};



#endif // DIALOG_H

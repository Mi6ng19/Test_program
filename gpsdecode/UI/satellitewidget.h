#ifndef SATELLITEWIDGET_H
#define SATELLITEWIDGET_H

//#include "global.h"
//#include "../basewidget.h"

#include <memory>




#include <fstream>
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
#include <QObject>
#include <QButtonGroup>
#include "gpsdecode/gpsdecode.h"
#include <QCoreApplication>

#include <QThread>

extern int exit_window7;
using namespace std;

class QLabel;
class GPSDecode;
class SubFieldWidget;

class SonSatelliteWidget : public QWidget{
    Q_OBJECT
public:
    explicit SonSatelliteWidget(QWidget *parent = 0);

    QLabel *satelliteText[32];
    QLabel *satelliteSig[32];

    int maxSize;

protected:
    void deriveMousePressEvent(QMouseEvent *event);
    void deriveMouseMoveEvent(QMouseEvent *event);
    void deriveMouseReleaseEvent(QMouseEvent *event);

private:
    int mX;                          //信噪比条X轴位移
};

class SatelliteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SatelliteWidget(QWidget *parent = 0);

    void refreshLanguage();
    void Change_mode();

    void sleep(unsigned int msec);

protected:
    void deriveMousePressEvent(QMouseEvent *event);
    void deriveMouseMoveEvent(QMouseEvent *event);
    void deriveMouseReleaseEvent(QMouseEvent* event);



    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

    void touchMenu();

public slots:
    void on_pushButton_menu_clicked();
    void on_pushButton_beidou_clicked();
    void on_pushButton_gps_clicked();
    void on_pushButton_glonass_clicked();
    void on_pushButton_gps_beidou_clicked();
    void on_pushButton_gps_glonass_clicked();
    void on_pushButton_off_clicked();
    void on_pushButton_Analog_clicked();
    void on_pushButton_Save_clicked();
    void on_pushButton_Exit_clicked();

signals:
    void SatelliteWidgetSlide(int, bool);
    void clicked();

private slots:
    void respon();

    void responMenu(int idx);

private:
    void drawGBL(QLabel *lab, int colorIdx, QString value);

    void calcutorCoor(double dir, double elev, int &x, int &y);

private:
    QLabel *openCloseStatus;

    //设置GPS模块模式 0: GPS 1:北斗 2:GLONASS 3:GPS+北斗 4:GPS+GLONASS 5:关 6:模拟
    QPushButton *menu;
    QPushButton *beidou;
    QPushButton *gps;
    QPushButton *glonass;
    QPushButton *gps_beidou;
    QPushButton *gps_glonass;
    QPushButton *off;
    QPushButton *Analog;

    QPushButton *SAVE;
    QPushButton *EXIT;

    QLabel *which_model[2];
    QLabel *menu_label;

    QLabel *line;
    QLabel *line1;
    QLabel *line2;
    QLabel *line3;
    QLabel *line4;
    QLabel *line5;

    QLabel *mainLabel;
    QLabel *ellLabel;

    QLabel *satellite[32];

    QLabel *backLabel;

    QLabel *textLabel1;
    QLabel *textLabel2;
    QLabel *textLabel3;
    QLabel *textLabel4;



    QLabel *textLabelR1;
    QLabel *textLabelR2;
    QLabel *textLabelR3;
    QLabel *textLabelR4;

    QPixmap pixEll;
    QPixmap pixBack;

    SonSatelliteWidget *pSonSatelliteWidget;
    SubFieldWidget* pSubFieldWidget;

    QString menuStr[2] = {"OpenSatellite", "CloseSatellite"};

private:
   int mTmpPosX;

  // bool bSubMenu;           //判断子菜单是否存在，true:存在 flase：不存在
};


class MyThread : public QThread
{
public:
    MyThread();
    void run();
    void sleep(unsigned int msec);
       //isStop是易失性变量，需要用volatile进行申明
};



#endif // SATELLITEWIDGET_H


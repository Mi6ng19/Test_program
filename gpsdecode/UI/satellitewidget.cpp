#include "satellitewidget.h"
#include "gpsdecode/gpsdecode.h"
//#include "UI/UI_SETTING/subfieldwidget.h"
//#include "UI/UI_SETTING/setwidget_unit.h"

#include <QPainter>
#include <QPolygon>
#include <QMouseEvent>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <QFile>
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define Line1 245
#define Line2 185

QLabel *NmeaLabel;
QLabel *BarSig[32];
QLabel *BarText[32];
int Set_mode = 3;
int meun_select = 0;
GPSDecode *g;
int GPS_CONTROL = 0;
QString Lat,Lon,Accuracy,Altitude;
int Checked = 0;
int thread_run = 0;

char buff[1024];
QString GET_NMEA;
int savenmea = -1;
QString filename;
FILE *fgettime;
int changemode = 0;
int countrun = 0;
int savemode = 0;
QPushButton *nowbutton,*nowbutton2;
int thread_quit = 1;
int exit_window7 = 0;

SatelliteWidget::SatelliteWidget(QWidget *parent) :
    QWidget(parent)
{
    qDebug() << "SatelliteWidget";

    setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    mainLabel = new QLabel(this);
    mainLabel->setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    mainLabel->setStyleSheet("background-color:white");


    for(int i=0;i<32;i++)
    {
        BarSig[i] = new QLabel(this);
        BarText[i] = new QLabel(this);
    }

    which_model[0] = new QLabel(this);
    which_model[1] = new QLabel(this);

    menu_label = new QLabel(this);

    menu = new QPushButton(this);
    beidou = new QPushButton(this);
    gps = new QPushButton(this);
    glonass = new QPushButton(this);
    gps_beidou = new QPushButton(this);
    gps_glonass = new QPushButton(this);
    off = new QPushButton(this);
    Analog = new QPushButton(this);
    SAVE = new QPushButton(this);
    EXIT = new QPushButton(this);

    menu->hide();
    beidou->hide();
    gps->hide();
    glonass->hide();
    gps_beidou->hide();
    gps_glonass->hide();
    off->hide();
    Analog->hide();

    //显示菜单按钮
    menu->setText("mode");
    menu->show();
    menu->raise();
    menu->setGeometry(80,280,80,40);

    qDebug() << "12";
    //显示保存按钮
    QString setText;
    setText = "保存";
    SAVE->setText(setText);
    SAVE->show();
    SAVE->raise();
    SAVE->setFocus();
    SAVE->setGeometry(0,280,70,40);

    //显示退出按钮
    setText = "返回";
    EXIT->setText(setText);
    EXIT->show();
    EXIT->raise();
    EXIT->setGeometry(170,280,70,40);
qDebug() << "34";
    //画线
    line = new QLabel(this);
    line->setStyleSheet("background-color:black");
    line->setGeometry(0, Line1 + 10, SCREEN_WIDTH, 1);

    line1 = new QLabel(this);
    line1->setStyleSheet("background-color:black");
    line1->setGeometry(0, Line1, SCREEN_WIDTH, 1);

    line2 = new QLabel(this);
    line2->setStyleSheet("background-color:black");
    line2->setGeometry(0, Line1 + 20, SCREEN_WIDTH, 1);


    line3 = new QLabel(this);
    line3->setStyleSheet("background-color:black");
    line3->setGeometry(0, Line2, SCREEN_WIDTH, 1);

    line4 = new QLabel(this);
    line4->setStyleSheet("background-color:black");
    line4->setGeometry(0, Line2 + 10, SCREEN_WIDTH, 1);

    line5 = new QLabel(this);
    line5->setStyleSheet("background-color:black");
    line5->setGeometry(0, Line2 + 20, SCREEN_WIDTH, 1);


//    pSubFieldWidget = new SubFieldWidget(this);
//    pSubFieldWidget->setMaxAmount(2);
//    pSubFieldWidget->setText(menuStr);
//    pSubFieldWidget->hide();

    QFont ft;
    ft.setPointSize(7);

    for (int i=0; i<32; i++){
        satellite[i] = new QLabel(this);
        satellite[i]->setAlignment(Qt::AlignCenter);
        satellite[i]->hide();
        satellite[i]->setFont(ft);
    }

    textLabel1 = new QLabel(this);
    textLabel1->setGeometry(0, 0, 110, 20);
    //textLabel1->setAlignment(Qt::AlignCenter);
    textLabel1->setText("经度：");
    textLabel1->raise();

    textLabel2 = new QLabel(this);
    textLabel2->setGeometry(0, 20, 110, 20);
    //textLabel2->setAlignment(Qt::AlignCenter);
    textLabel2->setText("纬度：");
    textLabel2->raise();

    textLabel3 = new QLabel(this);
    textLabel3->setGeometry(0, 40, 110, 20);
    //textLabel3->setAlignment(Qt::AlignCenter);
    textLabel3->setText("高度：");
    textLabel3->raise();

    textLabel4 = new QLabel(this);
    textLabel4->setGeometry(120, 40, 120, 20);
    //textLabel4->setAlignment(Qt::AlignCenter);
    textLabel4->setText("精度：");
    textLabel4->raise();


    //NMEA
    NmeaLabel = new QLabel(this);
    NmeaLabel->setFrameShape (QFrame::Box);
    NmeaLabel->setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(255, 170, 0);");
    NmeaLabel->setGeometry(10, 60, 230, 80);
    NmeaLabel->setText("NMEA");
    NmeaLabel->raise();


 //   QFont font("", 15, 55);
    textLabelR1 = new QLabel(this);
    textLabelR1->setGeometry(80, 0, 160, 20);
    textLabelR1->setAlignment(Qt::AlignCenter);
   // textLabelR1->setFont(16);

    textLabelR2 = new QLabel(this);
    textLabelR2->setGeometry(80, 20, 160, 20);
    textLabelR2->setAlignment(Qt::AlignCenter);
//    textLabelR2->setFont(FONT_BOLD16);

    textLabelR3 = new QLabel(this);
    textLabelR3->setGeometry(60, 40, 60, 20);
    textLabelR3->setAlignment(Qt::AlignCenter);
//    textLabelR3->setFont(FONT_BOLD16);

    textLabelR4 = new QLabel(this);
    textLabelR4->setGeometry(180, 40, 60, 20);
    textLabelR4->setAlignment(Qt::AlignCenter);
//    textLabelR4->setFont(FONT_BOLD16);

    ellLabel = new QLabel(this);
    ellLabel->setGeometry(0, 0, 240, 60);
    ellLabel->setPixmap(pixEll);

    openCloseStatus = new QLabel(this);
    openCloseStatus->setGeometry(0, 0, 240, 60);
    //openCloseStatus->setFont(FONT_BOLD);
    openCloseStatus->setAlignment(Qt::AlignCenter);
  //  openCloseStatus->setStyleSheet("background-color:white");
    openCloseStatus->hide();

    backLabel = new QLabel(this);
    backLabel->setGeometry(52, 72, 135, 135);
    backLabel->setPixmap(pixBack);

    pSonSatelliteWidget = new SonSatelliteWidget(this);
    pSonSatelliteWidget->setGeometry(0, 225, SCREEN_WIDTH, 50);
    pSonSatelliteWidget->show();
    pSonSatelliteWidget->raise();

    connect(menu,SIGNAL(clicked()),this,SLOT(on_pushButton_menu_clicked()));
    connect(beidou,SIGNAL(clicked()),this,SLOT(on_pushButton_beidou_clicked()));
    connect(gps,SIGNAL(clicked()),this,SLOT(on_pushButton_gps_clicked()));
    connect(glonass,SIGNAL(clicked()),this,SLOT(on_pushButton_glonass_clicked()));
    connect(gps_beidou,SIGNAL(clicked()),this,SLOT(on_pushButton_gps_beidou_clicked()));
    connect(gps_glonass,SIGNAL(clicked()),this,SLOT(on_pushButton_gps_glonass_clicked()));
    connect(off,SIGNAL(clicked()),this,SLOT(on_pushButton_off_clicked()));
    connect(Analog,SIGNAL(clicked()),this,SLOT(on_pushButton_Analog_clicked()));
    connect(EXIT,SIGNAL(clicked()),this,SLOT(on_pushButton_Exit_clicked()));
    connect(SAVE,SIGNAL(clicked()),this,SLOT(on_pushButton_Save_clicked()));



    thread_quit = 1;
    MyThread *thread = new MyThread;
    thread->start();

    qDebug() << "focusInEvent";
    g = GPSDecode::getInstance();
    connect(g, SIGNAL(refresh()), this, SLOT(respon()));

    if(GPS_CONTROL == 0)
    {
        g->setMode(Set_mode);
        GPS_CONTROL = 1;
    }
    respon();
}

void SatelliteWidget::refreshLanguage()
{
    qDebug() << "refreshLanguage";
//    textLabel1->setText(FYIII_CONF::SWITCH_LANGUAGE::switchLanguage("DestinationWidget", "Lat"));
//    textLabel2->setText(FYIII_CONF::SWITCH_LANGUAGE::switchLanguage("DestinationWidget", "Lon"));
//    textLabel3->setText(FYIII_CONF::SWITCH_LANGUAGE::switchLanguage("DestinationWidget", "Alititude"));
//    textLabel4->setText(FYIII_CONF::SWITCH_LANGUAGE::switchLanguage("RouteWidget", "LatLon2"));
//    for (int i=0; i<2; i++){
//        pSubFieldWidget->setSonText(FYIII_CONF::SWITCH_LANGUAGE::switchLanguage("SatelliteWidget", menuStr[i].toLatin1().data()), i);
//    }

//    openCloseStatus->setText(FYIII_CONF::SWITCH_LANGUAGE::switchLanguage("SatelliteWidget", "SatelliteClose"));
}

void SatelliteWidget::deriveMousePressEvent(QMouseEvent *event){
//    mTmpPosX = GET_EVENT_POS_X;
}

void SatelliteWidget::deriveMouseMoveEvent(QMouseEvent *event){
//    int posX = GET_EVENT_POS_X;
//    int posY = GET_EVENT_POS_Y;

//    static int sFosX = posX;

//    sFosX = mTmpPosX;
//    if (sFosX-posX > JUDGE_SLIDE_THRE){
//        if (bLRSlide)
//            SatelliteWidgetSlide(SatelliteWidgetIdx, true);

//        return;
//    } else if (sFosX-posX < -JUDGE_SLIDE_THRE){
//        if (bLRSlide)
//            SatelliteWidgetSlide(SatelliteWidgetIdx, false);

//        return;
//    }
//    mTmpPosX = posX;
}

void SatelliteWidget::deriveMouseReleaseEvent(QMouseEvent *event)
{
//    int posX = GET_EVENT_POS_X;
//    int posY = GET_EVENT_POS_Y;
//    if (posX>SCREEN_WIDTH-STATUS_BAR_HEIGHT){
//        if (posY>SCREEN_HEIGHT-STATUS_BAR_HEIGHT){
////            pSubFieldWidget->show();
////            pSubFieldWidget->raise();
////            pSubFieldWidget->setFocus();
//            touchMenu();
//        }
//    }
}

void SatelliteWidget::focusInEvent(QFocusEvent *event)
{
    SAVE->setFocus();
}

void SatelliteWidget::focusOutEvent(QFocusEvent *event)
{
    SAVE->setFocus();
}

void SatelliteWidget::touchMenu()
{
        qDebug() << "touchMenu";
//    pSubFieldWidget->show();
//    pSubFieldWidget->raise();
//    pSubFieldWidget->setFocus();
}
#define PI (3.14159265354)


void SatelliteWidget::on_pushButton_menu_clicked()
{
    qDebug() << "on_pushButton_menu_clicked";
    //隐藏菜单
    menu->hide();

    menu_label->setStyleSheet("background-color:white");
    menu_label->setGeometry(40,80,160,160);
    menu_label->show();
    menu_label->raise();


    beidou->setText("北斗");
    beidou->setGeometry(40,80,80,40);
    beidou->show();
    beidou->raise();

    gps->setText("GPS");
    gps->setGeometry(120,80,80,40);
    gps->show();
    gps->raise();


    glonass->setText("Glonass");
    glonass->setGeometry(40,120,80,40);
    glonass->show();
    glonass->raise();


    gps_beidou->setText("GPS+北斗");
    gps_beidou->setGeometry(120,120,80,40);
    gps_beidou->show();
    gps_beidou->raise();


    QFont font;
    font.setPixelSize(12);
    gps_glonass->setFont(font);
    gps_glonass->setText("GPS+Glonass");
    gps_glonass->setGeometry(40,160,80,40);
    gps_glonass->show();
    gps_glonass->raise();


    off->setText("关闭");
    off->setGeometry(120,160,80,40);
    off->show();
    off->raise();


    Analog->setText("模拟");
    Analog->setGeometry(40,200,80,40);
    Analog->show();
    Analog->raise();


}

void SatelliteWidget::Change_mode()
{
    qDebug() << "focusOutEvent";
    GPSDecode* g = GPSDecode::getInstance();
    disconnect(g, SIGNAL(refresh()), this, SLOT(respon()));

    sleep(500);
    qDebug() << "Change_mod";
    qDebug() << changemode;

    beidou->setFocus();
    if(changemode == 0)
    {
        Set_mode = 5;
        changemode = 1;
    }
    else
    {
        countrun = 0;
        Set_mode = savemode;
        changemode = 0;
        beidou->hide();
        gps->hide();
        glonass->hide();
        gps_beidou->hide();
        gps_glonass->hide();
        off->hide();
        Analog->hide();
        menu_label->hide();

        menu->show();
        menu->raise();
    }

    GPS_CONTROL = 0;
    Checked = 1;

    qDebug() << "focusInEvent";
    g = GPSDecode::getInstance();
    connect(g, SIGNAL(refresh()), this, SLOT(respon()));

    if(GPS_CONTROL == 0)
    {
        g->setMode(Set_mode);
        GPS_CONTROL = 1;
    }
    respon();
}

//设置GPS模块模式 0: GPS 1:北斗 2:GLONASS 3:GPS+北斗 4:GPS+GLONASS 5:关 6:模拟
void SatelliteWidget::on_pushButton_beidou_clicked()
{
    Set_mode = 1;
    savemode = 1;
    Change_mode();
}
void SatelliteWidget::on_pushButton_gps_clicked()
{
    Set_mode = 0;
    savemode = 0;
    Change_mode();
}
void SatelliteWidget::on_pushButton_glonass_clicked()
{
    Set_mode = 2;
    savemode = 2;
   Change_mode();
}

void SatelliteWidget::on_pushButton_gps_beidou_clicked()
{
    Set_mode = 3;
    savemode = 3;
    Change_mode();
}
void SatelliteWidget::on_pushButton_gps_glonass_clicked()
{
    Set_mode = 4;
    savemode = 4;
    Change_mode();
}
void SatelliteWidget::on_pushButton_off_clicked()
{
    Set_mode = 5;
    savemode = 5;
    Change_mode();
}
void SatelliteWidget::on_pushButton_Analog_clicked()
{
    Set_mode = 6;
    savemode = 6;
    Change_mode();
}

void SatelliteWidget::on_pushButton_Save_clicked()
{
    //-1 停止 1开始
    savenmea = -savenmea;

    if(savenmea == 1)
    {
    SAVE->setText("停止");
    fgettime = popen("date +%Y_%m_%d__%H_%M_%S","r");

    memset(buff,'\0',sizeof(buff));
    fread(buff, sizeof(buff), 1, fgettime);
    fclose(fgettime);
    filename = "/launcher/BHC_APP/";
    filename += buff;
    filename = filename.replace("\n","");


    QFile file(filename);
    if(file.exists())
        {
            qDebug()<<"文件存在";
        }else{

           qDebug()<<"文件不存在,正在新建文件.";
            file.open( QIODevice::ReadWrite | QIODevice::Text );
            file.close();

        }
    }
    else
    {
        SAVE->setText("保存");
    }
}

void SatelliteWidget::on_pushButton_Exit_clicked()
{

    thread_quit = 0;
    sleep(1000);//
    exit_window7 = 1;
    //隐藏界面
    delete this;

}

void SatelliteWidget::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MyThread::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void SatelliteWidget::respon()
{
    qDebug() << "respons";
    qDebug() << Set_mode;


    if(changemode == 1)
    {

        qDebug() << countrun++ ;
        if(countrun > 780)
        {
            Change_mode();
        }
    }
    thread_run = 1;
     QFont font;

    char q[256];
    char q1[256];
    char q2[256];
    memset(q1, 0, sizeof(q1));
    memset(q2, 0, sizeof(q2));



    QPixmap pixGreen(20,20);
    pixGreen.fill(Qt::green);

    QPixmap pixYellow(20,20);
    pixGreen.fill(Qt::yellow);
    GPSDecode* g = GPSDecode::getInstance();




  //  tranCoor1(g->getLat(), g->getLon(), q, q1, q2);

     //判断定位状态 A已定位 V未定位
    if (g->getLocation() == 'A'){
        Lat = "纬度：";
        Lat += QString::number(g->getLat(),'f',2);
        textLabel1->setText(Lat);

        Lon = "经度：";
        Lon += QString::number(g->getLon(),'f',2);
        textLabel2->setText(Lon);

        Altitude = "高度：";
        Altitude += QString::number(g->getAltitude(),'f',2);
        textLabel3->setText(Altitude);

        Accuracy = "精度：";
        Accuracy += QString::number(g->getAccuracy(),'f',2);
        textLabel4->setText(Accuracy);
    } else {
        textLabel1->setText("纬度：");
        textLabel2->setText("经度：");
        textLabel3->setText("高度：");
        textLabel4->setText("精度：");

        if (g->getStatus() == 2){   //切换gps模式和模拟模式的状态变量 0: GPS模式 1: 模拟模式 2:关闭模式
            for (int i=0; i<32; i++){
                BarSig[i]->hide();
                BarText[i]->hide();
            }
            return;
        }
    }

    font.setPixelSize(10);

    which_model[0]->setText("");
    which_model[1]->setText("");

    which_model[0]->setFont(font);
    which_model[1]->setFont(font);
    which_model[0]->setGeometry(100,140,240,20);
    which_model[1]->setGeometry(100,215,240,20);


    //显示高度等数据
    textLabel1->raise();
    textLabel2->raise();
    textLabel3->raise();
    textLabel4->raise();

    //红色信号30以下未定位，黄色30以上未定位，绿色已定位
    for(int i=0;i<32;i++)
    {
        BarSig[i]->hide();
        BarText[i]->hide();
        BarSig[i]->setStyleSheet("background-color:red");   //设置红色背景
        BarSig[i]->setGeometry(0,0,0,0);
    }

    //获取最大数量


    for(int i=0;i<g->getSatellites().size();i++)
    {
        BarSig[i]->show();
    }

    int x,y;

    QImage img(9, 50, QImage::Format_RGBA8888);
    QPainter paint(&img);
    paint.translate(9, 50);
    paint.rotate(270);
    font.setPixelSize(11);
    paint.setFont(font);


    int tmpCount = 0;
    //设置GPS模块模式 0: GPS 1:北斗 2:GLONASS 3:GPS+北斗 4:GPS+GLONASS
    int GPS_count = 0;
    int GLONASS_cont = 0;
    int Beidou_count = 0;
    int Analog_count = 0;

    for (int i=0; i<g->getSatellites().size(); i++)
    {
            if (g->getSatellites().at(i).snr < 2) continue;
            BarSig[tmpCount]->show();
            BarSig[tmpCount]->raise();

            calcutorCoor(g->getSatellites().at(i).azimuth, g->getSatellites().at(i).elevation, x, y);

            BarSig[i]->setStyleSheet("background-color:red");

            int sigHeight = 50*g->getSatellites().at(i).snr/50;


            qDebug() << g->getSatellites().at(i).prn;

            BarSig[tmpCount]->setWordWrap(true);
            BarSig[tmpCount]->setAlignment(Qt::AlignTop);
            BarSig[tmpCount]->setFont(font);

            BarSig[tmpCount]->setText(g->getSatellites().at(i).prn);


            if(Set_mode == 6)
            {
                which_model[0]->setText("glonass+beidou");
                which_model[1]->setText("gps");
                //模拟模式
                //GPS+GLONASS+beidou   GLONASS>50
                if(g->getSatellites().at(i).prn[0] == 'b')
                {
                    BarSig[tmpCount]->setGeometry(Analog_count*12,215-sigHeight, 12, sigHeight);
                    Analog_count++;
                }
                else if(g->getSatellites().at(i).prn.toInt() > 50)
                {
                     BarSig[tmpCount]->setGeometry(Analog_count*12,215-sigHeight, 12, sigHeight);
                     Analog_count++;
                }
                else
                {
                     BarSig[tmpCount]->setGeometry(GPS_count*12,245-sigHeight, 12, sigHeight);
                     GPS_count ++;
                }

            }
            else if(Set_mode == 4)
            {
                which_model[0]->setText("glonass");
                which_model[1]->setText("gps");
                //GPS+GLONASS   GLONASS数据较大
                if(g->getSatellites().at(i).prn.toInt() > 50)
                {
                     BarSig[tmpCount]->setGeometry(GLONASS_cont*12,215-sigHeight, 12, sigHeight);
                     GLONASS_cont++;
                }
                else
                {
                     BarSig[tmpCount]->setGeometry(GPS_count*12,245-sigHeight, 12, sigHeight);
                     GPS_count ++;
                }

            }
            else if(Set_mode == 3)
            {
                which_model[0]->setText("beidou");
                which_model[1]->setText("gps");
                //GPS+北斗
                if(g->getSatellites().at(i).prn[0] == 'b')
                {
                     BarSig[tmpCount]->setGeometry(Beidou_count*12,215-sigHeight, 12, sigHeight);
                     Beidou_count++;
                }
                else
                {
                     BarSig[tmpCount]->setGeometry(GPS_count*12,245-sigHeight, 12, sigHeight);
                     GPS_count ++;
                }
            }
            else if(Set_mode == 2)
            {
                which_model[0]->setText("glonass");
                //GLONASS
                BarSig[tmpCount]->setGeometry(GLONASS_cont*12,215-sigHeight, 12, sigHeight);
                GLONASS_cont++;

            }
            else if(Set_mode == 1)
            {
                which_model[0]->setText("beidou");
                //北斗
                BarSig[tmpCount]->setGeometry(Beidou_count*12,215-sigHeight, 12, sigHeight);
                Beidou_count++;

            }
            else if(Set_mode == 0)
            {
                which_model[0]->setText("gps");
                //GPS
                BarSig[tmpCount]->setGeometry(GPS_count*12,215-sigHeight, 12, sigHeight);
                GPS_count ++;

            }
//            BarText[tmpCount]->setFont(font);
//            BarText[tmpCount]->setGeometry(pSonSatelliteWidget->satelliteSig[tmpCount]->geometry().x(),120-sigHeight,12,sigHeight);

            memset(img.bits(), 0, 9*50*4);

            paint.drawText(0, 0, g->getSatellites().at(i).prn);

            BarSig[tmpCount]->raise();


            if (g->getSatellites().at(i).snr > 30)
            {
                BarSig[i]->setStyleSheet("background-color:yellow");
            }
            else
            {
                BarSig[tmpCount]->setStyleSheet("background-color:red");
            }
            tmpCount++;
        }

    for (int i=0; i<g->getSatellitesT().size(); i++)
    {
            for (int j=0; j<g->getSatellites().size();j++)
            {
                if (g->getSatellitesT().at(i).id == g->getSatellites().at(j).prn)
                {
                    BarSig[j]->setStyleSheet("background-color:rgb(126, 255, 25)");
                }
            }
     }


    for (int j=0; j<g->getSatellites().size();j++){
        if (g->getSatellites().at(j).prn == "46" || g->getSatellites().at(j).prn == "48" || g->getSatellites().at(j).prn == "51"){
            //satellite[j]->setText("S");
            memset(img.bits(), 0, 9*50*4);
            paint.drawText(0, 0, QString("S")+QString::number(g->getSatellites().at(j).prn.toInt()+87));
       //     BarText[j]->setPixmap(QPixmap::fromImage(img));
        } else if (g->getSatellites().at(j).prn == "33" || g->getSatellites().at(j).prn == "37" || g->getSatellites().at(j).prn == "39"){
            //satellite[j]->setText("S");
            memset(img.bits(), 0, 9*50*4);
            paint.drawText(0, 0, QString("S")+QString::number(g->getSatellites().at(j).prn.toInt()+87));
        //    BarText[j]->setPixmap(QPixmap::fromImage(img));
        } else if (g->getSatellites().at(j).prn == "42" || g->getSatellites().at(j).prn == "50"){
            //satellite[j]->setText("S");
            memset(img.bits(), 0, 9*50*4);
            paint.drawText(0, 0, QString("S")+QString::number(g->getSatellites().at(j).prn.toInt()+87));
        //    BarText[j]->setPixmap(QPixmap::fromImage(img));
        } else if (g->getSatellites().at(j).prn == "40" || g->getSatellites().at(j).prn == "41"){
            //satellite[j]->setText("S");
            memset(img.bits(), 0, 9*50*4);
            paint.drawText(0, 0, QString("S")+QString::number(g->getSatellites().at(j).prn.toInt()+87));
        //    BarText[j]->setPixmap(QPixmap::fromImage(img));
        } else if (g->getSatellites().at(j).prn == "38" || g->getSatellites().at(j).prn == "53" || g->getSatellites().at(j).prn == "54"){
            //satellite[j]->setText("S");
            memset(img.bits(), 0, 9*50*4);
            paint.drawText(0, 0, QString("S")+QString::number(g->getSatellites().at(j).prn.toInt()+87));
        //    BarText[j]->setPixmap(QPixmap::fromImage(img));
        }
    }

    backLabel->lower();
    mainLabel->lower();

}


MyThread::MyThread()
{

}



void MyThread::run()
{
    while(thread_quit)
    {

        if(1)
        {
            thread_run = 0;
    /********************************************************************/

    //显示nmea数据

        GET_NMEA = COPY_NMEA;
        QFont font;

        //判断是否保存数据
        if(savenmea == 1)
        {
            QFile nmeafile(filename);

            qDebug() << filename;
            nmeafile.open(QIODevice::WriteOnly | QIODevice::Append);


            QTextStream in(&nmeafile);
            in << GET_NMEA <<"\n";
             nmeafile.flush();
            nmeafile.close();

        }

       font.setPixelSize(10);
       NmeaLabel->setFont(font);
       int count_line = 1;
       //获取数据行数
       for(int i=0;i<GET_NMEA.length();i++)
       {
            if(GET_NMEA.data()[i] == '\n')
            {
                count_line++;
            }
       }


       //滑动显示
       int i=0;
       do
       {
            if(GET_NMEA.data()[i] == '\n')
            {

                NmeaLabel->setText(GET_NMEA);
                count_line--;
                if(count_line <= 0) break;
                GET_NMEA =  GET_NMEA.section('\n',1);
                sleep(500);
            }
            i++;
       }
       while(count_line-7 >= 0);

       GET_NMEA.clear();
    }
    /******************************************************************/
    }
}

//#include "mainwidget.h"
//#include "UI/setwidget.h"
//#include "UI/UI_SETTING/setwidget_system.h"
void SatelliteWidget::responMenu(int idx)
{
    qDebug() << "responMenu";
    GPSDecode* g = GPSDecode::getInstance();

    switch (idx){
    case 0: {
        ellLabel->show();
        ellLabel->raise();
        textLabel1->raise();
        textLabel2->raise();
        textLabel3->raise();
        textLabel4->raise();
        textLabelR1->raise();
        textLabelR2->raise();
        textLabelR3->raise();
        textLabelR4->raise();

        openCloseStatus->hide();
        ::system("echo \"SatelliteWidget startGps\" >> ../Log");
//        if (gArgFun.gSetting.gSettingSatellite == 6)
//            gArgFun.gSetting.gSettingSatellite = 3;
//        g->setMode(gArgFun.gSetting.gSettingSatellite);
//        SetWidget_System *pSetWidget_System = gArgFun.gWid->pSetWidget.get()->pSetWidget_System.get();
//        pSetWidget_System->satelliteTextLabel.get()->setText(
//                    pSetWidget_System->satelliteString[gArgFun.gSetting.gSettingSatellite]);
        break;}
    case 1:{
        ellLabel->raise();
        openCloseStatus->show();
        openCloseStatus->raise();
        ::system("echo \"SatelliteWidget stopGps\" >> ../Log");
        g->stopGPS();
//        SetWidget_System *pSetWidget_System = gArgFun.gWid->pSetWidget.get()->pSetWidget_System.get();
//        pSetWidget_System->satelliteTextLabel.get()->setText(
//                    pSetWidget_System->satelliteString[5]);
        for (int i=0; i<32; i++){
            satellite[i]->hide();
            pSonSatelliteWidget->satelliteText[i]->hide();
            pSonSatelliteWidget->satelliteSig[i]->hide();
        }

        textLabelR1->setText(0);
        textLabelR2->setText(0);
        textLabelR3->setText(0);
        textLabelR4->setText(0);
        break;
    }
    }

}

void SatelliteWidget::drawGBL(QLabel *lab, int colorIdx, QString value)
{
//    qDebug() << "drawGBL";
//    switch (colorIdx){
//    case 0:
//        lab->setStyleSheet("border-radius:10px;background-color:red");
//        break;
//    case 1:
//        lab->setStyleSheet("border-radius:10px;background-color:rgb(126, 255, 25)");
//        break;
//    case 2:
//        lab->setStyleSheet("border-radius:10px;background-color:yellow");
//        break;
//    }

//    switch (satelliteIdx){
//    case 0:
//        paint.drawPolygon(gpsPol);
//        break;
//    case 1:
//        paint.drawPolygon(beiPol);
//        break;
//    case 2:
//        paint.drawPolygon(glPol);
//        break;
//    }

    lab->setText(value);
}

#define RAD (60)
void SatelliteWidget::calcutorCoor(double dir, double elev, int &x, int &y)
{
    double tanValue;
    double elevDistance = RAD-RAD*elev/90;

    if (dir==0){
        y = RAD;
        x = 0;
    } else if (dir==90){
        y = 0;
        x = RAD;
    } else if (dir==180){
        y = -RAD;
        x = 0;
    } else if (dir==270){
        y = 0;
        x = -RAD;
    } else if (dir<90){
        tanValue = tan((90-dir)*PI/180);
        x = sqrt(elevDistance*elevDistance/(tanValue*tanValue+1));
        y = tanValue*x;
        y = -y;
    } else if (dir<180){
        tanValue = tan((dir-90)*PI/180);
        x = sqrt(elevDistance*elevDistance/(tanValue*tanValue+1));
        y = tanValue*x;
    } else if (dir<270){
        tanValue = tan((dir-180)*PI/180);
        x = sqrt(elevDistance*elevDistance/(tanValue*tanValue+1));
        y = tanValue*x;
        x = -x;
    } else {
        tanValue = tan((dir-270)*PI/180);
        x = sqrt(elevDistance*elevDistance/(tanValue*tanValue+1));
        y = tanValue*x;
        x = -x;
        y = -y;
    }
//    printf("%d, %d\n", x, y);
}


SonSatelliteWidget::SonSatelliteWidget(QWidget *parent):
    QWidget(parent)
{

    for (int i=0; i<32; i++){
        satelliteSig[i] = new QLabel(this);
        satelliteSig[i]->setGeometry(10*i, 0, 9, 50);
        satelliteSig[i]->hide();
        satelliteText[i] = new QLabel(this);
        satelliteText[i]->setGeometry(i*10, 0, 9, 50);
        satelliteText[i]->hide();
    }
}

void SonSatelliteWidget::deriveMousePressEvent(QMouseEvent *event)
{
//    mX = GET_EVENT_POS_X;
}

void SonSatelliteWidget::deriveMouseMoveEvent(QMouseEvent *event)
{
//    int posX = GET_EVENT_POS_X;
//    setGeometry(geometry().x()+posX-mX, geometry().y(), 20*maxSize, geometry().height());
//    if (geometry().x() > 0){
//        setGeometry(0, geometry().y(), geometry().width(), geometry().height());
//    } else if (geometry().x() < -20*maxSize+SCREEN_WIDTH){
//        setGeometry(-20*maxSize+SCREEN_WIDTH, geometry().y(), 20*maxSize, geometry().height());
//    }
}

void SonSatelliteWidget::deriveMouseReleaseEvent(QMouseEvent *event)
{
}

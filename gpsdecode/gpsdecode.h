#ifndef GPSDECODE_H
#define GPSDECODE_H

#include <QObject>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "gpsdecode/nmea.h"
//#include "grid/k2MapTile.h"
#include <QThread>

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
#include <QDialog>

extern "C"{
    #include "ubx.h"
}


extern QString COPY_NMEA;
class QTimer;
using namespace std;
using namespace nmea;

class GPSDecode : public QThread
{
    Q_OBJECT
public:

    GPSService *gps;

    static GPSDecode *getInstance();

    void setMode(int mode);           //设置GPS模块模式 0: GPS 1:北斗 2:GLONASS 3:GPS+北斗 4:GPS+GLONASS 5:关 6:模拟

    void resetRoute();          //重置关于行程的数据

    double &getLat();   //纬度
    double &getLon();   //经度
    const double &getSpeed();
    const double &getDirect();
    const double &getAltitude();    //高度
    const double &getNavaDirect();      //获取导航时的目标点的方位

    const double &getXte();             //获取偏航距
    const int getNavaTime();        //获取预计在途时间
    const int getFinalNavaTime();       //获取航线导航时的最终在途
    const double &getCurrentDistance();//获取当前距离
    const double &getFinalDistance();       //获取最终距离
    void shutdown();                //关机

    double maxMile;                 //最大路程
    double mMaxMile;                //从出厂到目前的最大里程
    double mMaxTime;                //从出厂到目前的时间
    double mMaxMoveTime;            //从出厂到目前移动的时间

 //   double maxTime;                 //全程平均下的总时间
    double totalUp;                 //累计上升
    double totalDown;               //累计下降

    void setNMEAOut(bool bl);

    double getGPSAverageSnr();      //invalid


    void stopGPS();

    const inline int &getStatus(){
        return switchStatus;
    }

    const inline double &getMileage(){
        return mileage;
    }

    const inline char &getLocation(){
        return gps->fix.location;
    }

    const inline double &getDilution(){
        return gps->fix.dilution;
    }

    //精度
    const inline double &getAccuracy(){
        return gps->fix.accuracy;
    }

    const inline uint8_t &getType(){
        return gps->fix.type;
    }

        //信号质量
    const inline uint8_t &getQuality(){
        return gps->fix.quality;
    }

    GPSTimestamp &getTimeStamp();

    vector<GPSSatellite> &getSatellites();
    vector<II> &getSatellitesT();

    vector<GPSSatellite> getSatellitesGP();
    vector<int> getSatellitesTGP();

    vector<GPSSatellite> getSatellitesGB();
    vector<int> getSatellitesTGB();

    vector<GPSSatellite> getSatellitesGL();
    vector<int> getSatellitesTGL();

    inline void clearGpsUnLocationCount(){
        gpsUnLocationCount = 0;
    }

signals:
    void refresh();             //GPS刷新信号
    void pushLowPowerSignal();        //电池电量低信号
    void pushUnLocationSignal();        //GPS未定位超时信号
    void pushAirportNodeName(QString);                  //到达途径点时触发该信号
    void approchAlarm(QString);                         //接近警告点时触发该信号
    void arriveDestination(QString);                        //到达目标点时触发该信号
    void sigBlueFile(QString);                  //发送蓝牙文信息

public slots:
    void touchGPSSignal();

protected:
    void run();

private:

    int fd;
    double mileage;
public:


//    K2LONLAT lastPos;

private:
    static GPSDecode *instance;
    explicit GPSDecode(QObject *parent = nullptr);
    int openSerial(char *cSerialName);
    void calculate();
    void wake();                                //从休眠模式唤醒
    void saveNode(Log_PositionFix_ *l);                            //保存航点到当前航迹中
    void saveTrackFile();                                           //保存当前航迹文件

    void recordCurrentTrack();                  //记录当前航迹
    void saveCurrentNode();                     //记录当前航迹时的达到条件的点

    //蓝牙接收文件提示相关
private:
    //文件状态
    QString fileState();
    //文件信息
    QMap<QString, QString> fileInfo();
    //文件进度
    QString fileProgress();
    //获取文件内容
    QList<QString> openFile(const QString &path);
    //清空文件内容
    void clearFile();
    //检查 是否接收文件
    void inspectFile();

private:
    NMEAParser *parser;

    string line;
    ifstream file;

    QTimer *t;

    int switchStatus;              //切换gps模式和模拟模式的状态变量 0: GPS模式 1: 模拟模式 2:关闭模式

    int gpsUnLocationCount;             //GPS未定位次数，未定位每过一秒增加一次，重新定位时变为0
    int adcCheckCount;            //ADC查询周期
    bool nmeaOutStatus;                   //nmea输出状态
    int nmeaFd;                         //
    double direct;
    double xte;
    double navaTime;
    double finalNavaTime;
    double currentDistance;
    double finalDistance;
};

#endif // GPSDECODE_H

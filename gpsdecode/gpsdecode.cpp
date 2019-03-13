﻿#include "gpsdecode.h"
#include "dialog.h"

#include <QTimer>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#ifdef Test
    //do nothing
#else
#include <termios.h>
#endif

#include <errno.h>
#include <limits.h>
#include <unistd.h>
//#include "UI/UI_SETTING/conf.h"
//#include "basewidget.h"
//#include "UI/global.h"
//#include "mainwidget.h"
//#include "UI/pressuretemperaturewidget.h"
QString COPY_NMEA = " ";

#define JUDGE_NUM(tmp, num)  \
    if ((tmp) >= ((num)-1)){        \
        (tmp) = 0;            \
    }  else {               \
        (tmp)++;              \
    }

const double &GPSDecode::getCurrentDistance()
{
    qDebug() << "getCurrentDistance";
    return currentDistance;
}

const double &GPSDecode::getFinalDistance()
{
    qDebug() << "getFinalDistance";
    return finalDistance;
}

void GPSDecode::shutdown()
{
    qDebug() << "shutdown";
    double latlon[2] = {gps->fix.latitude, gps->fix.longitude};
   // FYIII_CONF::GPSDECODE::writeLatLon((uchar*)latlon);
    double ff[3];
    ff[0] = mMaxMile;
    ff[1] = mMaxTime;
    ff[2] = mMaxMoveTime;
   // FYIII_CONF::GPSDECODE::writeMeliageTime((UC*)ff);
}

GPSDecode::GPSDecode(QObject *parent)
{
     qDebug() << "GPSDecode";
    parser = new NMEAParser;
    gps = new GPSService(*parser);

    t = new QTimer(this);
    gps->onUpdate += [](){};

//    lastPos.lat = 0;
//    lastPos.lon = 0;
    mileage = 0;

    double latlon[2];
   // FYIII_CONF::GPSDECODE::readLatLon((uchar*)latlon);
    gps->fix.latitude = latlon[0];
    gps->fix.longitude = latlon[1];

    double ff[3];
  //  FYIII_CONF::GPSDECODE::readMeliageTime((UC*)ff);
    mMaxMile = ff[0];
    mMaxTime = ff[1];
    mMaxMoveTime = ff[2];
    maxMile = 0;                 //里程
    totalUp = 0;                 //累计上升
    totalDown = 0;               //累计下降

    gpsUnLocationCount = 0;
}

int GPSDecode::openSerial(char *cSerialName)
{
       qDebug() << "openSerial";
    int iFd;

    struct termios opt;
    ::system("stty -F /dev/ttymxc1 -echo");
    iFd = open(cSerialName, O_RDWR | O_NOCTTY);
    if(iFd < 0) {
        perror(cSerialName);
        return -1;
    }

    tcgetattr(iFd, &opt);

    //cfsetispeed(&opt, B57600);
    //cfsetospeed(&opt, B57600);

     cfsetispeed(&opt, B9600);
     cfsetospeed(&opt, B9600);


    /*
     * raw mode
     */
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]   =   0xFF;
    opt.c_cc[VTIME]  =   150;

    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
        return   -1;
    }

    return iFd;
}

void GPSDecode::calculate()
{
    qDebug() << "calculate";
//    K2LONLAT pos;
//    pos.lat = gps->fix.latitude;
//    pos.lon = gps->fix.longitude;
//    direct = 0;
//    mileage  = Dot_distance_for_guide(lastPos, pos);

    if (mileage>1000)
       mileage = 0;
    if (gps->fix.location == 'A'){
        maxMile += mileage;
        mMaxMile += mileage;
        mMaxTime += 1;
        if (gps->fix.speed >= 1)
            mMaxMoveTime += 1;
    } else {
        maxMile += 0;
        mMaxMile += 0;
        mMaxTime += 0;
    }

    double COS_FACTOR;
    double vectorLatti;
    double vectorLongi;

    double lat = gps->fix.latitude;
    double lon = gps->fix.longitude;

//    if (gArgFun.gDesPos == 1 || gArgFun.gDesPos == 3){
//        COS_FACTOR = cos(lat*PI/180);
//        vectorLatti = gArgFun.gXDesPosLat - lat;
//        vectorLongi = gArgFun.gYDesPosLon - lon;
//        direct = caltulateDirection(vectorLatti, vectorLongi, COS_FACTOR)*180/PI;
//        xte = newdistance(gArgFun.gXDesPosLat, gArgFun.gYDesPosLon, lat, lon, gArgFun.gLat, gArgFun.gLon);
//     //   textLabel[3]->setText(getAngleString(tmp));
//        K2LONLAT pos1 = {lon, lat};
//        K2LONLAT pos2 = {gArgFun.gYDesPosLon, gArgFun.gXDesPosLat};
//        if (Dot_distance_for_guide(pos1, pos2) < gArgFun.gSetting.gSettingNava.range){
//            gArgFun.gMapWid->stopNava();
//            arriveDestination(gArgFun.desName);
//            gArgFun.gDesPos == 0;
//        }
//        currentDistance = Dot_distance_for_guide(pos1, pos2);
//        finalDistance = currentDistance;
//        navaTime = currentDistance/(gps->fix.speed/3.6);
//        finalNavaTime = navaTime;
//    } else if (gArgFun.gDesPos == 2){
//        if (gArgFun.gGPX->gNodeListRtept.size() > 0){
//            gArgFun.gXDesPosLat = gArgFun.gGPX->gNodeListRtept.at(gArgFun.gAirportPoint).lat;
//            gArgFun.gYDesPosLon = gArgFun.gGPX->gNodeListRtept.at(gArgFun.gAirportPoint).lon;
//            COS_FACTOR = cos(lat*PI/180);
//            vectorLatti = gArgFun.gXDesPosLat - lat;
//            vectorLongi = gArgFun.gYDesPosLon - lon;
//            direct = caltulateDirection(vectorLatti, vectorLongi, COS_FACTOR)*180/PI;
//            K2LONLAT pos1 = {lon, lat};
//            K2LONLAT pos2 = {gArgFun.gYDesPosLon, gArgFun.gXDesPosLat};
//            currentDistance = Dot_distance_for_guide(pos1, pos2);
//            if (gArgFun.airportReNave == 1){
//                pos2.lat = gArgFun.gGPX->gNodeListRtept.last().lat;
//                pos2.lon = gArgFun.gGPX->gNodeListRtept.last().lon;
//            } else {
//                pos2.lat = gArgFun.gGPX->gNodeListRtept.first().lat;
//                pos2.lon = gArgFun.gGPX->gNodeListRtept.first().lon;
//            }

//            if (currentDistance < gArgFun.gSetting.gSettingNava.range){
//                pushAirportNodeName(gArgFun.gGPX->gNodeListRtept.at(gArgFun.gAirportPoint).name);

//                if (gArgFun.airportReNave == 1){
//                    gArgFun.gAirportPoint++;

//                } else {
//                    gArgFun.gAirportPoint--;
//                }

//                if (gArgFun.gAirportPoint < 0 || gArgFun.gAirportPoint >= gArgFun.gGPX->gNodeListRtept.size()){
//                    gArgFun.gOldDespos = 0;
//                    gArgFun.gDesPos = 0;
//                    gArgFun.gMapWid->stopNava();
//                } else {
//                    gArgFun.desCurrentName = gArgFun.gGPX->gNodeListRtept.at(gArgFun.gAirportPoint).name;
//                }
//            }
//            double desLon, desLat;
//            if (gArgFun.airportReNave == 1){
//                desLon = gArgFun.gGPX->gNodeListRtept.last().lon;
//                desLat = gArgFun.gGPX->gNodeListRtept.last().lat;
//            } else {
//                desLon = gArgFun.gGPX->gNodeListRtept.first().lon;
//                desLat = gArgFun.gGPX->gNodeListRtept.first().lat;
//            }

//            navaTime = currentDistance/(gps->fix.speed/3.6);
//            K2LONLAT pos11 = {lon, lat};
//            K2LONLAT pos22 = {desLon, desLat};
//            finalDistance = Dot_distance_for_guide(pos11, pos22);
//            finalNavaTime = finalDistance/(gps->fix.speed/3.6);
//        }
//    }

    static double lastAlit = gps->fix.altitude;
    int tmpAli = gps->fix.altitude;
    totalUp += (tmpAli>0)?tmpAli:0;
    totalDown += (tmpAli<0)?-tmpAli:0;
    lastAlit = gps->fix.altitude;

    static bool isFirstApprochAlarm = true;        //

//    K2LONLAT s = {lon, lat};
//    K2LONLAT e;
//    for (int i=0; i<gArgFun.gGPX->gNodeListWpt.size(); i++){
//        if (gArgFun.gGPX->gNodeListWpt.at(i).isWorning){
//            e.lat = gArgFun.gGPX->gNodeListWpt.at(i).lat;
//            e.lon = gArgFun.gGPX->gNodeListWpt.at(i).lon;
//            if (Dot_distance_for_guide(s, e) < gArgFun.gGPX->gNodeListWpt.at(i).worningRange){
//                if (isFirstApprochAlarm){
//                    approchAlarm(gArgFun.gGPX->gNodeListWpt.at(i).name);
//                    isFirstApprochAlarm = false;
//                }
//                break;
//            }
//        }
//        if (i == gArgFun.gGPX->gNodeListWpt.size()-1){
//            isFirstApprochAlarm = true;
//        }
//    }                                               //检测警告点

    static int tmp = 0;
    if (tmp > 10){
        double latlon[2] = {gps->fix.latitude, gps->fix.longitude};
        //FYIII_CONF::GPSDECODE::writeLatLon((uchar*)latlon);
        double ff[3];
        ff[0] = mMaxMile;
        ff[1] = mMaxTime;
        ff[2] = mMaxMoveTime;
       // FYIII_CONF::GPSDECODE::writeMeliageTime((UC*)ff);

        tmp = 0;
    }
    tmp++;

    //lastPos = pos;
}

void GPSDecode::wake()
{
      qDebug() << "wake";
    struct stat buf;
    if(stat("../UBX256.DAT", &buf) != 0)
    {
        printf("stat gets error\n");
        return;
    }

    FILE * fp = fopen("../UBX256.DAT", "r");
    if(fp == NULL)
    {
        printf("file open fail\n");
        return;
    }

    unsigned char str[buf.st_size];//get file data
    fread(str,1,buf.st_size,fp);//read file data

    fclose(fp);
    Log_PositionFix_ *ll = nullptr;
    Log_PositionFix_ l;
    int size = buf.st_size;
//    DestinationNode oldMearsureNode;
//    DestinationNode oldTrackNode;
//    if (gArgFun.gGPX->mearsureTrackNode.size() > 0){
//        oldMearsureNode.lat = gArgFun.gGPX->mearsureTrackNode.last().lat;
//        oldMearsureNode.lon = gArgFun.gGPX->mearsureTrackNode.last().lon;
//    }
//    if (gArgFun.gGPX->tmpTrackNode.size() > 0){
//        oldTrackNode.lat = gArgFun.gGPX->tmpTrackNode.last().lat;
//        oldTrackNode.lon = gArgFun.gGPX->tmpTrackNode.last().lon;
//    }

//    K2LONLAT pos1;
//    K2LONLAT pos2;
    GPSDecode *g = GPSDecode::getInstance();
//    DestinationNode node;
    for (int i=0; i<size-100; i++){
        if (str[i] == 0xb5 && str[i + 1] == 0x62 && str[i + 2] == 0x21 && str[i + 3] == 0x0b){
            ll = getLpf(str+i);
            memcpy(&l, ll, sizeof(Log_PositionFix_));
            printf("lat = %lf, lon = %lf\n", l.latitude, l.longitude);
//            node.lat = l.latitude;
//            node.lon = l.longitude;
//            node.time = QString("%4").arg(l.year, 4, 10, QChar('0'))
//                                          +'-'+QString("%2").arg(l.month, 2, 10, QChar('0'))
//                                          +'-'+QString("%2").arg(l.day, 2, 10, QChar('0'))
//                                          +'T'+QString("%2").arg(l.hour, 2, 10, QChar('0'))
//                                          +'.'+QString("%2").arg(l.min, 2, 10, QChar('0'))
//                                          +'.'+QString("%2").arg(l.sec, 2, 10, QChar('0'))+"Z";

//            node.distanceNext = Dot_distance_for_guide(pos1, pos2);
//            maxMile += node.distanceNext;
//            mMaxMile += node.distanceNext;
            mMaxTime += 1;
            if (l.gSpeed >= 1)
                mMaxMoveTime += 1;

            g->getLat() = l.latitude;
            g->getLon() = l.longitude;

//            if (gArgFun.gMapWid->isMearsureStatus){
//                pos1.lat = oldMearsureNode.lat;
//                pos1.lon = oldMearsureNode.lon;
//                pos2.lat = node.lat;
//                pos2.lon = node.lon;
//                gArgFun.gGPX->mearsureTrackNode.append(node);
//            }

            static int internalTmp = 0;

//            if (gArgFun.gSetting.gSettingTrail.gRecordState == Setting_Idx::Setting_Trail_Idx::open && l.gSpeed > 0){
//                switch (gArgFun.gSetting.gSettingTrail.gRecordMode){
//                case Setting_Idx::Setting_Trail_Idx::automatic:{
//                    int speed = l.gSpeed;
//                    if (speed <  10){
//                        JUDGE_NUM(internalTmp, 5);
//                    } else if (speed < 30){
//                        JUDGE_NUM(internalTmp, 3);
//                    } else {
//                        JUDGE_NUM(internalTmp, 1);
//                    }
//                    if (internalTmp == 0){
//                        saveNode(&l);
//                    }
//                    break;}
//                case Setting_Idx::Setting_Trail_Idx::time:{
//                    switch (gArgFun.gSetting.gSettingTrail.gTimeSpace){
//                    case Setting_Idx::Setting_Trail_Idx::time1:
//                        JUDGE_NUM(internalTmp, 1);
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::time5:
//                        JUDGE_NUM(internalTmp, 5);
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::time10:
//                        JUDGE_NUM(internalTmp, 10);
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::time15:
//                        JUDGE_NUM(internalTmp, 15);
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::time30:
//                        JUDGE_NUM(internalTmp, 30);
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::time60:
//                        JUDGE_NUM(internalTmp, 60);
//                        break;
//                    }
//                    if (internalTmp == 0){
//                        saveNode(&l);
//                    }
//                    break;}
//                case Setting_Idx::Setting_Trail_Idx::distance:{
//                    switch (gArgFun.gSetting.gSettingTrail.gDistanceSpace){
//                    case Setting_Idx::Setting_Trail_Idx::distance5:
//                        if (node.distanceNext > 5){
//                            saveNode(&l);
//                        }
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::distance10:
//                        if (node.distanceNext > 10){
//                            saveNode(&l);
//                        }
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::distance15:
//                        if (node.distanceNext > 15){
//                            saveNode(&l);
//                        }
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::distance20:
//                        if (node.distanceNext > 20){
//                            saveNode(&l);
//                        }
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::distance25:
//                        if (node.distanceNext > 25){
//                            saveNode(&l);
//                        }
//                        break;
//                    case Setting_Idx::Setting_Trail_Idx::distance30:
//                        if (node.distanceNext > 30){
//                            saveNode(&l);
//                        }
//                        break;
//                    }
//                    break;
//                    }
//                }
//            }

//            memcpy(&oldTrackNode, &node, sizeof(DestinationNode));
//            memcpy(&oldMearsureNode, &node, sizeof(DestinationNode));
       }
    }
}

void GPSDecode::saveNode(Log_PositionFix_ *l)
{
    qDebug() << "saveNode";
    QString timeOld;
    QString timeNew;
    int tmpTimeYear;
    int tmpTimeMonth;
    int tmpTimeDay;
    int tmpTimeHour;
    int tmpTimeMin;
    int tmpTimeSes;
    long tmpTime;

//    if (gArgFun.gGPX->tmpTrackNode.size() >= 10000){
//        saveTrackFile();
//        return;
//    }

//    switch (gArgFun.gSetting.gSettingTrail.gSaveMode){
//    case Setting_Idx::Setting_Trail_Idx::fulltrack:
//        if (gArgFun.gGPX->tmpTrackNode.size() >= 10000){
//            saveTrackFile();
//            return;
//        }
//        break;
//    case Setting_Idx::Setting_Trail_Idx::day:
//        timeOld = gArgFun.gGPX->tmpTrackNode.at(0).time;
//        timeNew = gArgFun.gGPX->tmpTrackNode.at(gArgFun.gGPX->tmpTrackNode.size()-1).time;

        tmpTimeYear = timeNew.mid(0, 4).toInt()-timeOld.mid(0, 4).toInt();
        tmpTimeMonth = timeNew.mid(5, 2).toInt()-timeOld.mid(5, 2).toInt();
        tmpTimeDay = timeNew.mid(8, 2).toInt()-timeOld.mid(8, 2).toInt();

        tmpTimeHour = timeNew.mid(11, 2).toInt()-timeOld.mid(11, 2).toInt();
        tmpTimeMin = timeNew.mid(14, 2).toInt()-timeOld.mid(14, 2).toInt();
        tmpTimeSes = timeNew.mid(17, 2).toInt()-timeOld.mid(17, 2).toInt();
        tmpTime = tmpTimeYear*365*24*3600+tmpTimeMonth*30*24*3600+tmpTimeDay*24*3600+tmpTimeHour*3600+tmpTimeMin*60+tmpTimeSes;
//        if (tmpTime > 86400){
//            saveTrackFile();
//            return;
//        } else if (gArgFun.gGPX->tmpTrackNode.size() >= 10000){
//            saveTrackFile();
//            return;
//        }
//        break;
//    case Setting_Idx::Setting_Trail_Idx::week:
//        if (gArgFun.gGPX->tmpTrackNode.size() == 0) return;
//        timeOld = gArgFun.gGPX->tmpTrackNode.at(0).time;
//        timeNew = gArgFun.gGPX->tmpTrackNode.at(gArgFun.gGPX->tmpTrackNode.size()-1).time;

//        tmpTimeYear = timeNew.mid(0, 4).toInt()-timeOld.mid(0, 4).toInt();
//        tmpTimeMonth = timeNew.mid(5, 2).toInt()-timeOld.mid(5, 2).toInt();
//        tmpTimeDay = timeNew.mid(8, 2).toInt()-timeOld.mid(8, 2).toInt();

//        tmpTimeHour = timeNew.mid(11, 2).toInt()-timeOld.mid(11, 2).toInt();
//        tmpTimeMin = timeNew.mid(14, 2).toInt()-timeOld.mid(14, 2).toInt();
//        tmpTimeSes = timeNew.mid(17, 2).toInt()-timeOld.mid(17, 2).toInt();
//        tmpTime = tmpTimeYear*365*24*3600+tmpTimeMonth*30*24*3600+tmpTimeDay*24*3600+tmpTimeHour*3600+tmpTimeMin*60+tmpTimeSes;
//        if (tmpTime > 604800){
//            saveTrackFile();
//            return;
//        } else if (gArgFun.gGPX->tmpTrackNode.size() >= 10000){
//            saveTrackFile();
//            return;
//        }
//        break;
//    }

//    DestinationNode node;
//    node.time = QString("%1").arg(QDateTime::currentDateTime().date().year(), 4, 10, QChar('0'))
//                                  +'-'+QString("%1").arg(QDateTime::currentDateTime().date().month(), 2, 10, QChar('0'))
//                                  +'-'+QString("%1").arg(QDateTime::currentDateTime().date().day(), 2, 10, QChar('0'))
//                                  +'T'+QString("%1").arg(QDateTime::currentDateTime().time().hour(), 2, 10, QChar('0'))
//                                  +':'+QString("%1").arg(QDateTime::currentDateTime().time().minute(), 2, 10, QChar('0'))
//                                  +':'+QString("%1").arg(QDateTime::currentDateTime().time().second(), 2, 10, QChar('0'))+'Z';
//    node.time = QString("%1").arg(l->year, 4, 10, QChar('0'))
//                                  +'-'+QString("%1").arg(l->month, 2, 10, QChar('0'))
//                                  +'-'+QString("%1").arg(l->day, 2, 10, QChar('0'))
//                                  +'T'+QString("%1").arg(l->hour, 2, 10, QChar('0'))
//                                  +':'+QString("%1").arg(l->min, 2, 10, QChar('0'))
//                                  +':'+QString("%1").arg(l->sec, 2, 10, QChar('0'))+'Z';

//    node.ele = 0;
//    node.lat = l->latitude;
//    node.lon = l->longitude;
//    gArgFun.gGPX->addNodeToTmpTrack(node);
//    QString dd = GPX_PATH;
//    dd += TMP_GPX;
//    QFile outputfile(dd);
//    if(!outputfile.open(QIODevice::Append | QIODevice::Text))
//    {
//        //qDebug() << "Open output file failed";
//        return;
//    }

//    QXmlStreamWriter writer(&outputfile);
//    writer.setAutoFormatting(true);
//    writer.writeStartElement("trkpt");
//    writer.writeAttribute("lat", QString::number(node.lat, 'f', 6));
//    writer.writeAttribute("lon", QString::number(node.lon, 'f', 6));
//    writer.writeTextElement("ele", QString::number(node.ele, 'f', 1));
//    writer.writeTextElement("time", node.time);
//    writer.writeEndElement();

//    outputfile.close();
}

void GPSDecode::saveTrackFile()
{
//    gArgFun.gGPX->saveTmpTrackToTrackFile(0, gArgFun.gGPX->tmpTrackNode.first().time);

//    QString cmd = "rm -rf ";
//    QString dd = GPX_PATH;
//    dd += TMP_GPX;
//    ::system((cmd+GPX_PATH+TMP_GPX).toLatin1().data());
//    QFileInfo tmpGpxFile(dd);
//    if (!tmpGpxFile.isFile()){
//        FILE *gpxFp = fopen(dd.toUtf8().data(), "wb+");
//        if (gpxFp == NULL){
//            return;
//        }

//        char *src = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<gpx version=\"1.1\" creator=\"BHCNAV\" xmlns="
//                    "\"http://www.topografix.com/GPX/1/1\">";
//        fwrite(src, strlen(src), 1, gpxFp);
//        fclose(gpxFp);
//    }

//    gArgFun.gGPX->readTmpTrack();
}

void GPSDecode::recordCurrentTrack()
{
    static int internalTmp = 0;

//    if (gArgFun.gSetting.gSettingTrail.gRecordState == Setting_Idx::Setting_Trail_Idx::close){
//        return;
//    }

//    if (gps->fix.location == 'V') return;
//    if (gps->fix.speed < 1) return;

//    switch (gArgFun.gSetting.gSettingTrail.gRecordMode){
//    case Setting_Idx::Setting_Trail_Idx::automatic:{
//        int speed = gps->fix.speed;
//        if (speed <  10){
//            JUDGE_NUM(internalTmp, 5);
//        } else if (speed < 30){
//            JUDGE_NUM(internalTmp, 3);
//        } else {
//            JUDGE_NUM(internalTmp, 1);
//        }
//        if (internalTmp == 0){
//            saveCurrentNode();
//        }
//        break;}
//    case Setting_Idx::Setting_Trail_Idx::time:{
//        switch (gArgFun.gSetting.gSettingTrail.gTimeSpace){
//        case Setting_Idx::Setting_Trail_Idx::time1:
//            JUDGE_NUM(internalTmp, 1);
//            break;
//        case Setting_Idx::Setting_Trail_Idx::time5:
//            JUDGE_NUM(internalTmp, 5);
//            break;
//        case Setting_Idx::Setting_Trail_Idx::time10:
//            JUDGE_NUM(internalTmp, 10);
//            break;
//        case Setting_Idx::Setting_Trail_Idx::time15:
//            JUDGE_NUM(internalTmp, 15);
//            break;
//        case Setting_Idx::Setting_Trail_Idx::time30:
//            JUDGE_NUM(internalTmp, 30);
//            break;
//        case Setting_Idx::Setting_Trail_Idx::time60:
//            JUDGE_NUM(internalTmp, 60);
//            break;
//        }
//        if (internalTmp == 0){
//            saveCurrentNode();
//        }
//        break;}
//    case Setting_Idx::Setting_Trail_Idx::distance:{
//        static K2LONLAT oldPos;
//        K2LONLAT newPos = {gps->fix.longitude, gps->fix.latitude};

//        double dista = Dot_distance_for_guide(oldPos, newPos);
//        switch (gArgFun.gSetting.gSettingTrail.gDistanceSpace){
//        case Setting_Idx::Setting_Trail_Idx::distance5:
//            if (dista > 5){
//                oldPos.lat = gps->fix.latitude;
//                oldPos.lon = gps->fix.longitude;
//                saveCurrentNode();
//            }
//            break;
//        case Setting_Idx::Setting_Trail_Idx::distance10:
//            if (dista > 10){
//                oldPos.lat = gps->fix.latitude;
//                oldPos.lon = gps->fix.longitude;
//                saveCurrentNode();
//            }
//            break;
//        case Setting_Idx::Setting_Trail_Idx::distance15:
//            if (dista > 15){
//                oldPos.lat = gps->fix.latitude;
//                oldPos.lon = gps->fix.longitude;
//                saveCurrentNode();
//            }
//            break;
//        case Setting_Idx::Setting_Trail_Idx::distance20:
//            if (dista > 20){
//                oldPos.lat = gps->fix.latitude;
//                oldPos.lon = gps->fix.longitude;
//                saveCurrentNode();
//            }
//            break;
//        case Setting_Idx::Setting_Trail_Idx::distance25:
//            if (dista > 25){
//                oldPos.lat = gps->fix.latitude;
//                oldPos.lon = gps->fix.longitude;
//                saveCurrentNode();
//            }
//            break;
//        case Setting_Idx::Setting_Trail_Idx::distance30:
//            if (dista > 30){
//                oldPos.lat = gps->fix.latitude;
//                oldPos.lon = gps->fix.longitude;
//                saveCurrentNode();
//            }
//            break;
//        }
//        break;
//        }
//    }
}

void GPSDecode::saveCurrentNode()
{
    QString timeOld;
    QString timeNew;
    int tmpTimeYear;
    int tmpTimeMonth;
    int tmpTimeDay;
    int tmpTimeHour;
    int tmpTimeMin;
    int tmpTimeSes;
    long tmpTime;

//    if (gArgFun.gGPX->tmpTrackNode.size() >= 10000){
//        saveTrackFile();
//        return;
//    }

//    switch (gArgFun.gSetting.gSettingTrail.gSaveMode){
//    case Setting_Idx::Setting_Trail_Idx::fulltrack:
//        if (gArgFun.gGPX->tmpTrackNode.size() >= 10000){
//            saveTrackFile();
//            return;
//        }
//        break;
//    case Setting_Idx::Setting_Trail_Idx::day:
//        timeOld = gArgFun.gGPX->tmpTrackNode.at(0).time;
//        timeNew = gArgFun.gGPX->tmpTrackNode.at(gArgFun.gGPX->tmpTrackNode.size()-1).time;

//        tmpTimeYear = timeNew.mid(0, 4).toInt()-timeOld.mid(0, 4).toInt();
//        tmpTimeMonth = timeNew.mid(5, 2).toInt()-timeOld.mid(5, 2).toInt();
//        tmpTimeDay = timeNew.mid(8, 2).toInt()-timeOld.mid(8, 2).toInt();

//        tmpTimeHour = timeNew.mid(11, 2).toInt()-timeOld.mid(11, 2).toInt();
//        tmpTimeMin = timeNew.mid(14, 2).toInt()-timeOld.mid(14, 2).toInt();
//        tmpTimeSes = timeNew.mid(17, 2).toInt()-timeOld.mid(17, 2).toInt();
//        tmpTime = tmpTimeYear*365*24*3600+tmpTimeMonth*30*24*3600+tmpTimeDay*24*3600+tmpTimeHour*3600+tmpTimeMin*60+tmpTimeSes;
//        if (tmpTime > 86400){
//            saveTrackFile();
//            return;
//        } else if (gArgFun.gGPX->tmpTrackNode.size() >= 10000){
//            saveTrackFile();
//            return;
//        }
//        break;
//    case Setting_Idx::Setting_Trail_Idx::week:
//        if (gArgFun.gGPX->tmpTrackNode.size() == 0) return;
//        timeOld = gArgFun.gGPX->tmpTrackNode.at(0).time;
//        timeNew = gArgFun.gGPX->tmpTrackNode.at(gArgFun.gGPX->tmpTrackNode.size()-1).time;

//        tmpTimeYear = timeNew.mid(0, 4).toInt()-timeOld.mid(0, 4).toInt();
//        tmpTimeMonth = timeNew.mid(5, 2).toInt()-timeOld.mid(5, 2).toInt();
//        tmpTimeDay = timeNew.mid(8, 2).toInt()-timeOld.mid(8, 2).toInt();

//        tmpTimeHour = timeNew.mid(11, 2).toInt()-timeOld.mid(11, 2).toInt();
//        tmpTimeMin = timeNew.mid(14, 2).toInt()-timeOld.mid(14, 2).toInt();
//        tmpTimeSes = timeNew.mid(17, 2).toInt()-timeOld.mid(17, 2).toInt();
//        tmpTime = tmpTimeYear*365*24*3600+tmpTimeMonth*30*24*3600+tmpTimeDay*24*3600+tmpTimeHour*3600+tmpTimeMin*60+tmpTimeSes;
//        if (tmpTime > 604800){
//            saveTrackFile();
//            return;
//        } else if (gArgFun.gGPX->tmpTrackNode.size() >= 10000){
//            saveTrackFile();
//            return;
//        }
//        break;
//    }

//    DestinationNode node;
//    node.time = QString("%1").arg(QDateTime::currentDateTime().date().year(), 4, 10, QChar('0'))
//                                  +'-'+QString("%1").arg(QDateTime::currentDateTime().date().month(), 2, 10, QChar('0'))
//                                  +'-'+QString("%1").arg(QDateTime::currentDateTime().date().day(), 2, 10, QChar('0'))
//                                  +'T'+QString("%1").arg(QDateTime::currentDateTime().time().hour(), 2, 10, QChar('0'))
//                                  +':'+QString("%1").arg(QDateTime::currentDateTime().time().minute(), 2, 10, QChar('0'))
//                                  +':'+QString("%1").arg(QDateTime::currentDateTime().time().second(), 2, 10, QChar('0'))+'Z';
//    node.time = QString("%1").arg(gps->fix.timestamp.year, 4, 10, QChar('0'))
//                                  +'-'+QString("%1").arg(gps->fix.timestamp.month, 2, 10, QChar('0'))
//                                  +'-'+QString("%1").arg(gps->fix.timestamp.day, 2, 10, QChar('0'))
//                                  +'T'+QString("%1").arg(gps->fix.timestamp.hour, 2, 10, QChar('0'))
//                                  +':'+QString("%1").arg(gps->fix.timestamp.min, 2, 10, QChar('0'))
//                                  +':'+QString("%1").arg(gps->fix.timestamp.sec, 2, 10, QChar('0'))+'Z';

//    node.ele = gps->fix.altitude;
//    node.lat = gps->fix.latitude;
//    node.lon = gps->fix.longitude;
//    gArgFun.gGPX->addNodeToTmpTrack(node);
//    QString dd = GPX_PATH;
//    dd += TMP_GPX;
//    QFile outputfile(dd);
//    if(!outputfile.open(QIODevice::Append | QIODevice::Text))
//    {
//        //qDebug() << "Open output file failed";
//        return;
//    }

//    QXmlStreamWriter writer(&outputfile);
//    writer.setAutoFormatting(true);
//    writer.writeStartElement("trkpt");
//    writer.writeAttribute("lat", QString::number(node.lat, 'f', 6));
//    writer.writeAttribute("lon", QString::number(node.lon, 'f', 6));
//    writer.writeTextElement("ele", QString::number(node.ele, 'f', 1));
//    writer.writeTextElement("time", node.time);
//    writer.writeEndElement();

//    outputfile.close();
}

GPSDecode* GPSDecode::instance = nullptr;

GPSDecode* GPSDecode::getInstance()
{

    if (instance == nullptr){

        instance = new GPSDecode;
    }

    return instance;
}


//设置GPS模式
void GPSDecode::setMode(int mode)
{
    qDebug() << "setMode";
    const unsigned char alldata[] = {0xB5,0x62,0x06,0x17,0x14,0x00,0x00,0x41,0x00,0x02,0x00,0x00,0x00,0x00,
                                     0x00,0x00,0x00,0x00,0x00,0x5F,0x00,0x00,0x00,0x00,0x00,0x00,0xD3,0xE7};

    const unsigned char data_gps[] = {0xB5,0x62,0x06,0x3E,0x2C,0x00,0x00,0x00,0x20,0x05,0x00,0x08,0x10,0x00,0x01,
                                      0x00,0x01,0x01,0x01,0x01,0x03,0x00,0x01,0x00,0x01,0x01,0x03,0x08,0x10,0x00,
                                      0x00,0x00,0x01,0x01,0x05,0x00,0x03,0x00,0x00,0x00,0x01,0x01,0x06,0x08,0x0E,
                                      0x00,0x00,0x00,0x01,0x01,0xFD,0x2D};

    const unsigned  char data_glonass[] = {0xB5,0x62,0x06,0x3E,0x2C,0x00,0x00,0x00,0x20,0x05,0x00,0x08,0x10,0x00,0x00,
                                           0x00,0x01,0x01,0x01,0x01,0x03,0x00,0x00,0x00,0x01,0x01,0x03,0x08,0x10,0x00,
                                           0x00,0x00,0x01,0x01,0x05,0x00,0x03,0x00,0x00,0x00,0x01,0x01,0x06,0x08,0x0E,
                                           0x00,0x01,0x00,0x01,0x01,0xFC,0xF1};

    const unsigned  char data_gpsBd[] = {0xB5, 0x62, 0x06, 0x3E, 0x2C, 0x00, 0x00, 0x00, 0x20, 0x05, 0x00, 0x08, 0x10, 0x00, 0x01,
                                         0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x03, 0x08, 0x10, 0x00,
                                         0x01, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x06, 0x08, 0x0E,
                                         0x00, 0x00, 0x00, 0x01, 0x01, 0xFE, 0x41};

     const unsigned char data_bd[] = {0xB5, 0x62, 0x06, 0x3E, 0x2C, 0x00, 0x00, 0x00, 0x20, 0x05, 0x00, 0x08, 0x10, 0x00, 0x00,
                                      0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x08, 0x10, 0x00,
                                      0x01, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x06, 0x08, 0x0E,
                                      0x00, 0x00, 0x00, 0x01, 0x01, 0xFC, 0x01};


     const unsigned char data_gpsGl[] = {0xB5, 0x62, 0x06, 0x3E, 0x2C, 0x00, 0x00, 0x00, 0x20, 0x05, 0x00, 0x08, 0x10, 0x00, 0x01,
                                         0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x03, 0x08, 0x10, 0x00,
                                         0x00, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x06, 0x08, 0x0E,
                                         0x00, 0x01, 0x00, 0x01, 0x01, 0xFE, 0x31};

    close(fd);
    file.close();
    this->quit();
    switchStatus = 0;
    t->stop();
    disconnect(t, SIGNAL(timeout()), this, SLOT(touchGPSSignal()));
    ::system("echo \"GPSDecode setMode\" >> ../Log");
    if (mode > 4){
        ::system("echo 0 >> /sys/class/gpio/gpio11/value");
    } else {
        ::system("echo 1 >> /sys/class/gpio/gpio11/value");
    }


    switch (mode){
    case 0:
        fd = openSerial("/dev/ttymxc1");
        printf("GPS mode\r\n");
        write(fd,data_gps,sizeof(data_gps));
        this->start();
        break;
    case 1:
        fd = openSerial("/dev/ttymxc1");
        printf("Beidou mode\r\n");
        write(fd,alldata,sizeof(alldata));
        sleep(1);
        write(fd,data_bd,sizeof(data_bd));
        this->start();
        break;
    case 2:
        fd = openSerial("/dev/ttymxc1");
        printf("Glonass mode\r\n");
        write(fd,alldata,sizeof(alldata));
        sleep(1);
        write(fd,data_glonass,sizeof(data_glonass));
        this->start();
        break;
    case 3:
        ::system("echo \"GPSDecode data_gpsBd start\" >> ../Log");
        fd = openSerial("/dev/ttymxc1");
        printf("GPS+Bedou mode\r\n");
        write(fd,alldata,sizeof(alldata));
        sleep(1);
        write(fd,data_gpsBd,sizeof(data_gpsBd));
        this->start();
        ::system("echo \"GPSDecode data_gpsBd stop\" >> ../Log");
        break;
    case 4:
        fd = openSerial("/dev/ttymxc1");
        printf("GPS+GL mode\r\n");
        write(fd,alldata,sizeof(alldata));
        sleep(1);
        write(fd,data_gpsGl,sizeof(data_gpsGl));
        this->start();
        break;
    case 5:
        t->start(1000);
        connect(t, SIGNAL(timeout()), this, SLOT(touchGPSSignal()));
        switchStatus = 2;
        gps->fix.location = 'V';
        break;
    case 6:
        switchStatus = 1;
       // file.open(GPS_ANALOG_PATH);
        t->start(1000);
        connect(t, SIGNAL(timeout()), this, SLOT(touchGPSSignal()));
        break;
    }
    this->quit();
}

void GPSDecode::resetRoute()
{
    qDebug() << "resetRoute";
    maxMile = 0;                 //最大路程
    mMaxMile = 0;                //从出厂到目前的最大里程
 //   maxTime = 0;                 //全程平均下的总时间
    mMaxTime = 0;
    totalUp = 0;                 //累计上升
    totalDown = 0;               //累计下降
    mMaxMoveTime = 0;

    //stopTime = 0;                //在停止状态下的总时间
    double ff[3];
    memset(ff, 0, sizeof(ff));
   // FYIII_CONF::GPSDECODE::writeMeliageTime((UC*)ff);
}

double &GPSDecode::getLat()
{
    return gps->fix.latitude;
}

double &GPSDecode::getLon()
{
    return gps->fix.longitude;
}

const double &GPSDecode::getSpeed()
{
    return gps->fix.speed;
}

const double &GPSDecode::getDirect()
{
    return gps->fix.direct;
}

const double &GPSDecode::getAltitude()
{
//    if (gArgFun.gSetting.gSettingAlititude){
//        return gArgFun.gWid->pPressureTemperatureWidget.get()->pCalibrateAltitude->getPressureAltitude();
//    }
    return gps->fix.altitude;
}

const double &GPSDecode::getNavaDirect()
{
    return direct;
}

const double &GPSDecode::getXte()
{
    return xte;
}

const int GPSDecode::getNavaTime()
{
    return (int)navaTime;
}

const int GPSDecode::getFinalNavaTime()
{
    return finalNavaTime;
}

void GPSDecode::setNMEAOut(bool bl)
{
    nmeaOutStatus = bl;
    close(nmeaFd);
    if (nmeaOutStatus){
        nmeaFd = openSerial("/dev/ttyGS0");
        if(nmeaFd < 0) {
            return;
        }
    }
}

double GPSDecode::getGPSAverageSnr()
{
    return gps->fix.almanac.averageSNR();
}

void GPSDecode::stopGPS()
{
    switchStatus = false;
    gps->fix.almanac.clear();
    this->setMode(5);
}

GPSTimestamp& GPSDecode::getTimeStamp()
{
    return gps->fix.timestamp;
}

vector<GPSSatellite>& GPSDecode::getSatellites()
{
    return gps->fix.almanac.satellites;
}

vector<II>& GPSDecode::getSatellitesT()
{
    return gps->fix.almanac.satellitesT;
}

//vector<GPSSatellite> GPSDecode::getSatellitesGP()
//{
//    return gps->fix.almanac.satellitesGP;
//}

//vector<int> GPSDecode::getSatellitesTGP()
//{
//    return gps->fix.almanac.satellitesTGP;
//}

//vector<GPSSatellite> GPSDecode::getSatellitesGB()
//{
//    return gps->fix.almanac.satellitesGB;
//}

//vector<int> GPSDecode::getSatellitesTGB()
//{
//    return gps->fix.almanac.satellitesTGB;
//}

//vector<GPSSatellite> GPSDecode::getSatellitesGL()
//{
//    return gps->fix.almanac.satellitesGL;
//}

//vector<int> GPSDecode::getSatellitesTGL()
//{
//    return gps->fix.almanac.satellitesTGL;
//}

//#include "UI/backlight.h"
//#include "basewidget.h"

void GPSDecode::touchGPSSignal()
{
    string line;
    string src;

    char tmpStr[1024];
    try {
        if (switchStatus == 0){
            memset(tmpStr, 0, sizeof(tmpStr));
            read(fd, tmpStr, sizeof(tmpStr));


         COPY_NMEA = tmpStr;

            src = tmpStr;

//            FILE *fp = fopen("/launcher/BHC_USER/nmea.dat", "at+");
//            fseek(fp, 0, SEEK_END);
//            fwrite(tmpStr, strlen(tmpStr), 1, fp);
//            fclose(fp);

            for (int i=0; i<12;i++){
                line = src.substr(0, src.find("\n", 1));
                src.erase(0, src.find("\n", 1)+1);

                if (nmeaOutStatus){
                    //printf("%s\n", line.c_str());
                    write(nmeaFd, line.c_str(), line.size());
                    write(nmeaFd, "\n", 1);
                }

                parser->readLine(line);
                if (line.find("GPGLL") != -1){
                    break;
                }
                if (line.find("GNGLL") != -1){
                    break;
                }
                if (line.find("GLGLL") != -1){
                    break;
                }
                if (line.find("GBGLL") != -1){
                    break;
                }
            }
        } else if (switchStatus == 1){
            while (1){
                if (!file.is_open()) break;

                getline(file, line);

                if (nmeaOutStatus){
            //        printf("%s\n", line.c_str());
//                    write(nmeaFd, line.c_str(), line.size());
//                    write(nmeaFd, "\n", 1);
                }

                parser->readLine(line);
                if (line.find("GPGLL") != -1){
                    break;
                }
                if (line.find("GNGLL") != -1){
                    break;
                }
                if (line.find("GBGLL") != -1){
                    break;
                }
                if (line.size() == 0){
                   file .close();
                 //  file.open(GPS_ANALOG_PATH);
                }
            }
        } else {
            gps->fix.location = 'V';
            refresh();
            return;
        }
    } catch (NMEAParseError& e){
        cout << e.message << endl << endl;
        return;
        // You can keep feeding data to the gps service...
        // The previous data is ignored and the parser is reset.
    }


    refresh();


}

void GPSDecode::run()
{
    qDebug() << "run";
#ifdef QT_PC
    return;
#endif
    while (1){
        if (switchStatus){
            return;
        }
        touchGPSSignal();
    }
}













QString GPSDecode::fileState()
{
//    const QString strState = BT_STATE;

//    QList<QString> strList = openFile(strState);
//    if(strList.isEmpty()){return "";}

//    //状态值
//    QString value = strList.at(0);
//    return value;
}

QMap<QString,QString> GPSDecode::fileInfo()
{
//    qDebug()<<"GPSDecode::"<<__FUNCTION__<<endl;
//    printf("GPSDecode::fileInfo() =====================\n");
//    const QString strProgress = BT_FILEINFO;

//    QList<QString> strList = openFile(strProgress);

////    printf("GPSDecode::fileInfo() =================strList= %d \n",strList.size());
//    QMap<QString,QString> values;
//    if(strList.isEmpty()){return values;}

//    const int size = strList.size();
//    for(int i = 0 ; i < size ; i++){
//        const int index = strList.at(i).indexOf(":");
//        QString key = strList.at(i).mid(0,index+1);
//        QString value = strList.at(i).mid(index+1);
//        values.insert(key,value);
//    }
////    qDebug()<<"GPSDecode::"<<__FUNCTION__<<"values ="<<value<<endl;
////    printf("BaseWidget::fileInfo() =================values= %d \n",values.size());
//    return values;
}

QString GPSDecode::fileProgress()
{
//    const QString progress = BT_PROGRESS;

//    QList<QString> strList = openFile(progress);
//    if(strList.isEmpty()){return "";}
//    //进度值
//    QString value = strList.at(0);

//    return value;
}

QList<QString> GPSDecode::openFile(const QString &path)
{
    QFile file(path);
    QList<QString> strList;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
//        qDebug()<<"GPSDecode::"<<__FUNCTION__<<"open File error"<<endl;
//        printf("GPSDecode::openFile() =================values= %s \n",path.toStdString().c_str());
        file.close();
        return strList;
    }

    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str(line);
        qDebug()<< str;
        str.replace("\n","");
        strList.append(str);
    }
    file.close();
    return strList;
}

void GPSDecode::clearFile()
{
//    QFile file(BT_STATE);
//    if (file.exists())
//    {
//        file.remove();
//    }
//    file.close();
//    QFile file1(BT_FILEINFO);
//    if (file1.exists())
//    {
//        file1.remove();
//    }
//    file1.close();
//    QFile file2(BT_PROGRESS);
//    if (file2.exists())
//    {
//        file2.remove();
//    }
//    file2.close();
}

void GPSDecode::inspectFile()
{
//    printf("GPSDecode::inspectFile() =====================\n");
    //由文件信息方可操作
    QMap<QString,QString >mmap = fileInfo();
    if(mmap.isEmpty()){return;}

    //检查文件状态 1 正在传输  2 文件传输完毕  -1 传输错误
    const QString state = fileState();
//    printf("GPSDecode::inspectFile() ===========fileState = %d \n",state.toInt());
    if(state == "1"){
        //进度

    }else if(state == "2"){
//        gArgFun.blueReceiveDialog = true;
//        //传输完毕   <idReceiveSuccess>文件接收成功</idReceiveSuccess>
//        printf("GPSDecode::inspectFile() dilog show() 1 \n");
//        emit sigBlueFile(FYIII_CONF::SWITCH_LANGUAGE::switchLanguage("ShareWidget", "idReceiveSuccess"));
//        printf("GPSDecode::inspectFile() dilog show() 2 \n");
    }else if(state == "-1"){
//        gArgFun.blueReceiveDialog = true;
//        //传输错误
//        //提示失败              <idReceiveFail>文件接收失败</idReceiveFail>
//        printf("GPSDecode::inspectFile() dilog show() 3 \n");
//        emit sigBlueFile(FYIII_CONF::SWITCH_LANGUAGE::switchLanguage("ShareWidget", "idReceiveFail"));
//        printf("GPSDecode::inspectFile() dilog show() 4 \n");
    }

    const QString fileName = mmap.value("文件名称:");
    const int len = fileName.length();
//    printf("GPSDecode::inspectFile() fileName = %s \n",fileName.toStdString().c_str());
    if(fileName.endsWith(".gpx")){
        QString strName = fileName.mid(0,len-4);

        // .pgx 文件需要转移到 GPX文件夹下
        QString m_target = "/launcher/BHC_USER/GPX/";//目标路径
        QString path = "/launcher/BHC_USER/Bluetooth/";//源路径
        QString strPath = path;
        strPath.append(fileName);
        int cont = 1;
        while(true){
            QString fileFullName = m_target;
            fileFullName.append(strName);
            fileFullName.append(".gpx");
            //1 检查文件是否存在
            QFileInfo fileInfo(fileFullName);
            if(!fileInfo.isFile())
            {
//                printf("GPSDecode::inspectFile() strPath = %s \n",strPath.toStdString().c_str());
//                printf("GPSDecode::inspectFile() fileFullName = %s \n",fileFullName.toStdString().c_str());
//                printf("GPSDecode::inspectFile() strName = %s \n",strName.toStdString().c_str());
//                printf("GPSDecode::inspectFile() cont = %d \n",cont);
                //创建(复制)
                QFile file;
                if(file.copy(strPath,fileFullName)){
                    //若成功 则 删除 源
                    QFile file2(strPath);
                    if (file2.exists()){
                        file2.remove();
                    }
                    file2.close();
                    //通知刷新
                  //  gArgFun.gGPX->readGPX(strName.append(".gpx"));
                }
                file.close();

                break;
            }else{
                strName = fileName.mid(0,len-4);
                strName.append("_").append(QString::number(cont));
                cont++;
            }
        }


    }

    //清除
    clearFile();
}







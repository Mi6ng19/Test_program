#include "dialog.h"
#include "ui_dialog.h"

#define WIDTH 240
#define HEIGHT 320


static QString text="1234567"; //设置显示的文本
static QString text2="1234567";
static FILE *fp;
static char buff[1024]={0};
static char *num1, *num2, *num3;
static QString Window;
static int Beep_control = 0;
static int Flash_Light_control = 0;
static int Bluetooth_control = 0;
static int LCD_control = 0;
static int Key_control = 0;
static int Disk_control = 0;
static int Touchscreen_control = 0;
static int Camera_control = 0;
static int GPS_control  = 0;
static int Slider_control = 0;
static int Key_Data[13] = {0};
static int Checked_OK = 0;
static int Exit_Key_Test = 0;
static QPushButton * current_button;
static QProcess *process;
static QImage img(240, 320, QImage::Format_RGB16);
static int TF_exist = 0;
static float number1,number2,number3;
static float accx,accy,accz,magx,magy,magz,Xh,Yh,roll,pitch,angle;
static int totalAngle = 0,baseAngle = 0,currentAngle = 0;
static bool HorizontalCalibrateFlg = false;
static float percent = 0;
static int ax,ay,az,ar;
static QList<double> collect_Xx;
static QList<double> collect_Yy;
static QList<double> collect_Zz;
static char brightness[50];


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    pSlider = new QSlider(this);
    pSlider->hide();
    pSpinBox = new QSpinBox(this);
    pSpinBox->hide();

    connect(this,SIGNAL(setui()),this,SLOT(timetostart()));
    connect(parent,SIGNAL(sendData(QString)),this,SLOT(ReceiveData(QString)));
    startTimer(500);
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::timerEvent(QTimerEvent *)
{
    emit setui();
}

//选择所要进入的窗口
void Dialog::timetostart()
{
    switch(Window.toInt())
    {
        case 1: window_1(); break;
        case 2: window_2(); break;
        case 3: window_3(); break;
        case 4: window_4(); break;
        case 5: window_5(); break;
        case 6: window_6(); break;
        case 7: window_7(); break;
        case 8: window_8(); break;
        case 9: window_9(); break;
    }
}

void Dialog::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void Dialog::keyPressEvent(QKeyEvent *keyev)
{
    int keyValue = keyev->key();
    qDebug() << keyValue;

    if(keyev->type()==QKeyEvent::KeyPress)
    {
        if(Window.toInt() == 8)
        {
             process->close();
             this->raise();
        }

        if(Window.toInt() == 2 && Key_control == 0)
        {
            Checked_OK = 1;
            switch(keyValue)
            {
                case Key_Up:ui->pushButton_7->setText(QString::number(++Key_Data[1]));break;
                case Key_Down:ui->pushButton_8->setText(QString::number(++Key_Data[2]));break;
                case Key_Left:ui->pushButton_9->setText(QString::number(++Key_Data[3]));break;
                case Key_Right:ui->pushButton_10->setText(QString::number(++Key_Data[4]));break;

                case Key_Menu:ui->pushButton_11->setText(QString::number(++Key_Data[5]));break;
                case Key_return:ui->pushButton_12->setText(QString::number(++Key_Data[6]));break;
                case Key_Location:ui->pushButton_18->setText(QString::number(++Key_Data[7]));break;
                case Key_Ok:ui->pushButton_13->setText(QString::number(++Key_Data[8]));break;
                case Key_Add:ui->pushButton_14->setText(QString::number(++Key_Data[9]));Exit_Key_Test++ ;break;
                case Key_Sub:ui->pushButton_15->setText(QString::number(++Key_Data[10]));Exit_Key_Test++ ;break;

                case Key_Power:ui->pushButton_16->setText(QString::number(++Key_Data[11]));break;
                case Key_Find:ui->pushButton_17->setText(QString::number(++Key_Data[0]));break;
            }
        }

        if((LCD_control == 0 && Window.toInt() == 4) || (Bluetooth_control == 0 && Window.toInt() == 3)/
                (GPS_control == 0 && Window.toInt() == 7) || (Camera_control    == 0 && Window.toInt() == 8))
        {
            keyev->ignore();
        }
        else {
            //模拟当前焦点所在按钮点击事件
            if(Checked_OK == 0)
            {
                qDebug() << "back";
                QWidget *current_focus_w = this->focusWidget();//获取当前焦点的控件
                if(!current_focus_w)
                    return;
                current_button = qobject_cast<QPushButton*>(current_focus_w);

                if(current_button && keyValue == Key_Ok)
                {
                    qDebug() << current_button->text();
                    current_button->clicked();
                }
            }
        }

    }
}

void Dialog::keyReleaseEvent(QKeyEvent *keyev)
{
    int keyValue = keyev->key();

    if(keyev->type()==QKeyEvent::KeyRelease)
    {
        if(Window.toInt() == 2 && Key_control == 0)
        {
            switch(keyValue)
            {
                case Key_Add:Exit_Key_Test-- ;break;
                case Key_Sub:Exit_Key_Test-- ;break;
            }
        }
    }

}

void Dialog::mousePressEvent(QMouseEvent *event)
{

}

void Dialog::Get_Info(char *cmd,char *prefix)
{
#ifdef Test
    fp = popen("echo -200,-5227,-15076","r");
#else
    fp = popen(cmd,"r");
#endif
    memset(buff,'\0',sizeof(buff));
    fread(buff, sizeof(buff), 1, fp);
    fclose(fp);

    text.clear();
    text += prefix;
    text += buff;
}

void Dialog::Get_BatteryInfo()
{
    //获取电池状态
    Get_Info("cat /sys/class/system_info/fy_info/battery_type","电池状态: \n");
    num1 = strtok(buff, ",");
    num2 = strtok(NULL, ",");
    num3 = strtok(NULL, ",");
    text.clear();

    text += "电池状态: \n电池类型: ";
#ifdef Test
    num1[0]='1';
    num2[0]='1';
    strcpy(num3,"124");
#endif
    switch (num1[0])	//电池类型
    {
        case '1':text += "USB";		break;
        case '2':text += "2aa";		break ;
        case '3':text += "li" ;		break ;
        case '4':text += "USB+2aa";	break ;
        case '5':text += "USB+li";	break ;
    }
    text += "\n充电状态: ";
    switch (num2[0])	//充电状态
    {
        case '1':text += "未充电" ;		break ;
        case '2':text += "正在充电" ;		break ;
        case '3':text += "电池充满" ;		break ;
    }
    text += "\n电池电压: ";
    text += num3;
    ui->label->setText(text);
}

void Dialog::Get_BluetoothInfo()
{
    strcpy(buff,"hello");
    if(Bluetooth_control == 0)
    {
        ui->label->setText("正在开启蓝牙，请误操作\n");

        //初始化蓝牙
        ui->label->setText("正在初始化蓝牙\n");
        system("sh /etc/bluz_close.sh");
        system("sh /etc/bluz_init.sh");
        Get_Info("hciconfig","蓝牙");
        sleep(500);

        if(strlen(buff) < 10 && Bluetooth_control == 0)
        {
            ui->label->setText("蓝牙开启失败！\n");
            Bluetooth_control = 0;
        }
        else
        {
            Bluetooth_control = 1;
            ui->label->setText("蓝牙开启成功！\n");
            ui->label->setText("正在扫面周围设备！\n");
            system("hciconfig hci0 up");    //开启蓝牙


    #ifdef Test
            strcpy(buff,"'Bluetooth'");
    #else
            //获取蓝牙名称
            Get_Info("hciconfig hci0 name","蓝牙名称:");
    #endif
            strtok(buff,"'");
            num1=strtok(NULL,"'");
            text2.clear();
            text2 += "蓝牙名称: ";
            text2 += num1;
            text2 += "\n蓝牙扫描:\n";
    #ifdef Test
            text += "Scanning ...\n\t用户地址\t用户名称";
    #else
            //获取周围蓝牙信息
            Get_Info("hcitool scan","");
    #endif
            text = text.replace("Scanning ...\n","");
            text = text.replace("\t"," ");
            text2 += text;

            text2 = text2;
            ui->label->setText(text2);

            Bluetooth_control = 1;
        }
    }

}

void Dialog::Get_DisInfo(float* Info)
{
    float avaliable = 0,total = 0;
    QList<QStorageInfo> list = QStorageInfo::mountedVolumes();

    for(QStorageInfo& si : list)
    {
    qDebug() << "Display Name: " << si.displayName() << "Bytes Avaliable: " << si.bytesAvailable()<< "Bytes Free: " << si.bytesFree()<< "Bytes Total: " << si.bytesTotal();

        if(QString::compare(si.displayName(),"/mnt") == 0)
        {
            TF_exist = 1;

            Info[2] = si.bytesAvailable() / 1024 / 1024 / 1024;
            Info[3] = si.bytesTotal() / 1024 / 1024 / 1024;
        }
        else {
            avaliable += si.bytesAvailable();
            total += si.bytesTotal();
        }
    }

    //to GB
    Info[0] = avaliable / 1024 / 1024 / 1024;
    Info[1] = total / 1024 / 1024 / 1024;

      qDebug() << Info[0] << "  " << Info[1];
}

void Dialog::paintEvent(QPaintEvent *event)
{
    if(Camera_control == 1 && Window.toInt() == 8)
    {
#ifdef Test
        //do no thing
#else
           memset(img.bits(), 0, sizeof(img.bits()));
           getRGV16Imag(img.bits());
#endif


        Q_UNUSED(event);
        QPainter painter(this);

        // 绘制图片
        painter.drawImage(0, 0, img);
        update();
    }
}

//计算角度
int Dialog::Mag_calc_angle()
{
    //calibrate here!
    magx -= ax;
    magy -= ay;
    magz -= az;

    collect_Xx.append(magx);
    collect_Yy.append(magy);
    collect_Zz.append(magz);

    roll   = atan2(accx,    sqrt((accy * accy) + (accz * accz)));
    pitch  = atan2(accy,    sqrt((accx * accx) + (accz * accz)));

    Xh = magx * cos(pitch) + magy * sin(roll) * sin(pitch) - magz * cos(roll) * sin(pitch);
    Yh = magy * cos(roll) + magz * sin(roll);

    /*根据不同象限进行计算*/
   if(Xh > 0 && Yh > 0)
   {
       angle = 360 - atan(Yh/Xh)*180/M_PI;
   }
   else if(Xh > 0 && Yh < 0)
   {
       angle = -(atan(Yh/Xh)*180/M_PI);
   }
   else if(Xh < 0)
   {
       angle = 180 - atan(Yh/Xh)*180/M_PI;
   }
   else if(Xh == 0 && Yh < 0)
   {
       angle = 90;
   }
   else if(Xh == 0 && Yh > 0)
   {
       angle = 270;
   }
   angle = 360 - angle;
   sprintf(buff,"翻滚角：%-.2f\n俯仰角：%-.2f\n水平角：%d",roll*180/M_PI,pitch*180/M_PI,(int)angle);
   text = buff;
   ui->label_5->setText(text);
   return (int)angle;

}

//角度校准算法
int Dialog::Calibration_algorithm(int *center_x, int *center_y, int *radius, QList<double> paramOne, QList<double> paramTwo)
{
    int i = 0;

    double sum_x = 0.0f, sum_y = 0.0f;
    double sum_x2 = 0.0f, sum_y2 = 0.0f;
    double sum_x3 = 0.0f, sum_y3 = 0.0f;
    double sum_xy = 0.0f, sum_x1y2 = 0.0f, sum_x2y1 = 0.0f;

    double J= 0.0f, D= 0.0f, E= 0.0f, G= 0.0f, H= 0.0f;
    double a= 0.0f, b= 0.0f, c= 0.0f;

    double x = 0.0f;
    double y = 0.0f;
    double x2 = 0.0f;
    double y2 = 0.0f;

    //int N = points.size();
    for (i = 0; i < paramOne.size(); i++)
    {
         x = paramOne.at(i);
         y = paramTwo.at(i);
         x2 = x * x;
         y2 = y * y;
        sum_x += x;
        sum_y += y;
        sum_x2 += x2;
        sum_y2 += y2;
        sum_x3 += x2 * x;
        sum_y3 += y2 * y;
        sum_xy += x * y;
        sum_x1y2 += x * y2;
        sum_x2y1 += x2 * y;
    }

    J = paramOne.size() * sum_x2 - sum_x * sum_x;
    D = paramOne.size() * sum_xy - sum_x * sum_y;
    E = paramOne.size()* sum_x3 + paramOne.size() * sum_x1y2 - (sum_x2 + sum_y2) * sum_x;
    G = paramOne.size() * sum_y2 - sum_y * sum_y;
    H = paramOne.size() * sum_x2y1 + paramOne.size()* sum_y3 - (sum_x2 + sum_y2) * sum_y;

    if (0 == (J * G - D * D))
    {
        return -1;
    }

    if (0 == (D * D - G * J))
    {
        return -1;
    }
    a = (H * D - E * G) / (J * G - D * D);
    b = (H * J - E * D) / (D * D - G * J);
    c = -(a * sum_x + b * sum_y + sum_x2 + sum_y2) / paramOne.size();

    *center_x = (int)a / (-2);
    *center_y = (int)b / (-2);
    *radius = (int)sqrt(a * a + b * b - 4 * c) / 2;
    return 0;

}

//校准过程
int Dialog::CalibrateProces()
{
        if(true == HorizontalCalibrateFlg){
            if(totalAngle > 360){
                HorizontalCalibrateFlg = false;
                totalAngle = 0;
                baseAngle = 0;
                currentAngle = 0;
                Calibration_algorithm(&ax,&ay,&ar,collect_Xx,collect_Yy);
                char log1[128] = {0};
                sprintf(log1, "calibrate done! after calibtate: ax = %d\t ay = %d az = %d\n", ax,ay,az);
                printf("%s\n", log1);
                printf("collect %d points!\n", collect_Xx.size());
            }else{
                currentAngle = Mag_calc_angle();
                printf("currentAngel = %d\nbaseAngle = %d\n", currentAngle, baseAngle);
                int tmpAngle = abs(currentAngle - baseAngle);

                //Filter invalid points
                if(tmpAngle > 10){
                    removeInvalidData();
                    totalAngle += 0;
                }else
                    totalAngle += tmpAngle;

                baseAngle = currentAngle;

                printf("totalAngle = \t%d\n", (int)totalAngle);
                return currentAngle;
            }
        }

}

void Dialog::removeInvalidData()
{
    collect_Xx.removeLast();
    collect_Yy.removeLast();
    collect_Zz.removeLast();
}

void Dialog::setCalibrateFlag()
{
    baseAngle = Mag_calc_angle();
    currentAngle = Mag_calc_angle();
    totalAngle = 0;
    ax = 0;
    ay = 0;
    az = 0;
    ar = 0;
    collect_Xx.clear();
    collect_Yy.clear();
    collect_Zz.clear();
    HorizontalCalibrateFlg = true;
}

//传感器检测
void Dialog::window_1()
{
    //设置label 显示位置和大小
    ui->label->setGeometry(rect().x()+30, rect().y()+10,120, 41);
    ui->label_2->setGeometry(rect().x()+30, rect().y()+40,160, 41);
    ui->label_3->setGeometry(rect().x()+30, rect().y()+70,50, 81);
    ui->label_4->setGeometry(rect().x()+130, rect().y()+70,50, 81);
    ui->label_5->setGeometry(rect().x()+30, rect().y()+151,180, 81);
    ui->pushButton_5->setGeometry(rect().x()+150, rect().y()+160,80, 80);

    if(HorizontalCalibrateFlg == false)
    {
        ui->pushButton_5->setText("开始校准");
    }
    else
    {
        percent = totalAngle / 360.0 * 100;
        if(percent > 100) percent = 100.0;
        sprintf(buff,"正在校准\n%.2f%%",percent);
        text = buff;
        ui->pushButton_5->setText(text);

    }
     ui->pushButton_5->show();

    //获取气压值
#ifndef Test
    Get_Info("cat /sys/bus/i2c/drivers/bmp280/1-0076/iio:device1/in_pressure_input","气压值: ");
#else
    text = "气压值：100.2";
#endif
    ui->label->setText(text);

    //获取温度值
#ifndef Test
    Get_Info("cat /sys/bus/i2c/drivers/bmp280/1-0076/iio\:device1/in_temp_input","温度值: ");
#else
    strcpy(buff,"24500");
#endif
    number1 = atoi(buff);
    sprintf(buff,"温度值: %-.2f 度",number1/1000.0);
    text = buff;
    ui->label_2->setText(text);

    //获取加速度计值
#ifndef Test
    Get_Info("cat /sys/devices/platform/soc/2100000.aips-bus/21a4000.i2c/i2c-1/1-006a/input/input3/accel/data_xyz","加速度: \n");
#else
    strcpy(buff,"6666,6666,6666");
#endif
    num1 = strtok(buff, ",");
    num2 = strtok(NULL, ",");
    num3 = strtok(NULL, ",");

    number1 = atoi(num1);
    number2 = atoi(num2);
    number3 = atoi(num3);

    accx = number1;
    accy = number2;
    accz = number3;

    text.clear();
    text += "加速度: \nx:";

    sprintf(num1,"%-0.2f",(number1 / 1650.0));
    text += num1;
    text += "\ny:";

    sprintf(num2,"%-0.2f",(number2 / 1650.0));
    text += num2;
    text += "\nz:";

    sprintf(num3,"%-0.2f",(number3 / 1650.0));
    text += num3;
    ui->label_3->setText(text);

    //获取磁力计3轴值
#ifndef Test
    Get_Info("cat /sys/class/compass/msensor/mag_data","磁力计: ");
#else
    strcpy(buff,"5006,5006,5006");
#endif
    num1 = strtok(buff, ",");
    num2 = strtok(NULL, ",");
    num3 = strtok(NULL, ",");

    magx = atoi(num1);
    magy = atoi(num2);
    magz = atoi(num3);

    text.clear();
    text += "磁力计: \nx:";
    text += num1;
    text += "\ny:";
    text += num2;
    text += "\nz:";
    text += num3;
    ui->label_4->setText(text);

    //是否校准
    if(HorizontalCalibrateFlg == true)
    {
        CalibrateProces();
    }
    else {
         Mag_calc_angle();
    }

    //获取陀螺仪数据
    //echo 1 >  /sys/devices/platform/soc/2100000.aips-bus/21a4000.i2c/i2c-1/1-006a/input/input4/gyro/enable

}

//按键检测
void Dialog::window_2()
{
    if(Key_control == 0)
    {
        qDebug() << Exit_Key_Test;
        if(Exit_Key_Test >= 2)
        {
            Key_control = 1;
            ui->pushButton->setFocus(); //确认键获取焦点
            Checked_OK = 0;
            memset(Key_Data,0,sizeof(Key_Data));
        }

    }

}

//蓝牙检测
void Dialog::window_3()
{
    //获取蓝牙信息
    ui->label_2->setGeometry(rect().x()+30, rect().y()+0,200, 41);
    ui->label_2->setText("蓝牙检测：");
    Get_BluetoothInfo();
}

//LCD 检测
void Dialog::window_4()
{
    if(LCD_control == 0)
    {
        //LCD 检测 点击按钮 屏幕显示 红蓝绿 并刷新一张图片
        ui->pushButton_6->setText("开始LCD检测:\n刷新 红->蓝->绿->logo图片");
        sleep(2000);
        ui->pushButton_6->setText("");
        QPalette pal = ui->pushButton_6->palette();

        pal.setColor(QPalette::Button,Qt::red);
        ui->pushButton_6->setPalette(pal);
        ui->pushButton_6->setAutoFillBackground(true);
        ui->pushButton_6->setFlat(true);
        sleep(1000);

        pal.setColor(QPalette::Button,Qt::blue);
        ui->pushButton_6->setPalette(pal);
        ui->pushButton_6->setAutoFillBackground(true);
        ui->pushButton_6->setFlat(true);
        sleep(1000);

        pal.setColor(QPalette::Button,Qt::green);
        ui->pushButton_6->setPalette(pal);
        ui->pushButton_6->setAutoFillBackground(true);
        ui->pushButton_6->setFlat(true);
        sleep(1000);

        ui->pushButton_6->setStyleSheet("background-image:url(:/images/logo2.png)");

        LCD_control = 1;
    }

}

//触摸屏 检测
void Dialog::window_5()
{
    if(Touchscreen_control == 0)
    {
        Touchscreen_control = 1;
        qDebug("ts_test");
#ifdef Test
        //do nothing
#else

        QProcess::execute("/launcher/BHC_APP/ts_test");
        this->raise();
#endif
    }
}

//背光/存储卡 检测
void Dialog::window_6()
{
    if(Disk_control == 0)
    {
    float disk_info[4];
#ifndef Test
    Get_DisInfo(disk_info);
#else
    disk_info[0] = 66.6666;
    disk_info[1] = 66.6666;
    disk_info[2] = 66.6666;
    disk_info[3] = 66.6666;
#endif
    Disk_control = 1;
    qDebug() << disk_info[0] << "  " << disk_info[1];

    //设置label 显示位置和大小
    ui->label->setGeometry(rect().x()+20, rect().y()+30,210, 120);
    text.clear();
    text = "     磁盘空间检测";
    text += "\n总共存储空间：" + QString::number(disk_info[1],'f',2) + " G";
    text += "\n剩余可用空间：" + QString::number(disk_info[0],'f',2) + " G";
    TF_exist = 1;
    if(TF_exist == 1)
    {
        text += "\nTF卡总共存储空间：" + QString::number(disk_info[3],'f',2) + " G";
        text += "\nTF卡剩余可用空间：" + QString::number(disk_info[2],'f',2) + " G";
    }

    ui->label->setText(text);
    }

    if(Slider_control == 0)
    {

      //设置水平方向滑动
      pSlider->setOrientation(Qt::Horizontal);
      //设置滑动条控件的最小值
      pSlider->setMinimum(0);
      //设置滑动条控件的最大值
      pSlider->setMaximum(20);
      //设置步长
      pSlider->setSingleStep(1);
#ifndef Test
      Get_Info("cat /sys/devices/platform/backlight/backlight/backlight/brightness"," ");
#else
      strcpy(buff,"15");
#endif
      //设置滑动条控件的值
      pSlider->setValue(atoi(buff));

      pSlider->setGeometry(60,210,160, 40);
      pSlider->setStyleSheet("QSlider::groove:horizontal {\
                             border: 1px solid #4A708B;\
                             background: #C0C0C0;\
                             height: 10px;\
                             border-radius: 1px;\
                             padding-left:-1px;\
                             padding-right:-1px;\
                             }\
                             QSlider::sub-page:horizontal {\
                             background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                                 stop:0 #B1B1B1, stop:1 #c4c4c4);\
                             background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1,\
                                 stop: 0 #5DCCFF, stop: 1 #1874CD);\
                             border: 1px solid #4A708B;\
                             height: 10px;\
                             border-radius: 2px;\
                             }\
                             QSlider::add-page:horizontal {\
                             background: #575757;\
                             border: 0px solid #777;\
                             height: 10px;\
                             border-radius: 2px;\
                             }\
                             QSlider::handle:horizontal\
                             {\
                                 background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,\
                                 stop:0.6 #45ADED, stop:0.778409 rgba(255, 255, 255, 255));\
                                 width: 11px;\
                                 margin-top: -3px;\
                                 margin-bottom: -3px;\
                                 border-radius: 5px;\
                             }\
                             QSlider::handle:horizontal:hover {\
                                 background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.6 #2A8BDA,\
                                 stop:0.778409 rgba(255, 255, 255, 255));\
                                 width: 11px;\
                                 margin-top: -3px;\
                                 margin-bottom: -3px;\
                                 border-radius: 5px;\
                             }\
                             QSlider::sub-page:horizontal:disabled {\
                             background: #00009C;\
                             border-color: #999;\
                             }\
                             QSlider::add-page:horizontal:disabled {\
                             background: #eee;\
                             border-color: #999;\
                             }\
                             QSlider::handle:horizontal:disabled {\
                             background: #eee;\
                             border: 1px solid #aaa;\
                             border-radius: 4px;\
                             }");

      pSlider->show();
      pSlider->raise();

      Slider_control = 1;
      // 微调框
      pSpinBox->setMinimum(0);  // 最小值
      pSpinBox->setMaximum(20);  // 最大值
      pSpinBox->setSingleStep(1);  // 步长
      pSpinBox->setGeometry(10,200,40,40);

      pSpinBox->setValue(atoi(buff));
      pSpinBox->show();

      // 连接信号槽（相互改变）
      connect(pSpinBox, SIGNAL(valueChanged(int)), pSlider, SLOT(setValue(int)));
      connect(pSlider, SIGNAL(valueChanged(int)), pSpinBox, SLOT(setValue(int)));
      connect(pSlider, SIGNAL(valueChanged(int)), this, SLOT(move_slider()));


      //设置label 显示位置和大小
      ui->label_2->setGeometry(90,190,80, 40);
      ui->label_2->setText("背光控制");
    }
}

//GPS 检测
void Dialog::window_7()
{

    if(GPS_control == 0)
    {
#ifndef Test
        pSatelliteWidget = new SatelliteWidget(this);
        pSatelliteWidget->show();
        pSatelliteWidget->raise();
        pSatelliteWidget->setFocus();
        GPS_control = 1;
#endif
    }

    if(exit_window7 == 1)
    {
        ui->pushButton->setFocus();
        exit_window7 = 0;
    }
}

//摄像头检测
void Dialog::window_8()
{

#ifdef Test
        //do nothing
#else

    if(Camera_control == 0)
    {
        system("sh /test_app/camera_ko/camera_install.sh");
        qDebug("camera_test");
         Camera_control = 1;
    }

    //触发刷新
    ui->label->setText(" ");
#endif
}

//电池/蜂鸣器/闪光灯 检测
void Dialog::window_9()
{
    //设置label 显示位置和大小
    ui->label->setGeometry(rect().x()+30, rect().y()+40,180, 120);
    ui->label_2->setGeometry(rect().x()+50, rect().y()+130,141, 31);
    ui->label_3->setGeometry(rect().x()+10, rect().y()+210,81, 50);
    ui->label_4->setGeometry(rect().x()+10, rect().y()+160,81, 50);


    ui->label_2->setText("点击按钮开始检测");
    ui->label_3->setText("闪光灯检测:");
    ui->label_4->setText("蜂鸣器检测:");

    //显示电池信息 label
    Get_BatteryInfo();
}


void Dialog::ReceiveData(QString data)
{
    Window = data;

    if(Window.toInt() != 1)
    {
        ui->pushButton_5->deleteLater();
    }
    else {
         ui->pushButton_5->hide();
    }

    if(Window.toInt() != 2)
    {
        ui->pushButton_7->deleteLater();
        ui->pushButton_8->deleteLater();
        ui->pushButton_9->deleteLater();
        ui->pushButton_10->deleteLater();
        ui->pushButton_11->deleteLater();
        ui->pushButton_12->deleteLater();
        ui->pushButton_13->deleteLater();
        ui->pushButton_14->deleteLater();
        ui->pushButton_15->deleteLater();
        ui->pushButton_16->deleteLater();
        ui->pushButton_17->deleteLater();
        ui->pushButton_18->deleteLater();
    }
    else if(Window.toInt() == 2)
    {
        memset(Key_Data,0,sizeof(Key_Data));
        ui->label_2->setGeometry(rect().x()+30, rect().y()+235,200, 41);
        ui->label_2->setText("同时按下 + - 退出检测状态");
        this->setFocus();//获取键盘焦点
    }

    if(Window.toInt() != 3)
    {

    }
    if(Window.toInt() != 4)
    {
        ui->pushButton_6->deleteLater();
    }
    if(Window.toInt() != 6)
    {
//        ui->horizontalSlider->deleteLater();
    }
    if(Window.toInt() != 9)
    {
        ui->pushButton_3->deleteLater();
        ui->pushButton_4->deleteLater();
    }
}


void Dialog::on_pushButton_clicked()
{

    if((LCD_control == 0 && Window.toInt() == 4) || (Bluetooth_control == 0 && Window.toInt() == 3)/
            (GPS_control == 0 && Window.toInt() == 7) || (Camera_control    == 0 && Window.toInt() == 8))
    {
          //do nothing
    }
    else {
        switch(Window.toInt())
        {
            case 1:HorizontalCalibrateFlg = false;                          break;
            case 2:Key_control = 0;   Exit_Key_Test = 0;                    break;
            case 3:system("sh /etc/bluz_close.sh ");Bluetooth_control = 0;  break;
            case 4:LCD_control = 0;                                         break;
            case 5:Touchscreen_control = 0;                                 break;
            case 6:Disk_control = 0;TF_exist = 0;Slider_control = 0;        break;
            case 7:GPS_control = 0;                                         break;
            case 8:Camera_control = 0;                                      break;
        }

        emit sendData("成功");  //向父窗口发送成功
        parentWidget()->show(); //显示父窗口
        delete this;    //删除自身
    }

}

void Dialog::on_pushButton_2_clicked()
{
    if((LCD_control == 0 && Window.toInt() == 4) || (Bluetooth_control == 0 && Window.toInt() == 3)/
            (GPS_control == 0 && Window.toInt() == 7) || (Camera_control    == 0 && Window.toInt() == 8))
    {
        //do nothing
   }
    else {
        switch(Window.toInt())
        {
            case 2:Key_control = 0;   Exit_Key_Test = 0;                    break;
            case 3:system("sh /etc/bluz_close.sh ");Bluetooth_control = 0;  break;
            case 4:LCD_control = 0;                                         break;
            case 5:Touchscreen_control = 0;                                 break;
            case 7:GPS_control = 0;                                       break;
            case 8:Camera_control = 0;                                      break;
        }

        emit sendData("失败");  //向父窗口发送失败
        parentWidget()->show(); //显示父窗口
        delete this;    //删除自身
    }
}

void Dialog::on_pushButton_3_clicked()
{
    //喇叭测试  label_2
    switch(Beep_control)
    {
#ifdef Test
        //do no thing
#else
        case 0: system("echo 10 > /sys/devices/platform/beep/leds/beeper-pwm/brightness");
                 printf("echo 10 > /sys/devices/platform/beep/leds/beeper-pwm/brightness");
                Beep_control = 1;break;
        case 1: system("echo 0 > /sys/devices/platform/beep/leds/beeper-pwm/brightness");
                 printf("echo 0 > /sys/devices/platform/beep/leds/beeper-pwm/brightness");
                Beep_control = 0;break;
#endif
      }

}

void Dialog::on_pushButton_4_clicked()
{
    //闪光灯测试 label_3
     switch(Flash_Light_control)
     {
#ifdef Test
        //do no thing
#else
        case 0: system("echo 1 > /sys/class/gpio/gpio7/value");
                printf("echo 1 > /sys/class/gpio/gpio7/value");
                Flash_Light_control = 1;break;
        case 1: system("echo 0 > /sys/class/gpio/gpio7/value ");
                printf("echo 0 > /sys/class/gpio/gpio7/value");
                Flash_Light_control = 0;break;
#endif
       }

}

void Dialog::on_pushButton_5_clicked()
{
    setCalibrateFlag();
    HorizontalCalibrateFlg = true;
}

void Dialog::on_pushButton_6_clicked()
{

}

void Dialog::move_slider()
{
    sprintf(brightness,"echo %d > /sys/devices/platform/backlight/backlight/backlight/brightness",pSlider->value());
#ifndef Test
    system(brightness);
#else
    qDebug() << brightness;
#endif
}


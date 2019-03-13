#include "mainwindow.h"
#include "dialog.h"
#include "ui_mainwindow.h"

static QPushButton *button_number;
static QString button_name;
static QString Set_button_name;
static QPushButton * current_button;
static int Checked_OK = 0;
static int First_time_run = 0;
static QWidget *current_focus;
static QPushButton *button_location;
static QPushButton *button_group[9];
static int location = 0;
static int which_button = 0;
static QString Button_color = "";
static QString Background_color[9];
static QString Color = "";
static int start_MainWindow = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->scroll(0,0);
    if(First_time_run == 0)
    {
        button_group[0] = ui->pushButton;
        button_group[1] = ui->pushButton_2;
        button_group[2] = ui->pushButton_3;
        button_group[3] = ui->pushButton_4;
        button_group[4] = ui->pushButton_5;
        button_group[5] = ui->pushButton_6;
        button_group[6] = ui->pushButton_7;
        button_group[7] = ui->pushButton_8;
        button_group[8] = ui->pushButton_9;

        Button_color = "border-color: gray;border-width: 2px;border-style: solid;";
        for(int i = 0;i < 9;i ++)
        button_group[i]->setStyleSheet(Button_color);

        ui->pushButton_10->setStyleSheet(Button_color);
        this->setFocus();

        button_location = ui->pushButton;   //初始化按键位置
        First_time_run = 1;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::New_Dialog(char *num)
{
    //获取当前是哪个按钮按下
    button_name.clear();
    switch(num[0])
    {
        case '1':button_number = ui->pushButton;
                 button_name = "传感器";
                 break;
        case '2':button_number = ui->pushButton_2;
                 button_name = "按键";
                 break;
        case '3':button_number = ui->pushButton_3;
                 button_name = "蓝牙";
                 break;
        case '4':button_number = ui->pushButton_4;
                 button_name = "LCD";
                 break;
        case '5':button_number = ui->pushButton_5;
                 button_name = "触摸屏";
                 break;
        case '6':button_number = ui->pushButton_6;
                 button_name = "存储卡/陀螺仪";
                 break;
        case '7':button_number = ui->pushButton_7;
                 button_name = "GPS";
                 break;
        case '8':button_number = ui->pushButton_8;
                 button_name = "相机";
                 break;
        case '9':button_number = ui->pushButton_9;
                 button_name = "电池/蜂鸣器/闪光灯";
                 break;
    }
    button_number->setText(button_name);


    //主界面隐藏
    this->hide();

    //新建一个窗口对象，其中this指定了新的窗口的父对象是MainWindow，在销毁MainWindow时也会销毁子对象
    dialog = new Dialog(this);
    connect(dialog,SIGNAL(sendData(QString)),this,SLOT(ReceiveData(QString)));

    //model属性决定了show()应该将弹出的dialog设置为模态还是飞模态
    dialog->setModal(false);//默认属性为false并且show()弹出的窗口是非模态的

    emit sendData(num);  //向父窗口发送成功
    dialog->show();
}

void MainWindow::on_pushButton_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);
    button_location = ui->pushButton;
    which_button = 0;
    New_Dialog("1");
}

void MainWindow::on_pushButton_2_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);
    button_location = ui->pushButton_2;
    which_button = 1;

    New_Dialog("2");
}

void MainWindow::on_pushButton_3_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);
    button_location = ui->pushButton_3;
    which_button = 2;
    New_Dialog("3");
}

void MainWindow::on_pushButton_4_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);
    button_location = ui->pushButton_4;
    which_button = 3;
    New_Dialog("4");
}

void MainWindow::on_pushButton_5_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);
    button_location = ui->pushButton_5;
    which_button = 4;
    New_Dialog("5");
}

void MainWindow::on_pushButton_6_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);
    button_location = ui->pushButton_6;
    which_button = 5;
    New_Dialog("6");
}

void MainWindow::on_pushButton_7_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);
    button_location = ui->pushButton_7;
    which_button = 6;
    New_Dialog("7");
}

void MainWindow::on_pushButton_8_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);
    button_location = ui->pushButton_8;
    which_button = 7;
    New_Dialog("8");
}

void MainWindow::on_pushButton_9_clicked()
{
    Color = Button_color + Background_color[which_button];
    button_location->setStyleSheet(Color);

    button_location = ui->pushButton_9;
    which_button = 8;
    New_Dialog("9");
}

void MainWindow::on_pushButton_10_clicked()
{
    system("reboot");
    this->deleteLater();
}

void MainWindow::ReceiveData(QString data)
{
    Set_button_name.clear();
    Set_button_name = button_name;

    this->setFocus();

    button_location = button_number;
    button_location->setStyleSheet("border-color: blue;border-width: 2px;border-style: solid;");

    QPalette pal = button_number->palette();
    if(data == "成功")
    {
       pal.setColor(QPalette::Button,Qt::green);
       Set_button_name += " OK";
       Background_color[which_button] = "background-color: rgb(0,255,0);";

    }
    else if(data == "失败")
    {
       pal.setColor(QPalette::Button,Qt::red);
       Set_button_name += " NO";
       Background_color[which_button] = "background-color: red;";

    }

    QFont ft;
    if(button_number == ui->pushButton_9)
    {
        ft.setPixelSize(10);
        ui->pushButton_9->setFont(ft);
    }
    else if(button_number == ui->pushButton_6)
    {
        ft.setPixelSize(12);
        ui->pushButton_6->setFont(ft);
    }

    button_number->setText(Set_button_name);
    button_number->setPalette(pal);
    button_number->setAutoFillBackground(true);
    button_number->setFlat(true);
}

void MainWindow::keyPressEvent(QKeyEvent *keyev)
{
    int keyValue = keyev->key();
    qDebug() << keyValue;

    if( keyValue == Key_Up || keyValue == Key_Left || keyValue == Key_Down || keyValue == Key_Right )
    {
        for(location = 0;location < 9;location ++)
        {
            if(button_location == button_group[location])
            {
                break;
            }
        }

        Color = Button_color + Background_color[location];
        button_location->setStyleSheet(Color);
        switch(keyValue)
        {
            case Key_Up:    if(location - 2 >= 0)
                            {
                                button_location = button_group[location - 2];
                                location -= 2;
                            }break;
            case Key_Down:  if(location + 2 <= 8)
                            {
                                button_location = button_group[location + 2];
                                location += 2;
                            }break;
            case Key_Left:  if(location % 2 != 0)
                            {
                                button_location = button_group[location - 1];
                                location -= 1;
                            }break;
            case Key_Right: if(location % 2 == 0)
                            {
                                button_location = button_group[location + 1];
                                location += 1;
                            }break;
        }
        Color = "border-color: blue;border-width: 2px;border-style: solid;" + Background_color[location];
        button_location->setStyleSheet(Color);
    }


    if(keyev->type()==QKeyEvent::KeyPress)
    {
        //模拟当前焦点所在按钮点击事件
        if(Checked_OK == 0)
        {
            qDebug() << "back";

            if(keyValue == Key_Ok)
            {
                button_location->clicked();
            }
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{

}











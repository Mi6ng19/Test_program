#include "mainwindow.h"
#include <QApplication>
#include <QElapsedTimer>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/logo2.png"));//设置图片
    splash->show();//显示图片
    splash->showMessage("\n测试程序启动中\n 请稍后...", Qt::AlignCenter, Qt::red);

    int delayTime = 3;
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < (delayTime * 1000))
    {
         splash->show();
         a.processEvents();
    }


    MainWindow w;
    w.show();

    splash->finish(&w);
    delete splash;//释放空间，节省内存
    return a.exec();
}

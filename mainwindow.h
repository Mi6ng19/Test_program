#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QScrollArea>
#include "dialog.h"
#include <QThread>
#include <QPainter>
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
#include <QTreeView>
#include <QPainter>
#include <QSplashScreen>
#include <QMouseEvent>

extern SatelliteWidget *pSatelliteWidget;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void ReceiveData(QString data);

    void New_Dialog(char *num);

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();

    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *keyev);
private:
    QLabel *plabel;
    void sleep(unsigned int msec);

private:
    Ui::MainWindow *ui;
    Dialog *dialog;     //添加私有成员，为一个dialog(窗口)的指针

signals:
    void sendData(QString);
};

#endif // MAINWINDOW_H

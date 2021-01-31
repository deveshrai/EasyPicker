#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHeaderView>
#include <QSizePolicy>
#include <QFileDialog>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QMessageBox>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QRgb>
#include <QCamera>
#include <QCameraInfo>
#include <QMultimedia>
#include <QCameraViewfinder>
#include <QLabel>
#include <QDebug>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionImport_CSV_triggered();

    void on_tableWidget_cellClicked(int row, int column);

    void on_tableWidget_cellActivated(int row, int column);

    void on_cameraComboBox_activated(const QString &arg1);

    void on_MainWindow_destroyed();

private:
    bool populateTableFromCSV(QString csvFileName);
    void populateCameraList(void);
    bool findAndStartCamera(void);
    Ui::MainWindow *ui;
    QString CSV_file_Data;
    QCamera *camera=nullptr;
    bool cameraFound=false;
    QCameraViewfinder *camviewfinder;
    QLabel *overlayCross=nullptr;

};
#endif // MAINWINDOW_H

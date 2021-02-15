#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define CAMERA_OFFSETX_VALUE 14.7
#define CAMERA_OFFSETY_VALUE 1.5

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
#include "settingsdialog.h"

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

    void on_actionSet_Pick_Up_Location_triggered();

    void on_actionSettings_triggered();

    void on_adjustTopPushButton_clicked();

    void on_adjustBottomPushButton_clicked();

    void on_adjustLeftPushButton_clicked();

    void on_adjustRightPushButton_clicked();

    void on_incrementComboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_actionVacuum_toggled(bool arg1);

    void on_actionHome_Machine_triggered();

    void on_actionSave_triggered();

    void on_actionNew_Project_triggered();

    void on_actionOpen_triggered();

    void on_actionSet_Place_Location_triggered();

    void on_actionCamera_Coordinates_triggered();

    void on_actionCamera_Coordinates_toggled(bool arg1);

    void on_actionCamera_Coordinates_triggered(bool checked);

private:
    bool populateTableFromCSV(QString csvFileName);
    bool populateTableFromPUP(QString pupFileName);
    void changeTableToMM();
    void connectToSerial();
    void populateCameraList(void);
    bool findAndStartCamera(void);
    void updateCoordinates(void);
    void moveTo(double x, double y, double z);
    void jogTo(double x, double y, double z);

    Ui::MainWindow *ui;
    QString CSV_file_Data;
    QCamera *camera=nullptr;
    bool cameraFound=false;
    QCameraViewfinder *camviewfinder;
    QLabel *overlayCross=nullptr;
    SettingsDialog *settingsDialog;
    QString machineSerial;
    QString vacuumSerial;
    QString incrementAmount;
    QSerialPort *mcPort,*vcPort;
    double locX=0,locY=0,locZ=0;
    int selectedRow=0,selectedColumn=0;
    bool cameraMode=false;
    double cameraOffsetX=CAMERA_OFFSETX_VALUE;
    double cameraOffsetY=CAMERA_OFFSETY_VALUE;


};


#endif // MAINWINDOW_H

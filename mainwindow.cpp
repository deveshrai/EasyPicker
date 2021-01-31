#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusbar->showMessage("Disconnected");

    this->populateCameraList();
    this->findAndStartCamera();


}

MainWindow::~MainWindow()
{
    delete ui;
    exit(0);
}
void MainWindow::populateCameraList(void)
{

    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cameras) {
        ui->cameraComboBox->addItem(cameraInfo.deviceName());
//        qDebug()<<cameraInfo.deviceName();
//        if(cameraInfo.deviceName()==settings.value("settings/cameraName"))     //"EAB7A68FEC2B4487AADFD8A91C1CB782")
//        {
//            ui->cameraComboBox->insertItem()
//        }
    }
}

void MainWindow::on_cameraComboBox_activated(const QString &arg1)
{
    QSettings settings("Devesh Rai", "Easy Picker");
    settings.setValue("settings/cameraName",arg1);

    //qDebug()<<arg1;
    findAndStartCamera();
}

bool MainWindow::findAndStartCamera(void)
{
    QSettings settings("Devesh Rai", "Easy Picker");
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cameras) {
        qDebug()<<cameraInfo.deviceName();
        if(cameraInfo.deviceName()==settings.value("settings/cameraName"))     //"EAB7A68FEC2B4487AADFD8A91C1CB782")
        {
            if(camera!=nullptr)
                delete camera;
            camera = new QCamera(cameraInfo);
            cameraFound=true;
        }
    }
    if(cameraFound)
    {
        if(this->overlayCross!=nullptr)
            delete overlayCross;

        this->overlayCross=new QLabel();
        QPixmap pix(100,100);//(":/new/Icons/transparent100-100.png");
        //pix->swap(QPixmap());
        //pix->fill(QColor(255,255,255,255));
        QPainter paint(&pix);
        paint.fillRect(0,0,100,100,QBrush(QColor(255,255,255,0)));
        paint.setPen(*(new QColor(Qt::green)));
        paint.drawLine(0,50,100,50);
        paint.drawLine(50,0,50,100);
        overlayCross->setPixmap(pix);

        ui->gridLayout->removeWidget(ui->cameraView);
        ui->gridLayout->addWidget(ui->cameraView,0,0,Qt::AlignHCenter|Qt::AlignVCenter);
        ui->gridLayout->addWidget(this->overlayCross,0,0,Qt::AlignCenter);

        camviewfinder = new QCameraViewfinder(ui->cameraView);
        camera->setViewfinder(camviewfinder);
        camviewfinder->show();
        camviewfinder->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
        camviewfinder->setFixedSize(200,200);
        camera->start();

        //delete camera;
        //delete paint;
        //delete pix;
    }
    return cameraFound;
}

bool MainWindow::populateTableFromCSV(QString csvFileName)
{
    QList<QString> csvLines;
    QList<QStringList> csvContent;
    QStringList headerNames={"Part Value","X","Y","Rotation","Part Name","Pick up X", "Pick up Y", "Pick up Z", "Place X", "Place Y", "Place Z", "Enabled"};
    QFile file(csvFileName);
    QTableWidgetItem *Capacitor=new QTableWidgetItem("Capacitor");
    Capacitor->setIcon(QIcon(QPixmap(":/new/Icons/icons8-capacitor-80.png")));
    QTableWidgetItem *Resistor=new QTableWidgetItem("Resistor");
    Resistor->setIcon(QIcon(QPixmap(":/new/Icons/icons8-resistor-80.png")));
    QTableWidgetItem *Connector=new QTableWidgetItem("Connector");
    Connector->setIcon(QIcon(QPixmap(":/new/Icons/icons8-usb-micro-a-80.png")));
    QTableWidgetItem *Inductor=new QTableWidgetItem("Inductor");
    Inductor->setIcon(QIcon(QPixmap(":/new/Icons/icons8-oscillator-80.png")));
    QTableWidgetItem *Diode=new QTableWidgetItem("Diode");
    Diode->setIcon(QIcon(QPixmap(":/new/Icons/icons8-led-diode-80.png")));
    QTableWidgetItem *IC=new QTableWidgetItem("IC");
    IC->setIcon(QIcon(QPixmap(":/new/Icons/icons8-processor-80.png")));
    QTableWidgetItem *UnknownPart=new QTableWidgetItem("Unknown Part");
    UnknownPart->setIcon(QIcon(QPixmap(":/new/Icons/icons8-question-mark-96.png")));


    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                    return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        csvLines.append(in.readLine());
    }


    foreach(QString csvLine, csvLines)
    {
        csvContent.append(csvLine.split(";"));
    }

    int csvRowCount=csvLines.size();
    int csvColCount=headerNames.size();//csvContent[0].size();

    ui->tableWidget->setRowCount(csvRowCount);
    ui->tableWidget->setColumnCount(csvColCount);
    ui->tableWidget->setHorizontalHeaderLabels(headerNames);

    for(int xCnt=0;xCnt<5;xCnt++)
    {
        for(int yCnt=0;yCnt<csvRowCount;yCnt++)
        {
            ui->tableWidget->setItem(yCnt,xCnt,new QTableWidgetItem(csvContent[yCnt][xCnt]));
            if(xCnt==4)
            {
                QString val=csvContent[yCnt][xCnt];
                if(val.contains("U"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,IC);
                else if(val.contains("IC"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,IC);
                else if(val.contains("D"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,Diode);
                else if(val.contains("LED"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,Diode);
                else if(val.contains("J"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,Connector);
                else if(val.contains("R"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,Resistor);
                else if(val.contains("C"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,Capacitor);
                else if(val.contains("Y"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,Inductor);
                else if(val.contains("L"))
                    ui->tableWidget->setVerticalHeaderItem(yCnt,Inductor);
                else
                    ui->tableWidget->setVerticalHeaderItem(yCnt,UnknownPart);

            }
        }
    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    return true;
}

void MainWindow::on_actionImport_CSV_triggered()
{
    QString csvFileName=QFileDialog::getOpenFileName(this, tr("Open file"), "~/Desktop", tr("CSV/mnt files (*.csv *.mnt)"));


    if(csvFileName!="")
    {
        if(populateTableFromCSV(csvFileName))
            ui->statusbar->showMessage(csvFileName+" Opened and imported.",5000);
        else
        {
            QMessageBox errorMessage;
            errorMessage.setText("An error occured reading the file!");
            errorMessage.exec();
        }
    }
    else
    {
        QMessageBox errorMessage;
        errorMessage.setText("No file selected.");
        errorMessage.exec();
    }

}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
//    QMessageBox mb;
//    mb.setText(ui->tableWidget->item(row,column)->text()+" "+QString::number(row)+","+QString::number(column));
//    mb.exec();
    ui->partNameLabel->setText(ui->tableWidget->item(row,4)->text());
    ui->pickUpLocationLabel->setText("("+ui->tableWidget->item(row,1)->text()+","+ui->tableWidget->item(row,2)->text()+",0)");

}

void MainWindow::on_tableWidget_cellActivated(int row, int column)
{
    //ui->partNameLabel->setText();
}



void MainWindow::on_MainWindow_destroyed()
{

}

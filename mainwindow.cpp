#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->showMaximized();
    this->showFullScreen();
    QSettings settings("Devesh Rai", "Easy Picker");
    this->machineSerial=settings.value("settings/machineSerialName").toString();
    this->vacuumSerial=settings.value("settings/vacuumSerialName").toString();
    this->incrementAmount=settings.value("settings/incrementAmount").toString();
    this->cameraOffsetX=settings.value("settings/xCameraOffset").toDouble();
    this->cameraOffsetY=settings.value("settings/yCameraOffset").toDouble();

    for(int i=0; i<ui->incrementComboBox->count();i++)
    {
        //qDebug()<<ui->incrementComboBox->itemText((i));
        if(ui->incrementComboBox->itemText(i)==this->incrementAmount)
        {
            //qDebug()<<"Found "<<this->incrementAmount<<" at "<<i;
            ui->incrementComboBox->setCurrentIndex(i);
        }
    }
    if(this->machineSerial!=""&&this->vacuumSerial!="")
    {
        this->connectToSerial();
        ui->statusbar->showMessage("Connected to Machine: "+this->machineSerial+". Connected to Vacuum: "+this->vacuumSerial);
    }
    else
        ui->statusbar->showMessage("Disconnected");
    this->settingsDialog = new SettingsDialog();
    connect(settingsDialog,SIGNAL(accepted()),this,SLOT(updateSettings()));
    this->camScene = new QGraphicsScene(this);
    this->camVideoItem = new QGraphicsVideoItem();
    this->populateCameraList();
    this->findAndStartCamera();



}
void MainWindow::updateSettings()
{
    QSettings settings("Devesh Rai", "Easy Picker");
    this->machineSerial=settings.value("settings/machineSerialName").toString();
    this->vacuumSerial=settings.value("settings/vacuumSerialName").toString();
    this->incrementAmount=settings.value("settings/incrementAmount").toString();
    this->cameraOffsetX=settings.value("settings/xCameraOffset").toDouble();
    this->cameraOffsetY=settings.value("settings/yCameraOffset").toDouble();
//    mcPort->close();
//    vcPort->close();

//    this->connectToSerial();
//    if(this->machineSerial!=""&&this->vacuumSerial!="")
//    {
//        this->connectToSerial();
//        ui->statusbar->showMessage("Connected to Machine: "+this->machineSerial+". Connected to Vacuum: "+this->vacuumSerial);
//    }
}

void MainWindow::connectToSerial()
{
    mcPort = new QSerialPort(this->machineSerial);
    mcPort->setBaudRate(115200);
    mcPort->open(QIODevice::ReadWrite);

    vcPort = new QSerialPort(this->vacuumSerial);
    vcPort->setBaudRate(115200);
    vcPort->open(QIODevice::ReadWrite);

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
            if(camera !=nullptr)
            {
                camera->stop();
                delete camera;
            }
            camera = new QCamera(cameraInfo);
            cameraFound=true;
        }
    }
    if(cameraFound)
    {

        ui->gridLayout->removeWidget(ui->cameraView);
        ui->gridLayout->addWidget(ui->cameraView,0,0,Qt::AlignHCenter|Qt::AlignVCenter);

        camScene->addItem(camVideoItem);
        camVideoItem->setSize(QSizeF(200,200));
        camera->setViewfinder(camVideoItem);
        ui->cameraView->setScene(camScene);
        camScene->addLine(100,50,100,150,QPen(Qt::green));
        camScene->addLine(50,100,150,100,QPen(Qt::green));
        camera->start();

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
    qDebug()<<csvRowCount;
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
    for(int xCnt=5;xCnt<12;xCnt++)
    {
        for(int yCnt=0;yCnt<csvRowCount;yCnt++)
        {
            ui->tableWidget->setItem(yCnt,xCnt,new QTableWidgetItem("-0"));
        }
    }
    //ui->tableWidget->item(row,5)->
    this->changeTableToMM();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    return true;
}
bool MainWindow::populateTableFromPUP(QString pupFileName)
{
    QList<QString> pupLines;
    QList<QStringList> pupContent;
    QStringList headerNames={"Part Value","X","Y","Rotation","Part Name","Pick up X", "Pick up Y", "Pick up Z", "Place X", "Place Y", "Place Z", "Enabled"};
    QFile file(pupFileName);
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
        pupLines.append(in.readLine());
    }


    foreach(QString pupLine, pupLines)
    {
        pupContent.append(pupLine.split(";"));
    }

    int pupRowCount=pupLines.size();
    int pupColCount=headerNames.size();//csvContent[0].size();

    ui->tableWidget->setRowCount(pupRowCount);
    ui->tableWidget->setColumnCount(pupColCount);
    qDebug()<<pupRowCount;
    ui->tableWidget->setHorizontalHeaderLabels(headerNames);

    for(int xCnt=0;xCnt<12;xCnt++)
    {
        for(int yCnt=0;yCnt<pupRowCount;yCnt++)
        {
            ui->tableWidget->setItem(yCnt,xCnt,new QTableWidgetItem(pupContent[yCnt][xCnt]));
            if(xCnt==4)
            {
                QString val=pupContent[yCnt][xCnt];
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

void MainWindow::changeTableToMM()
{
    //qDebug()<<ui->tableWidget->rowCount();
    for(int xCnt=1;xCnt<3;xCnt++)
    {
        for(int yCnt=0;yCnt<ui->tableWidget->rowCount();yCnt++)
        {
            QString milStr,mmStr;
            double milVal,mmVal;
            milStr=ui->tableWidget->item(yCnt,xCnt)->text();
            milVal=milStr.toDouble();
            mmVal=milVal*0.0254;
            mmStr=QString::number(mmVal,'g',3);
            ui->tableWidget->setItem(yCnt,xCnt,new QTableWidgetItem(mmStr));
            //qDebug()<<ui->tableWidget->item(yCnt,4)->text()<<milStr<<","<<milVal<<","<<mmVal<<","<<mmStr;
        }
    }
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
void MainWindow::on_actionOpen_triggered()
{
    QString fileName=QFileDialog::getOpenFileName(this, tr("Open file"), "", tr("Pick Und Place files (*.pup)"));


    if(fileName!="")
    {
        if(populateTableFromPUP(fileName))
            ui->statusbar->showMessage(fileName+" Opened",5000);
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
void MainWindow::on_actionSet_Pick_Up_Location_triggered()
{


    ui->tableWidget->item(this->selectedRow,5)->setText(QString::number(this->locX));
    ui->tableWidget->item(this->selectedRow,6)->setText(QString::number(this->locY));
    ui->tableWidget->item(this->selectedRow,7)->setText(QString::number(this->locZ));
}

void MainWindow::on_actionSet_Place_Location_triggered()
{
    ui->tableWidget->item(this->selectedRow,8)->setText(QString::number(this->locX));
    ui->tableWidget->item(this->selectedRow,9)->setText(QString::number(this->locY));
    ui->tableWidget->item(this->selectedRow,10)->setText(QString::number(this->locZ));
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    ui->partNameLabel->setText(ui->tableWidget->item(row,4)->text());
    ui->pickUpLocationLabel->setText("("+ui->tableWidget->item(row,1)->text()+","+ui->tableWidget->item(row,2)->text()+",0)");
    this->selectedRow=row;
    this->selectedColumn=column;
    if(column<5)
    {

        this->moveTo(locX,locY,0);
        qDebug()<<ui->tableWidget->item(row,1)->text().toDouble();
        this->moveTo(ui->tableWidget->item(row,1)->text().toDouble() -(this->cameraMode==true?this->cameraOffsetX:0),
                     ui->tableWidget->item(row,2)->text().toDouble() -(this->cameraMode==true?this->cameraOffsetY:0),
                     0);
        this->moveTo(locX,locY,20);

    }
    else if(column>=5&&column<=7&&ui->tableWidget->item(row,5)->text()!="-0")
    {
        this->moveTo(locX,locY,0);
        this->moveTo(ui->tableWidget->item(row,5)->text().toDouble() -(this->cameraMode==true?cameraOffsetX:0),
                     ui->tableWidget->item(row,6)->text().toDouble() -(this->cameraMode==true?cameraOffsetY:0),
                     0);
        this->moveTo(locX,locY,ui->tableWidget->item(row,7)->text().toDouble());

    }
    else if(column>=8&&column<=10&&ui->tableWidget->item(row,8)->text()!="-0")
    {
        this->moveTo(locX,locY,0);
        this->moveTo(ui->tableWidget->item(row,8)->text().toDouble() -(this->cameraMode==true?cameraOffsetX:0),
                     ui->tableWidget->item(row,9)->text().toDouble() -(this->cameraMode==true?cameraOffsetY:0),
                     0);
        this->moveTo(locX,locY,ui->tableWidget->item(row,10)->text().toDouble());

    }
}

void MainWindow::on_tableWidget_cellActivated(int row, int column)
{
    //ui->partNameLabel->setText();
}



void MainWindow::on_MainWindow_destroyed()
{

}



void MainWindow::on_actionSettings_triggered()
{

    this->settingsDialog->exec();
}
void MainWindow::moveTo(double x, double y, double z)
{
    qDebug()<<x<<" "<<y<<" "<<z;
    QString jogCommand="G0 X"+QString::number(x)+"Y"+QString::number(y)+"Z"+QString::number(z)+ "\r\n";
    QByteArray qba=jogCommand.toUtf8();
    mcPort->write(qba);
    mcPort->waitForBytesWritten(1000);
    locX=x;
    locY=y;
    locZ=z;

    this->updateCoordinates();
}
void MainWindow::jogTo(double x, double y, double z)
{
    QString jogCommand="$J=G91 G21 X"+QString::number(x)+"Y"+QString::number(y)+"Z"+QString::number(z)+ " F1000\r\n";
    QByteArray qba=jogCommand.toUtf8();

    mcPort->write(qba);
    mcPort->waitForBytesWritten(1000);
    locX+=x;
    locY+=y;
    locZ+=z;

    this->updateCoordinates();
}

void MainWindow::on_adjustTopPushButton_clicked()
{
//    QString jogCommand="$J=G91 G21 Y"+this->incrementAmount+ " F1000\r\n";
//    QByteArray qba=jogCommand.toUtf8();
//    //qba.append('e');
//    mcPort->write(qba);
//    locY+=this->incrementAmount.toDouble();
//    this->updateCoordinates();
    this->jogTo(0,this->incrementAmount.toDouble(),0);
}

void MainWindow::on_adjustBottomPushButton_clicked()
{
//    QString jogCommand="$J=G91 G21 Y-"+this->incrementAmount+ " F1000\r\n";
//    QByteArray qba=jogCommand.toUtf8();
//    //qba.append('e');
//    mcPort->write(qba);
//    locY-=this->incrementAmount.toDouble();
//    this->updateCoordinates();
        this->jogTo(0,0-(this->incrementAmount.toDouble()),0);
}


void MainWindow::on_adjustLeftPushButton_clicked()
{
//    QString jogCommand="$J=G91 G21 X-"+this->incrementAmount+ " F1000\r\n";
//    QByteArray qba=jogCommand.toUtf8();
//    //qba.append('e');
//    mcPort->write(qba);
//    locX-=this->incrementAmount.toDouble();
//    this->updateCoordinates();
        this->jogTo((0-this->incrementAmount.toDouble()),0,0);
}

void MainWindow::on_adjustRightPushButton_clicked()
{
//    QString jogCommand="$J=G91 G21 X" +this->incrementAmount+ " F1000\r\n";
//    //qDebug()<<jogCommand;
//    QByteArray qba=jogCommand.toUtf8();
//    //qba.append('e');
//    mcPort->write(qba);
//    locX+=this->incrementAmount.toDouble();
//    this->updateCoordinates();
    this->jogTo((this->incrementAmount.toDouble()),0,0);
}

void MainWindow::on_incrementComboBox_currentIndexChanged(const QString &arg1)
{
    QSettings settings("Devesh Rai", "Easy Picker");
    settings.setValue("settings/incrementAmount",arg1);
    this->incrementAmount=arg1;
    qDebug()<<this->incrementAmount;
}

void MainWindow::on_pushButton_3_clicked()
{
//    QString jogCommand="$J=G91 G21 Z-" +this->incrementAmount+ " F1000\r\n";
//    //qDebug()<<jogCommand;
//    QByteArray qba=jogCommand.toUtf8();
//    //qba.append('e');
//    mcPort->write(qba);
//    locZ-=this->incrementAmount.toDouble();
//    this->updateCoordinates();
    this->jogTo(0,0,(0-this->incrementAmount.toDouble()));
}

void MainWindow::on_pushButton_4_clicked()
{
//    QString jogCommand="$J=G91 G21 Z" +this->incrementAmount+ " F1000\r\n";
//    //qDebug()<<jogCommand;
//    QByteArray qba=jogCommand.toUtf8();
//    //qba.append('e');
//    mcPort->write(qba);
//    locZ+=this->incrementAmount.toDouble();
//    this->updateCoordinates();
    this->jogTo(0,0,(this->incrementAmount.toDouble()));
}
void MainWindow::updateCoordinates(void)
{
    QString newHeadLocation="("+QString::number(locX)+", "+QString::number(locY)+", "+QString::number(locZ)+")";
    ui->headLocationLabel->setText(newHeadLocation);
}

void MainWindow::on_actionVacuum_toggled(bool arg1)
{
    if(arg1)
    {
        QByteArray qba;
        qba.append('e');
        vcPort->write(qba);
    }
    else
    {
        QByteArray qba;
        qba.append('d');
        vcPort->write(qba);
    }
}

void MainWindow::on_actionHome_Machine_triggered()
{
    QString homeCommand="G0 Z0\r\nG0 X0 Y0 Z0\r\n";
    //qDebug()<<jogCommand;
    QByteArray qba=homeCommand.toUtf8();
    mcPort->write(qba);
    locX=0;locY=0;locZ=0;
    this->updateCoordinates();
}

void MainWindow::on_actionSave_triggered()
{
    //Save .pup file
    QString fileName=QFileDialog::getSaveFileName(this,"Save Pick And Place File","","Pick Und Place File (*.pup)");
    qDebug()<<fileName;

    if(fileName!="")
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
            return;
        }


        QString fileData="";
        qDebug()<<ui->tableWidget->rowCount()<<" "<<ui->tableWidget->columnCount();
        for(int row=0; row<ui->tableWidget->rowCount();row++)
        {
            for(int column=0; column<ui->tableWidget->columnCount();column++)
            {
                fileData=fileData+ui->tableWidget->item(row,column)->text()+((column+1)==ui->tableWidget->columnCount()?"":";");

            }
            fileData+="\n";
        }
        file.write(fileData.toUtf8());
        file.close();
        ui->statusbar->showMessage(fileName+" written!");

    }

}



void MainWindow::on_actionNew_Project_triggered()
{
//    //for(int i=0;i<ui->tableWidget->rowCount();i++)
//       //ui->tableWidget->removeRow(0);
//    ui->tableWidget->model()->removeRows(0,ui->tableWidget->rowCount());

    //overlayCross->setStyleSheet("background-color: rgba(0,0,0,0%)");//setStyleSheet("background-color: none;");
    if(this->overlayCross!=nullptr)
        delete overlayCross;
    this->overlayCross=new QLabel();

    QPixmap pix(50,50);
    //QPixmap pix(":/new/Icons/transparent100-100.png")
    QPainter paint(&pix);

    paint.setPen(*(new QColor(Qt::green)));
    paint.drawLine(0,25,50,25);
    paint.drawLine(25,0,25,50);
    paint.end();

    overlayCross->setPixmap(pix);
    ui->gridLayout->addWidget(this->overlayCross,0,0,Qt::AlignCenter);
}




void MainWindow::on_actionCamera_Coordinates_triggered()
{
    //Camera is 10.7mm offset on X (10.687)
}

void MainWindow::on_actionCamera_Coordinates_toggled(bool arg1)
{


}

void MainWindow::on_actionCamera_Coordinates_triggered(bool checked)
{

    //Camera is 10.7mm offset on X (10.687)
    if(checked==true)
    {
        cameraMode=true;
//        QString jogCommand="$J=G91 G21 X-14.7 Y-1.5 F1000\r\n";
//        //qDebug()<<jogCommand;
//        QByteArray qba=jogCommand.toUtf8();
//        //qba.append('e');
//        mcPort->write(qba);
//        locX-=14.7;
//        this->updateCoordinates();
        jogTo(0-this->cameraOffsetX,0-this->cameraOffsetY,0);
        ui->actionHome_Machine->setDisabled(true);
        ui->actionSet_Pick_Up_Location->setDisabled(true);
        ui->actionSet_Place_Location->setDisabled(true);
        ui->actionStart_Automated->setDisabled(true);
        ui->actionStep_Through_Next->setDisabled(true);
        ui->actionVacuum->setDisabled(true);

    }
    else
    {
        cameraMode=false;
//        QString jogCommand="$J=G91 G21 X14.7 Y1.5 F1000\r\n";
//        //qDebug()<<jogCommand;
//        QByteArray qba=jogCommand.toUtf8();
//        //qba.append('e');
//        mcPort->write(qba);
//        locX+=14.7;
//        this->updateCoordinates();
        jogTo(this->cameraOffsetX,this->cameraOffsetY,0);
        ui->actionHome_Machine->setDisabled(false);
        ui->actionSet_Pick_Up_Location->setDisabled(false);
        ui->actionSet_Place_Location->setDisabled(false);
        ui->actionStart_Automated->setDisabled(false);
        ui->actionStep_Through_Next->setDisabled(false);
        ui->actionVacuum->setDisabled(false);
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit(0);
}

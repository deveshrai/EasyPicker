#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSettings settings("Devesh Rai", "Easy Picker");
    this->machineSerial=settings.value("settings/machineSerialName").toString();
    this->vacuumSerial=settings.value("settings/vacuumSerialName").toString();
    this->incrementAmount=settings.value("settings/incrementAmount").toString();
    for(int i=0; i<ui->incrementComboBox->count();i++)
    {
        qDebug()<<ui->incrementComboBox->itemText((i));
        if(ui->incrementComboBox->itemText(i)==this->incrementAmount)
        {
            qDebug()<<"Found "<<this->incrementAmount<<" at "<<i;
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

    this->populateCameraList();
    this->findAndStartCamera();



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
        camviewfinder->setAspectRatioMode(Qt::KeepAspectRatio);
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
    for(int xCnt=5;xCnt<11;xCnt++)
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

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    ui->partNameLabel->setText(ui->tableWidget->item(row,4)->text());
    ui->pickUpLocationLabel->setText("("+ui->tableWidget->item(row,1)->text()+","+ui->tableWidget->item(row,2)->text()+",0)");
    this->selectedRow=row;
    this->selectedColumn=column;

    if(ui->tableWidget->item(row,5)->text()=="-0")
    {


        QString mcCommand="G0 Z0 F3000\r\nG0 X"+ui->tableWidget->item(row,1)->text()+"Y"+ui->tableWidget->item(row,2)->text()+ "\r\nG0 Z17 F3000\r\n";
        QByteArray qba=mcCommand.toUtf8();
        mcPort->write(qba);
        locX=ui->tableWidget->item(row,1)->text().toDouble();
        locY=ui->tableWidget->item(row,2)->text().toDouble();
        locZ=17;
        this->updateCoordinates();
    }
    else
    {
        QString mcCommand="G0 Z0 F3000\r\nG0 X"+ui->tableWidget->item(row,5)->text()+"Y"
                +ui->tableWidget->item(row,5)->text()
                + "\r\nG0 Z"+ui->tableWidget->item(row,7)->text()
                +" F3000\r\n";
        QByteArray qba=mcCommand.toUtf8();
        mcPort->write(qba);
        locX=ui->tableWidget->item(row,5)->text().toDouble();
        locY=ui->tableWidget->item(row,6)->text().toDouble();
        locZ=ui->tableWidget->item(row,7)->text().toDouble();
        this->updateCoordinates();
    }
}

void MainWindow::on_tableWidget_cellActivated(int row, int column)
{
    //ui->partNameLabel->setText();
}



void MainWindow::on_MainWindow_destroyed()
{

}

void MainWindow::on_actionSet_Pick_Up_Location_triggered()
{

}

void MainWindow::on_actionSettings_triggered()
{
    this->settingsDialog->exec();
}

void MainWindow::on_adjustTopPushButton_clicked()
{
    QString jogCommand="$J=G91 G21 Y"+this->incrementAmount+ " F1000\r\n";
    QByteArray qba=jogCommand.toUtf8();
    //qba.append('e');
    mcPort->write(qba);
    locY+=this->incrementAmount.toDouble();
    this->updateCoordinates();
}

void MainWindow::on_adjustBottomPushButton_clicked()
{
    QString jogCommand="$J=G91 G21 Y-"+this->incrementAmount+ " F1000\r\n";
    QByteArray qba=jogCommand.toUtf8();
    //qba.append('e');
    mcPort->write(qba);
    locY-=this->incrementAmount.toDouble();
    this->updateCoordinates();
}

void MainWindow::on_adjustLeftPushButton_clicked()
{
    QString jogCommand="$J=G91 G21 X-"+this->incrementAmount+ " F1000\r\n";
    QByteArray qba=jogCommand.toUtf8();
    //qba.append('e');
    mcPort->write(qba);
    locX-=this->incrementAmount.toDouble();
    this->updateCoordinates();
}

void MainWindow::on_adjustRightPushButton_clicked()
{
    QString jogCommand="$J=G91 G21 X" +this->incrementAmount+ " F1000\r\n";
    //qDebug()<<jogCommand;
    QByteArray qba=jogCommand.toUtf8();
    //qba.append('e');
    mcPort->write(qba);
    locX+=this->incrementAmount.toDouble();
    this->updateCoordinates();
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
    QString jogCommand="$J=G91 G21 Z-" +this->incrementAmount+ " F1000\r\n";
    //qDebug()<<jogCommand;
    QByteArray qba=jogCommand.toUtf8();
    //qba.append('e');
    mcPort->write(qba);
    locZ-=this->incrementAmount.toDouble();
    this->updateCoordinates();
}

void MainWindow::on_pushButton_4_clicked()
{
    QString jogCommand="$J=G91 G21 Z" +this->incrementAmount+ " F1000\r\n";
    //qDebug()<<jogCommand;
    QByteArray qba=jogCommand.toUtf8();
    //qba.append('e');
    mcPort->write(qba);
    locZ+=this->incrementAmount.toDouble();
    this->updateCoordinates();
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
    QString homeCommand="G0 X0 Y0 Z0\r\n";
    //qDebug()<<jogCommand;
    QByteArray qba=homeCommand.toUtf8();
    mcPort->write(qba);
    locX=0;locY=0;locZ=0;
    this->updateCoordinates();
}

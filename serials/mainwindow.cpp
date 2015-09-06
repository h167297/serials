#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iconhelper.h"
#include "myhelper.h"
#include "scanthread.h"
#ifdef Q_OS_LINUX
#include "commondispatcher.h"
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),sendflag(true),fileopen(false),
    portOpened(false),isCustomBaudRate(false),isFirst(true),
    autoOpen(true),
    DisplayBuffer(""),currentOpenPath("."),currentSavePath("."),
    #ifdef Q_OS_WIN
    currentSettingsPath("."),
    #endif
    #ifdef Q_OS_LINUX
    currentSettingsPath("/usr/lib/serials"),
    #endif
    RecSize(0),SendSize(0)
{
    ui->setupUi(this);
    #ifdef Q_OS_LINUX
    serial = new serialsDispatcher;
    #endif
    localHostName = QHostInfo::localHostName();
    Welcome = QString("大家好!我是"+localHostName+"。");
    #ifdef Q_OS_WIN
    serial = new QSerialPort;
    #endif
    time = new QTimer;

    int x=(QApplication::desktop()->width() - this->width())/2;
    int y=(QApplication::desktop()->height() - this->height())/2;
    setGeometry ( x,y, this->width(),this->height());
    available = qApp->desktop()->availableGeometry();

    addLabelOnStatusBar(status_1st,QSize(132,17),QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    addLabelOnStatusBar(status_2nd,QSize(230,17),QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
    addPermanent(status_per);

    intValidator = new QIntValidator(0, 4000000);
    ui->BaudComboBox->setInsertPolicy(QComboBox::NoInsert);

    ScanPort = new ScanThread();
    ScanPort->start();

    ui->BaudComboBox->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    ui->BaudComboBox->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    ui->BaudComboBox->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    ui->BaudComboBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->BaudComboBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->BaudComboBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->BaudComboBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->BaudComboBox->addItem(QStringLiteral("自定义"));
    ui->BaudComboBox->setCurrentIndex(3);

    ui->DataComboBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->DataComboBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->DataComboBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->DataComboBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->DataComboBox->setCurrentIndex(3);

    ui->ParityComboBox->addItem(QStringLiteral("无"), QSerialPort::NoParity);
    ui->ParityComboBox->addItem(QStringLiteral("偶校验"), QSerialPort::EvenParity);
    ui->ParityComboBox->addItem(QStringLiteral("奇校验"), QSerialPort::OddParity);
    ui->ParityComboBox->addItem(QStringLiteral("0校验"), QSerialPort::SpaceParity);
    ui->ParityComboBox->addItem(QStringLiteral("1校验"), QSerialPort::MarkParity);

    ui->StopComboBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    #ifdef Q_OS_WIN
    ui->StopComboBox->addItem(QStringLiteral("1.5"),QSerialPort::OneAndHalfStop);
    #endif
    ui->StopComboBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->FCComboBox->addItem(QStringLiteral("无"), QSerialPort::NoFlowControl);
    ui->FCComboBox->addItem(QStringLiteral("硬流控"), QSerialPort::HardwareControl);
    ui->FCComboBox->addItem(QStringLiteral("软流控"), QSerialPort::SoftwareControl);


    display_update = new QTimer;
    display_update->start(50);

    connect(display_update,SIGNAL(timeout()),this,SLOT(display_update_handle()));
    connect(ScanPort,SIGNAL(portname_changed(QStringList,QStringList)),this,SLOT(update_PortName(QStringList,QStringList)));
    connect(ScanPort,SIGNAL(status_changed()),this,SLOT(PinoutSignals()));
    connect(ui->BaudComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(ui->DataComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(ui->ParityComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(ui->StopComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(ui->FCComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(status_per->per_3rd,SIGNAL(clicked(bool)),this,SLOT(counter_clear()));

    this->installEventFilter(this);
    ui->textEdit->installEventFilter(this);

    #ifdef HODE_SIZE
    ui->btnMenu_Max->setEnabled(false);
    setMaximumSize(650,650);
    #endif

    this->InitStyle();
    if(ui->HexSendcheckBox->isChecked())
    {
        QByteArray Mtemp = Welcome.toUtf8().toHex();

        ui->textEdit->setPlainText(Mtemp);
    }
    else
    {
         ui->textEdit->setPlainText(Welcome);
    }
    readSettings();

    status_1st->setText(tr("  未连接！"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete time;
    delete display_update;
    delete intValidator;
    delete status_per->per_1st;
    delete status_per->per_2nd;
    delete status_per->per_3rd;
    delete status_per;
    delete status_1st;
    delete status_2nd;
    ScanPort->stop();
    ScanPort->terminate();
    ScanPort->wait();  //等待线程结束
    delete ScanPort;
    serial->close();
    delete serial;
}

void MainWindow::update_PortName(const QStringList v_old_name, const QStringList v_new_name)
{
    disconnect(ui->SerialcomboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(on_SerialcomboBox_currentTextChanged(QString)));
    ui->SerialcomboBox->clear();
    ui->SerialcomboBox->addItems(v_new_name);
    if(isFirst)
    {
        isFirst = false;
        QString FilePath = currentSettingsPath+"//serials.ini";
        QFile file(FilePath);
        if(file.open(QFile::ReadOnly|QFile::Text))//以只写方式打开文件
        {
            QSettings wSettings(FilePath, QSettings::IniFormat);
            ui->SerialcomboBox->setCurrentText(wSettings.value("SerialPort/PortName").toString());
        }
        if(autoOpen)
        {
            emit on_OpenCom_clicked();
        }
    }
#ifndef USE_INDICATOR
    if(!ui->OpenCom->isEnabled())
#else
    if(portOpened)
#endif
    {
        if(v_old_name.length()>v_new_name.length())
        {
            int temp_lenght=0;
           foreach(const QString &name,v_new_name) //hash table will be better
           {
            if(name!=currentSettings.name) //注意，这个currentSettings.name是拔掉之前方框内选中的值，因为串口号方框的变化未连接到updateSettings()槽
            {
                temp_lenght++;
            }
           }
           if(temp_lenght==v_new_name.length())
           {
              #ifndef USE_INDICATOR
              emit ui->CloseCom->click();
              #else
              emit ui->OpenCom->click();
              #endif
               myHelper::ShowMessageBoxError(currentSettings.name+"已断开！",this->windowFlags());
           }
        }
         ui->SerialcomboBox->setCurrentText(currentSettings.name);  //显示当前打开的串口
    }
    connect(ui->SerialcomboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(on_SerialcomboBox_currentTextChanged(QString)));
}

MainWindow::Settings MainWindow::settings() const
{
    return currentSettings;
}

bool MainWindow::time_Format_check(const QString vtime)
{
    QRegExp RegExp("[1-9]+[0-9]*");
    return RegExp.exactMatch(vtime);
}

bool MainWindow::hex_Format_check(const QString vhex)
{
    QRegExp RegExp("[0-9a-fA-F\\s]+");
    return RegExp.exactMatch(vhex);
}

void MainWindow::CRC_Parity(char *CRCArray, char nCount)
{
    unsigned char i,j;

    for (j=0;j<nCount;j++)
    {
        CRC16.CRCByte.Lo^=CRCArray[j];
        for (i=0;i<8;i++)
        {
          if (CRC16.CRCWord & 0x0001)
         {
           CRC16.CRCWord =CRC16.CRCWord >>1;
           CRC16.CRCWord =CRC16.CRCWord ^ 0xA001;//生成多项式为 G(X)=X^16+X^15+X^2+1
         }
          else
               CRC16.CRCWord =CRC16.CRCWord >>1;
        }
    }

    CRC16H=CRC16.CRCByte.Hi;
    CRC16L=CRC16.CRCByte.Lo;
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    Q_ASSERT(object == ui->textEdit||object == this||
             object == ui->lab_Title||object == ui->lab_Ico);

        if(object == this)
        {
              if (event->type() == QEvent::KeyPress)
              {
                  QKeyEvent *KeyEvent = static_cast<QKeyEvent*>(event);
                  if (KeyEvent->key() == Qt::Key_Return&&(KeyEvent->modifiers() & Qt::ControlModifier))
                  {
                      if(ui->SendData->isEnabled())
                      {
                        emit ui->SendData->clicked(true);
                      }

                      return true;
                  }
              }
        }

          if(object == ui->textEdit)
          {
             QMouseEvent *MouseEvent = static_cast<QMouseEvent*>(event);
             if(MouseEvent->button() == Qt::MidButton)
             {
                 ui->textEdit->clear();
                 return true;
             }
          }

         if( object == ui->lab_Ico )
         {
             QMouseEvent *MouseEvent = static_cast<QMouseEvent*>(event);

             if( MouseEvent->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonDblClick )
             {
                 this->on_btnMenu_Close_clicked();
                 return true;
             }
         }
         if( object == ui->lab_Title )
         {
             QMouseEvent *MouseEvent = static_cast<QMouseEvent*>(event);

             if( MouseEvent->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonDblClick )
             {
                 #ifndef HODE_SIZE
                 this->on_btnMenu_Max_clicked();
                 #endif
                 return true;
             }
             else if (MouseEvent->button() == Qt::LeftButton) {
                if(initial) //解决首次启动产生一次单击事件，导致mousePoint值异常，使窗口移动失败的问题
                {
                    initial = false;
                }
                else
                {
                     mousePressed = true;
                     mousePoint = MouseEvent->globalPos() - this->pos();
                     winRateX = (mousePoint.x()-ui->lab_Ico->width())*1.0/ui->lab_Title->width();
                     MouseEvent->accept();
                }
             }
         }

    return QMainWindow::eventFilter(object,event);
}


void MainWindow::updateSettings()
{
    currentSettings.name = ui->SerialcomboBox->currentText();
    #ifdef Q_OS_LINUX
    if (ui->BaudComboBox->currentIndex() == 7) {
        currentSettings.baudRate = ui->BaudComboBox->currentText().toInt();
    } else {
        currentSettings.baudRate = ui->BaudComboBox->itemData(ui->BaudComboBox->currentIndex()).toInt();
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = ui->DataComboBox->itemData(ui->DataComboBox->currentIndex()).toInt();
    currentSettings.stringDataBits = ui->DataComboBox->currentText();

    currentSettings.parity = ui->ParityComboBox->itemData(ui->ParityComboBox->currentIndex()).toInt();
    currentSettings.stringParity = ui->ParityComboBox->currentText();

    currentSettings.stopBits = ui->StopComboBox->itemData(ui->StopComboBox->currentIndex()).toInt();
    currentSettings.stringStopBits = ui->StopComboBox->currentText();

    currentSettings.flowControl = ui->FCComboBox->itemData(ui->FCComboBox->currentIndex()).toInt();
    currentSettings.stringFlowControl = ui->FCComboBox->currentText();

    #endif

    #ifdef Q_OS_WIN
    if (ui->BaudComboBox->currentIndex() == 7) {
        currentSettings.baudRate = ui->BaudComboBox->currentText().toInt();
    } else {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->BaudComboBox->itemData(ui->BaudComboBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->DataComboBox->itemData(ui->DataComboBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->DataComboBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->ParityComboBox->itemData(ui->ParityComboBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->ParityComboBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->StopComboBox->itemData(ui->StopComboBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->StopComboBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->FCComboBox->itemData(ui->FCComboBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->FCComboBox->currentText();

    #endif
        serial->setPortName(currentSettings.name);
    if(portOpened)
    {
        serial->setBaudRate(currentSettings.baudRate);
        serial->setDataBits(currentSettings.dataBits);
        serial->setParity(currentSettings.parity);
        serial->setStopBits(currentSettings.stopBits);
        serial->setFlowControl(currentSettings.flowControl);

        status_1st->setText(tr("  连接到 %1！").arg(currentSettings.name));
    }
}

bool MainWindow::on_OpenFile_clicked()
{

  openFileName = QFileDialog::getOpenFileName(this,
   tr("打开文件"),
   currentOpenPath,
   tr("文本文件(*.txt);;二进制文件 (*.bin);;十六进制文件 (*.hex);;所有文件 (*)"));
   if(!openFileName.isEmpty())
   {
     QFile File(openFileName);

     if(!File.open(QFile::ReadOnly|QFile::Text))//以只读方式打开文件，如果打开失败则返回，弹出对话框
     {

        QMessageBox::warning(this,tr("读取文件"),tr("无法读取文件 %1:\n %2.").arg(openFileName).arg(File.errorString()));

         return false;//%1和%2表示后面两个arg的值

     }

     QTextStream in(&File);//新建流对象，指向选定文件
    #ifdef Q_OS_WIN
     in.setCodec("GB2312");// Windows汉字本地码为GB2312,因为Qt默认以UTF-8解码中文,读取windows文本需要加上这句
    #endif
//     FileContent = in.readAll().toLocal8Bit();//转化为Linux的本地码
     FileContent = in.readAll().toUtf8();//Linux汉字本地码为UTF-8,与上句效果相同
     ui->textBrowser->setTextColor(QColor("black"));
     ui->textBrowser->setPlainText(FileContent+"\n");//将文件中所有的内容写到文本浏览器中

     currentOpenPath = QFileInfo(openFileName).canonicalFilePath();

     ui->FilePath->setText(currentOpenPath);

     ui->textBrowser->setVisible(true);//将文本设置为可见
   }

   if(!portOpened)
   {
       fileopen=true;
   }

   return true;
}

bool MainWindow::on_SaveContent_clicked()
{
    if(ui->textBrowser->toPlainText().isEmpty())
    {
        return false;
    }
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString temp = time.toString("yyyyMMddhhmmss"); //设置显示格式
    QString SavePath = currentSavePath+"//serials_"+temp;
    saveFileName = QFileDialog::getSaveFileName(this,
     tr("保存内容"),
     SavePath,
     tr("文本文件(*.txt);;所有文件 (*)"));
    if(!saveFileName.isEmpty())//文件名不为空，则保存文件
    {
        QFile file(saveFileName);

        if(!file.open(QFile::WriteOnly|QFile::Text))//以只写方式打开文件，如果打开失败则返回，弹出对话框
        {

            QMessageBox::warning(this,tr("保存内容"),tr("无法保存内容 %1:\n %2").arg(saveFileName).arg(file.errorString()));

            return false;

        }

        QTextStream out(&file);

        out<<ui->textBrowser->toPlainText();//将文本编辑器中的内容以纯文本的形式输出到流对象中

        currentSavePath = QFileInfo(saveFileName).canonicalPath();//获取文件的标准路径

    }
    return true;
}

void MainWindow::readSerial()
{
    QByteArray temp ;QString display;
    ui->textBrowser->setTextColor(QColor("blue"));
    temp = serial->readAll();
    RecSize += temp.length();
    if(sendflag)
    {
        sendflag = false;
        display = QString("\n")+QString("[")+currentSettings.name+QString("]")+QString(":\n");
        DisplayBuffer  += display.toUtf8();
    }

    if(ui->HexReccheckBox->isChecked())
    {
       temp = temp.toHex().toUpper();

       for(int i=0;i<temp.length();++i)
       {
           DisplayBuffer  += temp.at(i);
           if(i%2)
           {
               DisplayBuffer += " ";
           }
       }
    }
    else
    {
        static QByteArray TEMP ="";
        static uchar code = 3;

        for(int i=0;i<temp.length();++i)
        {
            if((temp.at(i)&0x80)!=0)//如果文本无英文换行或标点
            {
                TEMP += temp.at(i);
                if(!(--code))//UTF-8用三个字节编码常用汉字
                {
                    DisplayBuffer += TEMP;
                    TEMP ="";
                    code = 3;
                }
            }
            else
            {
                DisplayBuffer += temp.at(i);
            }
        }
    }
//    if(ui->textBrowser->toPlainText().capacity()>10240)
//    {

//        ui->textBrowser->clear();
//    }
    if(portOpened)
    {
        status_per->per_1st->setText(QString("接收：%1 B").arg(RecSize));
    }
}

void MainWindow::timeHandle()
{
    time->start(ui->Time->text().toInt());
    on_SendData_clicked();
}

void MainWindow::display_update_handle()
{
    static uchar ms=0;++ms;
    if( (ms>1) || DisplayBuffer.size() > 1024 )
    {
        ms = 0;
        if(!DisplayBuffer.isEmpty())
        {
            ui->textBrowser->insertPlainText(DisplayBuffer);
            DisplayBuffer.clear();
        }

        if(serial->isOpen())
        {
            new_status = serial->pinoutSignals();
            if(new_status != old_status)
            {
                emit status_changed();
                old_status = new_status;
            }
        }
    }
    display_update->start(50);
}

#ifndef USE_INDICATOR
void MainWindow::on_OpenCom_clicked()
{
       updateSettings();
        #ifdef Q_OS_LINUX
         if(serial->open())
        #endif

       #ifdef Q_OS_WIN
        if (serial->open(QIODevice::ReadWrite))
       #endif
        {

            serial->clear();

            serial->setPortName(currentSettings.name);
            serial->setBaudRate(currentSettings.baudRate);
            serial->setDataBits(currentSettings.dataBits);
            serial->setParity(currentSettings.parity);
            serial->setStopBits(currentSettings.stopBits);
            serial->setFlowControl(currentSettings.flowControl);
           #ifdef Q_OS_LINUX
            connect(serial->iface,SIGNAL(Dbus_serial()),this,SLOT(readSerial()));//信号和槽函数连接，当串口缓冲区有数据时，进行读串口操作
           #endif

           #ifdef Q_OS_WIN
            connect(serial,SIGNAL(readyRead()),this,SLOT(readSerial()));
           #endif
            connect(time,SIGNAL(timeout()),this,SLOT(timeHandle()));

            ui->OpenCom->setEnabled(false);
            ui->OpenCom->setStyleSheet("background-color: #A0A0A0");
            ui->CloseCom->setEnabled(true);
            ui->CloseCom->setStyleSheet("background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #4D4D4D, stop:1 #292929);");
            ui->CloseCom->setStyleSheet("QPushButton:hover{"
                "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #636363, stop:1 #575757)}");
            ui->CloseCom->setStyleSheet("QPushButton:pressed{"
                                       "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #4D4D4D, stop:1 #292929)}");

            ui->textBrowser->clear();

            ui->SaveContent->setEnabled(false);
            ui->SaveContent->setStyleSheet("background-color: #A0A0A0");

            ui->ClearDisplay->setEnabled(false);
            ui->ClearDisplay->setStyleSheet("background-color: #A0A0A0");

            ui->SerialcomboBox->setEnabled(false);

            if(fileopen)
            {
                ui->SendFile->setEnabled(true);
                ui->SendFile->setStyleSheet("background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #4D4D4D, stop:1 #292929);");
                ui->SendFile->setStyleSheet("QPushButton:hover{"
                    "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #636363, stop:1 #575757)}");
                ui->SendFile->setStyleSheet("QPushButton:pressed{"
                                           "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #4D4D4D, stop:1 #292929)}");

            }

            if(ui->TimecheckBox->isChecked())
            {
                time->start(ui->Time->text().toInt());
            }

            if((!ui->textEdit->toPlainText().isEmpty())&&(!ui->TimecheckBox->isChecked()))
            {
              ui->SendData->setEnabled(true);
              ui->SendData->setStyleSheet("background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #4D4D4D, stop:1 #292929);");
              ui->SendData->setStyleSheet("QPushButton:hover{"
                  "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #636363, stop:1 #575757)}");
              ui->SendData->setStyleSheet("QPushButton:pressed{"
                                         "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #4D4D4D, stop:1 #292929)}");

            }

            ui->statusBar->showMessage(tr(" 连接到 %1 : %2, %3, %4, %5, %6")
                                   .arg(currentSettings.name).arg(currentSettings.stringBaudRate).arg(currentSettings.stringDataBits)
                                   .arg(currentSettings.stringParity).arg(currentSettings.stringStopBits).arg(currentSettings.stringFlowControl));

    } else {

        ui->statusBar->showMessage(tr(" 打开失败！"));
    }
}

void MainWindow::on_CloseCom_clicked()
{
    serial->clear();
    serial->close();

    #ifdef Q_OS_LINUX
    disconnect(serial->iface,SIGNAL(Dbus_serial()),this,SLOT(readSerial()));
    #endif

    #ifdef Q_OS_WIN
    disconnect(serial,SIGNAL(readyRead()),this,SLOT(readSerial()));
    #endif
    ui->OpenCom->setEnabled(true);
    ui->OpenCom->setStyleSheet("background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #4D4D4D, stop:1 #292929);");
    ui->OpenCom->setStyleSheet("QPushButton:hover{"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #636363, stop:1 #575757)}");
    ui->OpenCom->setStyleSheet("QPushButton:pressed{"
                               "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #4D4D4D, stop:1 #292929)}");

    ui->CloseCom->setEnabled(false);
    ui->CloseCom->setStyleSheet( "background-color: #A0A0A0");

    ui->SendData->setEnabled(false);
    ui->SendData->setStyleSheet( "background-color: #A0A0A0");
    ui->SendFile->setEnabled(false);
    ui->SendFile->setStyleSheet( "background-color: #A0A0A0");
    ui->SerialcomboBox->setEnabled(true);

    ui->statusBar->showMessage(tr(" 未连接！"));
    time->stop();
    disconnect(time,SIGNAL(timeout()),this,SLOT(timeHandle()));
}
#endif

void MainWindow::on_BaudComboBox_currentIndexChanged(int index)
{
    isCustomBaudRate = !ui->BaudComboBox->itemData(index).isValid();
    ui->BaudComboBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->BaudComboBox->clearEditText();
        QLineEdit *edit = ui->BaudComboBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void MainWindow::on_SendData_clicked()
{
    if(!portOpened)
    {
        myHelper::ShowMessageBoxInfo("请先打开串口！",this->windowFlags());
    }
    else
    {

    serial->clear();
    ui->textBrowser->setTextColor(QColor("green"));

    if(ui->HexSendcheckBox->isChecked())
    {
        char a[2];
        QStringList temp = ui->textEdit->toPlainText().split(QRegExp(" "),QString::SkipEmptyParts);
        if(ui->CRC16checkBox->isChecked()) //先这样，以后完善
        {
            CRC16.CRCWord=0xffff;
            for(QStringList::const_iterator it=temp.cbegin();it!=temp.cend();++it)
            {
                if((*it).length()>2)
                {
                    QByteArray text="";
                    for(int i=0;i<(*it).length();++i)
                    {
                        text += (*it).at(i);
                        if((i+1)%2==0)
                        {
                            a[0] = text.toInt(0,16);
                            #ifdef Q_OS_LINUX
                            serial->write(text.toInt(0,16),1);
                            #endif
                            #ifdef Q_OS_WIN
                            serial->write(a,1);
                            #endif
                            CRC_Parity(a,1);
                            text="";
                            ++SendSize;
                        }
                    }

                }
                else
                {
                    a[0] = (*it).toInt(0,16);
                    #ifdef Q_OS_LINUX
                    serial->write((*it).toInt(0,16),1);
                    #endif
                    #ifdef Q_OS_WIN
                    serial->write(a,1);
                    #endif
                    CRC_Parity(a,1);
                    ++SendSize;
                }
            }
            #ifdef Q_OS_LINUX
            serial->write(CRC16L,1);
            serial->write(CRC16H,1);
            #endif

            #ifdef Q_OS_WIN
            a[0] = CRC16L;a[1] = CRC16H; //这里为MODBUS协议定制，先发送低字节，再发送高字节
            serial->write(a,2);
            #endif

            ++SendSize;++SendSize;
        }
        else
        {
            for(QStringList::const_iterator it=temp.cbegin();it!=temp.cend();++it)
            {
                if((*it).length()>2)
                {
                    QByteArray text="";
                    for(int i=0;i<(*it).length();++i)
                    {
                        text += (*it).at(i);
                        if((i+1)%2==0)
                        {
                            a[0] = text.toInt(0,16);
                            #ifdef Q_OS_LINUX
                            serial->write(text.toInt(0,16),1);
                            #endif
                            #ifdef Q_OS_WIN
                            serial->write(a,1);
                            #endif
                            text="";
                            ++SendSize;
                        }
                    }

                }
                else
                {
                    #ifdef Q_OS_LINUX
                    serial->write((*it).toInt(0,16),1);
                    #endif

                    #ifdef Q_OS_WIN
                    a[0] = (*it).toInt(0,16);
                    serial->write(a,1);
                    #endif
                    ++SendSize;
                }
            }
        }
    }
    else
    {
        serial->write(ui->textEdit->toPlainText().toUtf8());
        SendSize += ui->textEdit->toPlainText().toUtf8().size();
    }

    sendflag = true;
    if(!ui->textEdit->toPlainText().isEmpty())
    {
         ui->textBrowser->append("["+localHostName+"]"+":\n"+ui->textEdit->toPlainText());

         if(ui->HexSendcheckBox->isChecked())  //这种做法很傻，暂时这样做。
         {
             if(ui->CRC16checkBox->isChecked())
             {
               char a[1];

               if( *(ui->textEdit->toPlainText().end()-1) != ' ')
               {
                   ui->textBrowser->insertPlainText(" ");
               }

               ui->textBrowser->setTextColor(QColor("red"));
               a[0] = CRC16L;
               ui->textBrowser->insertPlainText(QByteArray(a,1).toHex().toUpper()+" ");
               a[0] = CRC16H;
               ui->textBrowser->insertPlainText(QByteArray(a,1).toHex().toUpper());
             }
         }
    }

    ui->textBrowser->setTextColor(QColor("blue"));
   }

    if(portOpened)
    {
        status_per->per_2nd->setText(QString("发送：%1 B").arg(SendSize));
    }
}

void MainWindow::on_ClearDisplay_clicked()
{
    ui->textBrowser->clear();
    counter_clear();
}

void MainWindow::on_textEdit_textChanged()
{
   if(ui->HexSendcheckBox->isChecked())
   {
       static int old_lenght = 0,new_lenght = 0;

       QString temp = ui->textEdit->toPlainText();
       temp.replace(QRegExp("[\\s]+"), "");

       if(!hex_Format_check(temp))
       {
           ui->textEdit->textCursor().deletePreviousChar();

           //自动格式化需要
           temp = ui->textEdit->toPlainText();
           temp.replace(QRegExp("[\\s]+"), "");
       }

       new_lenght = temp.length();

       if(old_lenght < 2)
       {
          old_lenght = new_lenght;

          disconnect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(on_textEdit_textChanged()));
          ui->textEdit->clear();
          for(int i=0;i<new_lenght;++i)
          {
              ui->textEdit->insertPlainText(QString(temp.at(i)).toUpper());
              if(i%2)
              {
                  ui->textEdit->insertPlainText(" ");
              }
          }

          connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(on_textEdit_textChanged()));

       }
      else
      {
           if( (new_lenght - old_lenght) > 1 )
           {
               old_lenght = new_lenght;
               ui->textEdit->insertPlainText(" ");
           }
       }

       if( new_lenght == 0)
       {
           old_lenght = new_lenght;
       }

       if(new_lenght %2 !=0)
       {
           old_lenght = new_lenght-1;
       }


     }

    if(ui->textEdit->toPlainText().isEmpty())
    {
        ui->TimecheckBox->setChecked(false);
        time->stop();
    }
}

void MainWindow::on_textBrowser_textChanged()
{
    ui->textBrowser->moveCursor(QTextCursor::End);
}


void MainWindow::on_TimecheckBox_clicked(bool checked)
{
    if(checked)
    {
        time->start(ui->Time->text().toInt());
    }
    else
    {
        time->stop();
    }
}

void MainWindow::on_HexSendcheckBox_clicked(bool checked)
{
   char flag = 0;
   if(time->isActive())
   {
        flag = 1;
   }
    if(checked)
    {
        QByteArray temp = ui->textEdit->toPlainText().toUtf8().toHex();
        ui->textEdit->clear();

        ui->textEdit->insertPlainText(temp);

        ui->CRC16checkBox->setCheckable(true);
    }
    else
    {
        char a[1];QByteArray content ="";
        QStringList temp = ui->textEdit->toPlainText().split(QRegExp(" "),QString::SkipEmptyParts);
        ui->textEdit->clear();
        for(QStringList::const_iterator it=temp.cbegin();it!=temp.cend();++it)
        {
            a[0] = (*it).toInt(0,16);

            content += a[0];
        }
        ui->textEdit->insertPlainText(QString(content));
        content = "";

        ui->CRC16checkBox->setChecked(false);
        ui->CRC16checkBox->setCheckable(false);
    }
    if(flag)
    {
      ui->TimecheckBox->setChecked(true);
      emit ui->TimecheckBox->clicked(true);
    }
}

void MainWindow::on_SendFile_clicked()
{

    if(!FileContent.isEmpty())
    {
        if(!portOpened)
        {
            myHelper::ShowMessageBoxInfo("请先打开串口！",this->windowFlags());
        }
        else
        {
            ui->textBrowser->clear();
            ui->textBrowser->setTextColor(QColor("green"));

            serial->clear();

            sendflag = true;

            if(!ui->textBrowser->toPlainText().isEmpty())
            {
                 ui->textBrowser->insertPlainText("\n");
            }

            ui->textBrowser->insertPlainText("["+localHostName+"]"+":\n");

            serial->write(FileContent);
            sendFileTips();
       }
    }
}

void MainWindow::on_Time_textChanged(const QString &arg1)
{
    if(!time_Format_check(arg1))
    {
        ui->Time->backspace();
    }

    if(!ui->Time->text().isEmpty())
    {
        if(!ui->textEdit->toPlainText().isEmpty())
        {
            ui->TimecheckBox->setEnabled(true);
        }
    }
    else
    {
        ui->TimecheckBox->setChecked(false);
        time->stop();
        ui->TimecheckBox->setEnabled(false);
    }
}

void MainWindow::on_Help_clicked()
{

    ui->textBrowser->setTextColor(QColor("black"));

    ui->textBrowser->setPlainText("一、RS-232串口(DB-9)\n\n");
    QString imgPath = QString(":/image/RS232_DB9.png");
    imgPathToHtml(imgPath);
    ui->textBrowser->insertHtml(imgPath);
    disconnect(ui->textBrowser,SIGNAL(textChanged()),this,SLOT(on_textBrowser_textChanged()));
    ui->textBrowser->insertPlainText("\n\n二、关于软件\n\n");
    #ifdef Q_OS_WIN
    ui->textBrowser->insertPlainText("\"串口助手\"是一个通用的串口调试软件,支持windows与Linux，此为windows版本。\n");

    ui->textBrowser->insertPlainText("Linux版本在Ubuntu 14.04上测试通过，操作串口无需root权限，"
                                     "具有与windows版本完全一致的体验，可使用以下3条命令下载并安装(3条命令需按顺序执行)：\n\n"
                                     "sudo add-apt-repository ppa:h167297/hening\n"
                                     "sudo apt-get update\n"
                                     "sudo apt-get install serials\n\n");
    ui->textBrowser->setTextColor(QColor("red"));
    ui->textBrowser->insertPlainText( "注意：请保证你的系统版本不低于Ubuntu 14.04！如果下载安装时提示缺少依赖，可先使用"
                                      "sudo apt-get update和sudo apt-get upgrade"
                                      "两条命令更新系统后再尝试下载！如仍有问题，则按照提示下载缺少的依赖包。\n");
    ui->textBrowser->setTextColor(QColor("black"));
    ui->textBrowser->insertPlainText("安装完成后将在桌面生成快捷方式，可直接双击运行，也可在终端使用serials命令启动！\n\n");
    #endif
    #ifdef Q_OS_LINUX
    ui->textBrowser->insertPlainText("\"串口助手\"是一个通用的串口调试软件,支持windows与Linux，此为Linux版本（菜单功能暂未开发）。\n");
    #endif
    ui->textBrowser->setTextColor(QColor("red"));
    ui->textBrowser->insertPlainText( "注意：中文为UTF-8编码,中文发送过程中更改串口参数可能导致乱码！笔记本无串口，串口号一项将为空！\n");
    ui->textBrowser->setTextColor(QColor("black"));
    ui->textBrowser->insertPlainText("如果出现了乱码请关闭串口后再打开，如仍然有乱码，则重启本软件。\n");
#ifdef Release
    ui->textBrowser->setTextColor(QColor("red"));
    ui->textBrowser->insertPlainText("本软件以GPL协议发布，转载或修改本软件后请注明作者并附上修改后的源码！\n");
#endif

    ui->textBrowser->setTextColor(QColor("green"));

    ui->textBrowser->insertPlainText("\n当前版本：V1.0.4\n\n");

    ui->textBrowser->setTextColor(QColor("black"));

    ui->textBrowser->insertPlainText("作者列表：\n姓名：贺宁  联系邮箱：h167297@qq.com  \n博客地址：");

    ui->textBrowser->insertHtml("http://blog.csdn.net/he_ning");

    QFile file(":/image/SerialsUpdate.txt");
    if(!file.open(QFile::ReadOnly|QFile::Text))//以只读方式打开文件，如果打开失败则返回，弹出对话框
    {
        return ;
    }
    QTextStream in(&file);
    in.setCodec("GB2312");//这个文件时在windows上编辑的
    ui->textBrowser->insertPlainText("\n\n三、更新日志\n");
    ui->textBrowser->insertPlainText("\n"+in.readAll().toUtf8());
    connect(ui->textBrowser,SIGNAL(textChanged()),this,SLOT(on_textBrowser_textChanged()));
}



void MainWindow::on_CloseFile_clicked()
{
    FileContent = QByteArray("");
    ui->FilePath->clear();
    fileopen=false;
}

void MainWindow::InitStyle()
{
    //设置窗体标题栏隐藏
//    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->setWindowFlags(Qt::FramelessWindowHint);
    location = this->geometry();
    max = false;
    lock = false;
    mousePressed = false;
    top = false;
    firstmove = true;
    initial = true;

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);

    ui->lab_Ico->installEventFilter(this);

    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Min, QChar(0xf068), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_lock,QChar(0xf09c),10);
#ifdef Q_OS_WIN
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf17a), 11);
#endif
#ifdef Q_OS_LINUX
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf17c), 11);
#endif
//    QImage IcoFile(":/image/wICO.png");
//    ui->lab_Ico->setPixmap(QPixmap::fromImage(IcoFile));
}

int MainWindow::TitleWidth(int width)
{
    return (width-ui->btnMenu->width()-ui->btnMenu_Close->width()
            -ui->btnMenu_lock->width()-ui->btnMenu_Max->width()
            -ui->btnMenu_Min->width()-ui->lab_Ico->width());
}

void MainWindow::writeSettings()
{
    updateSettings();
    QString SavePath = currentSettingsPath+"/serials.ini";
    #ifdef Q_OS_LINUX
    commondispatcher wSettings(SavePath);
    #endif
    #ifdef Q_OS_WIN
    QSettings wSettings(SavePath, QSettings::IniFormat);
    #endif
    wSettings.setValue("SerialPort/PortName",currentSettings.name);
    wSettings.setValue("SerialPort/BaudRate",currentSettings.baudRate);
    wSettings.setValue("SerialPort/DataBits",currentSettings.dataBits);
    wSettings.setValue("SerialPort/Parity",currentSettings.stringParity);
    wSettings.setValue("SerialPort/StopBits",currentSettings.stringStopBits);
    wSettings.setValue("SerialPort/FlowControl",currentSettings.stringFlowControl);
    wSettings.setValue("SerialPort/CustomBaudRate",isCustomBaudRate);

    wSettings.setValue("PinoutSignals/DataTerminalReady",ui->DTRcheckBox->isChecked());
    wSettings.setValue("PinoutSignals/RequestToSend",ui->RTScheckBox->isChecked());

    wSettings.setValue("RecieveControl/HexRec",ui->HexReccheckBox->isChecked());

    wSettings.setValue("SendControl/HexSend",ui->HexSendcheckBox->isChecked());
    wSettings.setValue("SendControl/CRC16",ui->CRC16checkBox->isChecked());
//    wSettings.setValue("SendControl/DelaySend",ui->TimecheckBox->isChecked());
    wSettings.setValue("SendControl/DelayValue",ui->Time->text());

    wSettings.setValue("AppSettings/Geometry",this->geometry());
    wSettings.setValue("AppSettings/PreGeometry",location);
    wSettings.setValue("AppSettings/WindowMax",max);
    wSettings.setValue("AppSettings/WindowLock",lock);

    wSettings.setValue("BaseSettings/OpenPath",currentOpenPath);
    wSettings.setValue("BaseSettings/SavePath",currentSavePath);
    wSettings.setValue("BaseSettings/SettingsPath",currentSettingsPath);
    wSettings.setValue("BaseSettings/SendValue",ui->textEdit->toPlainText());
}

void MainWindow::readSettings()
{
    QString FilePath = currentSettingsPath+"/serials.ini";
    QFile file(FilePath);

    if(file.open(QFile::ReadOnly|QFile::Text))//以只读方式打开文件，如果打开失败则返回，弹出对话框
    {
        #ifdef Q_OS_LINUX
        commondispatcher wSettings(FilePath);
        #endif
        #ifdef Q_OS_WIN
        QSettings wSettings(FilePath, QSettings::IniFormat);
        #endif
        if(wSettings.value("SerialPort/CustomBaudRate").toBool())
        {
            ui->BaudComboBox->setEditable(true);
            ui->BaudComboBox->clearEditText();
            QLineEdit *edit = ui->BaudComboBox->lineEdit();
            edit->setText(wSettings.value("SerialPort/BaudRate").toString());
        }
        else
        {
            ui->BaudComboBox->setCurrentText(wSettings.value("SerialPort/BaudRate").toString());
        }
        ui->DataComboBox->setCurrentText(wSettings.value("SerialPort/DataBits").toString());
        ui->ParityComboBox->setCurrentText(wSettings.value("SerialPort/Parity").toString());
        ui->StopComboBox->setCurrentText(wSettings.value("SerialPort/StopBits").toString());
        ui->FCComboBox->setCurrentText(wSettings.value("SerialPort/FlowControl").toString());

        ui->DTRcheckBox->setChecked(wSettings.value("PinoutSignals/DataTerminalReady").toBool());
        ui->RTScheckBox->setChecked(wSettings.value("PinoutSignals/RequestToSend").toBool());

        ui->HexReccheckBox->setChecked(wSettings.value("RecieveControl/HexRec").toBool());

        ui->HexSendcheckBox->setChecked(wSettings.value("SendControl/HexSend").toBool());
        ui->CRC16checkBox->setChecked(wSettings.value("SendControl/CRC16").toBool());
//        ui->TimecheckBox->setChecked(wSettings.value("SendControl/DelaySend").toBool());
        ui->Time->setText(wSettings.value("SendControl/DelayValue").toString());

        this->setGeometry(wSettings.value("AppSettings/Geometry").toRect());
        location = wSettings.value("AppSettings/PreGeometry").toRect();
        max = wSettings.value("AppSettings/WindowMax").toBool();
        if(max)
        {
            IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
            ui->btnMenu_Max->setToolTip("还原");
            this->setMinimumSize(this->width (),this->height ());
        }
        lock = !wSettings.value("AppSettings/WindowLock").toBool();
        emit on_btnMenu_lock_clicked();

        currentOpenPath = wSettings.value("BaseSettings/OpenPath").toString();
        currentSavePath = wSettings.value("BaseSettings/SavePath").toString();
        currentSettingsPath = wSettings.value("BaseSettings/SettingsPath").toString();
        ui->textEdit->setText(wSettings.value("BaseSettings/SendValue").toString());
    }
}

void MainWindow::imgPathToHtml(QString &path)
{
    path = QString("<img src=\"%1\"/>").arg(path);
}

void MainWindow::addLabelOnStatusBar(QLabel *&statusLabel, const QSize &minisize,QSizePolicy sizepolicy )
{
    statusLabel = new QLabel;
    statusLabel->setMinimumSize(minisize);

    statusLabel->setSizePolicy(sizepolicy);
    statusLabel->setFont(QFont("微软雅黑",11));
    statusLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    statusLabel->setStyleSheet("border-radius:0px;"
                               "color: #F0F0F0;"
                               "background-color:rgba(0,0,0,0);"
                               "border-style:none;");
    ui->statusBar->addWidget(statusLabel);
}

void MainWindow::addPermanent(Permanent *& per)
{
    per = new Permanent;

    per->per_1st = new QLabel;
    per->per_1st->setFont(QFont("微软雅黑",11));
    per->per_1st->setAlignment(Qt::AlignCenter);
    per->per_1st->setFixedSize(110,18);
    per->per_1st->setStyleSheet("border-radius:0px;"
                                "color: #F0F0F0;"
                                "background-color:rgba(0,0,0,0);"
                                "border-style:none;");
    per->per_1st->setText(" 版本：V1.0.4 ");
    ui->statusBar->addPermanentWidget(per->per_1st);

    per->per_2nd = new QLabel;
    per->per_2nd->setFont(QFont("微软雅黑",11));
    per->per_2nd->setAlignment(Qt::AlignCenter);
    per->per_2nd->setFixedSize(110,18);
    per->per_2nd->setStyleSheet("border-radius:0px;"
                                "color: #F0F0F0;"
                                "background-color:rgba(0,0,0,0);"
                                "border-style:none;");
    per->per_2nd->setText(
      "<style>a{text-decoration:none}</style>"
      "<a style = 'color:white;'"
      "href=\"http://blog.csdn.net/he_ning\">— 博客地址 —</a>");
    per->per_2nd->setOpenExternalLinks(true);
    ui->statusBar->addPermanentWidget(per->per_2nd);

    per->per_3rd = new QPushButton;
    per->per_3rd->setFocusPolicy(Qt::NoFocus);
    per->per_3rd->setFixedSize(25,18);
    per->per_3rd->setToolTip("windows");
    per->per_3rd->setStyleSheet("border-radius:0px;");
    per->per_3rd->setStyleSheet("hover"
    "{background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(25, 134, 199, 0), stop:1 #636363);}");
#ifdef Q_OS_WIN
    per->per_3rd->setToolTip("windows");
    IconHelper::Instance()->SetIcon(per->per_3rd, QChar(0xf17a), 11);
#endif
#ifdef Q_OS_LINUX
   per->per_3rd->setToolTip("Linux");
   IconHelper::Instance()->SetIcon(per->per_3rd, QChar(0xf17c), 11);
#endif
    ui->statusBar->addPermanentWidget(per->per_3rd);
}

void MainWindow::setPermanent(MainWindow::Permanent *&per, Qt::Alignment align, QChar text, const QString &tips)
{
    per->per_1st->setText("接收：0 B");
    per->per_1st->setAlignment(align);

    per->per_2nd->setText("发送：0 B");
    per->per_2nd->setAlignment(align);

    per->per_3rd->setToolTip(tips);
    IconHelper::Instance()->SetIcon(per->per_3rd, text, 11);
}

void MainWindow::originPermanent(Permanent *& per)
{
    per->per_1st->setAlignment(Qt::AlignCenter);
    per->per_1st->setText(" 版本：V1.0.4 ");

    per->per_2nd->setAlignment(Qt::AlignCenter);
    per->per_2nd->setText(
      "<style>a{text-decoration:none}</style>"
      "<a style = 'color:white;'"
      "href=\"http://blog.csdn.net/he_ning\">— 博客地址 —</a>");
    per->per_2nd->setOpenExternalLinks(true);
#ifdef Q_OS_WIN
    per->per_3rd->setToolTip("windows");
    IconHelper::Instance()->SetIcon(per->per_3rd, QChar(0xf17a), 11);
#endif
#ifdef Q_OS_LINUX
   per->per_3rd->setToolTip("Linux");
   IconHelper::Instance()->SetIcon(per->per_3rd, QChar(0xf17c), 11);
#endif
}

void MainWindow::sendFileTips()
{
    int baud = currentSettings.stringBaudRate.toInt();
    int size = FileContent.size();
    SendSize += size;
    short databits = currentSettings.stringDataBits.toShort();
    int n = size*8.0/databits>int(size*8.0/databits)?int(size*8.0/databits)+1:int(size*8.0/databits);
    float apend = 1 + currentSettings.stringStopBits.toFloat();
    if(currentSettings.parity != QSerialPort::NoParity)
    {
        ++apend;
    }
    apend = n*apend;
    float sendsize = size*8.0 + apend;
    float SendTime = sendsize/baud;
    QString sizeUnit = "B",timeUnit = "s";
    QString msg = QString("编码方式：UTF-8  文件大小：%1 %2  预计耗时：%3 %4").arg(size).arg(sizeUnit).arg(SendTime).arg(timeUnit);
    ui->textBrowser->insertPlainText(msg);

    if(portOpened)
    {
        status_per->per_2nd->setText(QString("发送：%1 B").arg(SendSize));
    }
}
#ifdef Q_OS_LINUX
bool MainWindow::isInternal(QPoint mouse, QRect window)
{
    if(((mouse.x() > window.x()) && (mouse.x() < (window.x() + window.width())))
         && ((mouse.y() >window.y()) && (mouse.y()<(window.y()+32))))
    {
        return true;
    }
    return false;
}
#endif

void MainWindow::PinoutSignals()
{
    QSerialPort::PinoutSignals  temp = serial->pinoutSignals();
    if(portOpened)
    {
        bool dcd = temp & 0x08;
        bool dsr = temp & 0x10;
        bool cts = temp & 0x80;
        bool ri  = temp & 0x20;
        bool rts = temp & 0x40;

        QString DCD = QString("DCD = %1 ").arg(int(dcd));
        QString DSR = QString("DSR = %1 ").arg(int(dsr));
        QString CTS = QString("CTS = %1 ").arg(int(cts));
        QString RI  = QString("RI = %1 ").arg(int(ri));

        QString signal = DCD + DSR + CTS + RI ;
        status_2nd->setText(signal);

        ui->RTScheckBox->setChecked(rts);
    }
}

void MainWindow::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setMinimumSize(650,650);
        this->setGeometry(location);
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip("最大化");
    } else {
        location = this->geometry();
#ifdef Q_OS_LINUX
        this->setGeometry(available.x(),available.y(),available.width()-12,available.height()-12);
#endif
#ifdef Q_OS_WIN
        this->setGeometry(available);
#endif
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip("还原");
        this->setMinimumSize(this->width (),this->height ());
    }
    max = !max;
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) //注释掉了拖到顶最大化功能
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
#ifndef HODE_SIZE
        if(firstmove)
        {
            firstmove = false;
            location = this->geometry();
        }
#endif
        this->move(e->globalPos() - mousePoint);
#ifndef HODE_SIZE
        if(e->globalY() == 0)
        {
            top = true;
        }
#endif

#ifdef Q_OS_LINUX
        if(!isInternal(e->globalPos(),this->geometry()))
        {
            this->setGeometry(e->globalX()-this->width()/2,e->globalY()-16,this->width(),this->height());
        }
#endif
        e->accept();
    }
#ifndef HODE_SIZE
    else if (mousePressed && (e->buttons() && Qt::LeftButton) && max) {
        max = !max;
        this->setMinimumSize(650,650);
        this->setGeometry(e->globalPos().x()-(ui->lab_Ico->width()+TitleWidth(location.width())*winRateX),0,location.width(),location.height());
        mousePoint = e->globalPos() - this->pos();
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip("最大化");
    }
#endif
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
#ifndef HODE_SIZE
    firstmove = true;
    if(top)
    {
        top = false;
#ifdef Q_OS_LINUX
        this->setGeometry(available.x(),available.y(),available.width()-12,available.height()-12);
#endif
#ifdef Q_OS_WIN
        this->setGeometry(available);
#endif
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip("还原");
        this->setMinimumSize(this->width (),this->height ());
        max = !max;
    }
#endif
}

void MainWindow::on_btnMenu_clicked()
{

}

void MainWindow::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}

void MainWindow::on_btnMenu_Close_clicked()
{
    writeSettings();
    qApp->exit();
}


void MainWindow::on_btnMenu_lock_clicked()
{
    static Qt::WindowFlags wflags = this->windowFlags();
    if(lock)
    {
        this->setWindowFlags(wflags);
        this->show();
        IconHelper::Instance()->SetIcon(ui->btnMenu_lock, QChar(0xf09c), 10);
        ui->btnMenu_lock->setToolTip("前端显示");
    }
    else
    {
        this->setWindowFlags(wflags | Qt::WindowStaysOnTopHint);
        this->show();
        IconHelper::Instance()->SetIcon(ui->btnMenu_lock, QChar(0xf023), 10);
        ui->btnMenu_lock->setToolTip("取消前端显示");
    }

    lock = !lock;
}

#ifdef USE_INDICATOR
void MainWindow::on_OpenCom_clicked()
{
    if(!portOpened)
    {
        updateSettings();

         #ifdef Q_OS_LINUX
          if(serial->open())
         #endif

        #ifdef Q_OS_WIN
         if (serial->open(QIODevice::ReadWrite))
        #endif
         {
             portOpened = true;
             serial->clear();

             serial->setBaudRate(currentSettings.baudRate);
             serial->setDataBits(currentSettings.dataBits);
             serial->setParity(currentSettings.parity);
             serial->setStopBits(currentSettings.stopBits);
             serial->setFlowControl(currentSettings.flowControl);
             serial->setDataTerminalReady(ui->DTRcheckBox->isChecked());
             serial->setRequestToSend(ui->RTScheckBox->isChecked());


             PinoutSignals();//Linux尚未考虑
            #ifdef Q_OS_LINUX
             connect(serial->iface,SIGNAL(Dbus_serial()),this,SLOT(readSerial()));//信号和槽函数连接，当串口缓冲区有数据时，进行读串口操作
            #endif

            #ifdef Q_OS_WIN
             connect(serial,SIGNAL(readyRead()),this,SLOT(readSerial()));
            #endif
             connect(time,SIGNAL(timeout()),this,SLOT(timeHandle()));

             ui->Indicator->setStyleSheet("background-image: url(:/image/Indicator_green.png);"
                                          "background-repeat:no-repeat;"
                                          "background-position: 50% 50%;");
             ui->OpenCom->setText("关闭");

             setPermanent(status_per,Qt::AlignLeft,QChar(0xf1b8),"计数清零");

             if(ui->TimecheckBox->isChecked())
             {
                 time->start(ui->Time->text().toInt());
             }

             status_1st->setText(tr("  连接到 %1！").arg(currentSettings.name));


     } else {

         status_1st->setText(tr("  打开失败！"));
     }
    }
    else
    {
       portOpened = false;
       serial->clear();
       serial->close();
       //猜测发送数据用的
       #ifdef Q_OS_LINUX
       disconnect(serial->iface,SIGNAL(Dbus_serial()),this,SLOT(readSerial()));
       #endif

       #ifdef Q_OS_WIN
       disconnect(serial,SIGNAL(readyRead()),this,SLOT(readSerial()));
       #endif

       ui->Indicator->setStyleSheet("background-image: url(:/image/Indicator_gray.png);"
                                    "background-repeat:no-repeat;"
                                    "background-position: 50% 50%;");
       ui->OpenCom->setText("打开");

       status_1st->setText(tr("  未连接！"));
       status_2nd->clear();
       originPermanent(status_per);
       time->stop();
       disconnect(time,SIGNAL(timeout()),this,SLOT(timeHandle()));
    }
}
#endif

void MainWindow::on_DTRcheckBox_clicked(bool checked)
{
    if(portOpened)
    {
        serial->setDataTerminalReady(checked);

    }
}

void MainWindow::on_RTScheckBox_clicked(bool checked)
{
    if(portOpened)
    {
        serial->setRequestToSend(checked);
    }
}

void MainWindow::on_SerialcomboBox_currentTextChanged(const QString &arg1)
{
    if(serial->isOpen())
    {
        if(!arg1.isEmpty())
        {
            emit on_OpenCom_clicked();
            RecSize = SendSize = 0;
            emit on_OpenCom_clicked();
        }
    }
}

void MainWindow::counter_clear()
{
    if(serial->isOpen())
    {
        RecSize = SendSize = 0;
        status_per->per_1st->setText("接收：0 B");
        status_per->per_2nd->setText("发送：0 B");
    }
}

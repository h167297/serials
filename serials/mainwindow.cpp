#include "mainwindow.h"
#include "ui_mainwindow.h"
//#define Release
Thread::Thread()
{
    stopped = false;
}

void Thread::run()
{
    while(!stopped)
    {
        port_name.clear();
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
           port_name.append(info.portName());

        if(port_name!=old_port_name)
        {
            emit portname_changed(old_port_name,port_name);
            old_port_name = port_name;
        }
    }
}

void Thread::stop()
{
    stopped = true;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),sendflag(true),
    currentOpenPath("."),currentSavePath(".")
{
    ui->setupUi(this);
    serialsdbus = new serialsDispatcher;
    int x=(QApplication::desktop()->width() - this->width())/2;
    int y=(QApplication::desktop()->height() - this->height())/2;
    setGeometry ( x,y, this->width(),this->height());

    localHostName = QHostInfo::localHostName();
    Welcome = QString("大家好!我是"+localHostName+"。");

    serial = new QSerialPort;
    time = new QTimer;
    intValidator = new QIntValidator(0, 4000000);
    ui->BaudComboBox->setInsertPolicy(QComboBox::NoInsert);

    Scan_serialport.start();

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
    ui->ParityComboBox->addItem(QStringLiteral("奇校验"), QSerialPort::OddParity);
    ui->ParityComboBox->addItem(QStringLiteral("偶校验"), QSerialPort::EvenParity);

    ui->StopComboBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    ui->StopComboBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->FCComboBox->addItem(QStringLiteral("无"), QSerialPort::NoFlowControl);
    ui->FCComboBox->addItem(QStringLiteral("硬流控"), QSerialPort::HardwareControl);
    ui->FCComboBox->addItem(QStringLiteral("软流控"), QSerialPort::SoftwareControl);

    if(ui->HexSendcheckBox->isChecked())
    {
        QByteArray Mtemp = Welcome.toLocal8Bit().toHex();
        for(int i=0;i<Mtemp.length();++i)
        {
            ui->textEdit->insertPlainText(QString(Mtemp.at(i)).toUpper());
            if(i%2)
            {
                ui->textEdit->insertPlainText(" ");
            }
        }
    }
    else
    {
         ui->textEdit->setPlainText(Welcome);
    }

    connect(&Scan_serialport,SIGNAL(portname_changed(QStringList,QStringList)),this,SLOT(update_PortName(QStringList,QStringList)));
    connect(ui->BaudComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(ui->DataComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(ui->ParityComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(ui->StopComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));
    connect(ui->FCComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateSettings()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete serial;
    delete time;
    delete intValidator;
    serialsdbus->serial_close();
    Scan_serialport.stop();
    Scan_serialport.terminate();
    Scan_serialport.wait();
    delete serialsdbus;
}

void MainWindow::update_PortName(const QStringList v_old_name, const QStringList v_new_name)
{

    ui->SerialcomboBox->clear();
    ui->SerialcomboBox->addItems(v_new_name);

    if(!ui->OpenCom->isEnabled())
    {
        if(v_old_name.length()>v_new_name.length())
        {
            int temp_lenght=0;
           foreach(const QString &name,v_new_name) //hash table will be better
           {
            if(name!=currentSettings.name)
            {
                temp_lenght++;
            }
           }
           if(temp_lenght==v_new_name.length())
           {
            emit ui->CloseCom->click();
            QMessageBox::critical(NULL, "critical", currentSettings.name+"已断开！","确定");
           }
        }
         ui->SerialcomboBox->setCurrentText(currentSettings.name);  //显示当前打开的串口
    }
}

MainWindow::Settings MainWindow::settings() const
{
    return currentSettings;
}

bool MainWindow::time_Format_check(const QString vtime)
{
    int z_num=0;
    for(int i=0;i<vtime.length();++i)
    {
        if((vtime.toLatin1().at(i)<'0')||vtime.toLatin1().at(i)>'9')
        {
            return false;
        }
        if(vtime.toLatin1().at(i)=='0')
        {
            ++z_num;
        }
    }
    if(z_num==vtime.length())
    {
        return false;
    }
    return true;
}

bool MainWindow::hex_Format_check(const QString vhex)
{
    for(int i=0;i<vhex.length();++i)
    {
        if(vhex.at(i)!=0x20)
        {
            if(!((vhex.at(i)>='0')&&(vhex.at(i)<='9')))
            {
                if(!((vhex.at(i)>='a')&&(vhex.at(i)<='f')))
                {
                   if(!((vhex.at(i)>='A')&&(vhex.at(i)<='F')))
                   {
                        return false;
                   }
                }
            }
        }
    }
    return true;
}


void MainWindow::updateSettings()
{
    currentSettings.name = ui->SerialcomboBox->currentText();

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

    serialsdbus->serial_setPortName(currentSettings.name);
    serialsdbus->serial_setBaudRate(currentSettings.baudRate);
    serialsdbus->serial_setDataBits(currentSettings.dataBits);
    serialsdbus->serial_setParity(currentSettings.parity);
    serialsdbus->serial_setStopBits(currentSettings.stopBits);
    serialsdbus->serial_setFlowControl(currentSettings.flowControl);
    if(!ui->OpenCom->isEnabled())
    {
        ui->statusBar->showMessage(tr(" 连接到 %1 : %2, %3, %4, %5, %6")
                               .arg(currentSettings.name).arg(currentSettings.stringBaudRate).arg(currentSettings.stringDataBits)
                               .arg(currentSettings.stringParity).arg(currentSettings.stringStopBits).arg(currentSettings.stringFlowControl));
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
     //in.setCodec("GB2312");// Windows汉字本地码为GB2312,因为Qt默认以UTF-8解码中文,读取windows文本需要加上这句
     FileContent = in.readAll(); //转码改在服务端进行
     //FileContent = in.readAll().toLocal8Bit();//转化为Linux的本地码
     //FileContent = in.readAll().toUtf8();//Linux汉字本地码为UTF-8,与上句效果相同
     ui->textBrowser->setTextColor(QColor("black"));
     ui->textBrowser->setPlainText(FileContent.toUtf8()+"\n");//将文件中所有的内容写到文本浏览器中

     currentOpenPath = QFileInfo(openFileName).canonicalFilePath();

     ui->FilePath->setText(currentOpenPath);

     ui->textBrowser->setVisible(true);//将文本设置为可见

     ui->CloseFile->setEnabled(true);

   }

   ui->SendFile->setEnabled(true);

   return true;
}

bool MainWindow::on_SaveContent_clicked()
{
    saveFileName = QFileDialog::getSaveFileName(this,
     tr("保存内容"),
     currentSavePath,
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

        currentSavePath = QFileInfo(saveFileName).canonicalFilePath();//获取文件的标准路径
    }
    return true;
}

void MainWindow::readSerial()
{
    QByteArray temp ;
    ui->textBrowser->setTextColor(QColor("blue"));
    if(sendflag)
    {
        sendflag = false;
        ui->textBrowser->append(currentSettings.name+":\n");
    }

    if(ui->HexReccheckBox->isChecked())
    {
       temp = serialsdbus->serial_readAll().toHex().toUpper();
       for(int i=0;i<temp.length();++i)
       {
           ui->textBrowser->insertPlainText(QString(temp.at(i)));
           if(i%2)
           {
             ui->textBrowser->insertPlainText(" ");
           }
       }
    }
    else
    {
        static QByteArray TEMP ="";static uchar code = 3;
        temp = serialsdbus->serial_readAll();
        for(int i=0;i<temp.length();++i)
        {
            if((temp.at(i)&0x80)!=0)//如果文本无英文换行或标点
            {
                TEMP += temp.at(i);
                if(!(--code))//UTF-8用三个字节编码常用汉字
                {
                    ui->textBrowser->insertPlainText(TEMP);
                    TEMP ="";
                    code = 3;
                }
            }
            else
            {
                ui->textBrowser->insertPlainText(QString(temp.at(i)));
            }
        }
    }
    //qDebug()<<ui->textBrowser->toPlainText().capacity();
    if(ui->textBrowser->toPlainText().capacity()>10240)
    {

        ui->textBrowser->clear();
    }
}

void MainWindow::timeHandle()
{
    time->start(ui->Time->text().toInt());
    on_SendData_clicked();
}

void MainWindow::on_OpenCom_clicked()
{  
       //serialsdbus->serial_close();
       updateSettings();
     if(serialsdbus->serial_open()){

            serialsdbus->serial_clear();

            serialsdbus->serial_setPortName(currentSettings.name);
            serialsdbus->serial_setBaudRate(currentSettings.baudRate);
            serialsdbus->serial_setDataBits(currentSettings.dataBits);
            serialsdbus->serial_setParity(currentSettings.parity);
            serialsdbus->serial_setStopBits(currentSettings.stopBits);
            serialsdbus->serial_setFlowControl(currentSettings.flowControl);

            connect(serialsdbus->iface,SIGNAL(Dbus_serial()),this,SLOT(readSerial()));//信号和槽函数连接，当串口缓冲区有数据时，进行读串口操作
            connect(time,SIGNAL(timeout()),this,SLOT(timeHandle()));

            ui->OpenCom->setEnabled(false);
            ui->CloseCom->setEnabled(true);
            ui->OpenFile->setEnabled(true);
            ui->textEdit->setEnabled(true);
            ui->textBrowser->clear();
            ui->SaveContent->setEnabled(false);
            ui->ClearDisplay->setEnabled(false);

            ui->SerialcomboBox->setEnabled(false);

            if(ui->TimecheckBox->isChecked())
            {
                time->start(ui->Time->text().toInt());
            }

            if((!ui->textEdit->toPlainText().isEmpty())&&(!ui->TimecheckBox->isChecked()))
            {
              ui->SendData->setEnabled(true);
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
    serialsdbus->serial_clear();
    serialsdbus->serial_close();
    disconnect(serialsdbus->iface,SIGNAL(Dbus_serial()),this,SLOT(readSerial()));
    ui->OpenCom->setEnabled(true);
    ui->CloseCom->setEnabled(false);
    ui->OpenFile->setEnabled(false);
    ui->textEdit->setEnabled(false);
    ui->SendData->setEnabled(false);

    ui->SerialcomboBox->setEnabled(true);
    ui->statusBar->showMessage(tr(" 未连接！"));
    time->stop();
    disconnect(time,SIGNAL(timeout()),this,SLOT(timeHandle()));
}

void MainWindow::on_BaudComboBox_currentIndexChanged(int index)
{
    bool isCustomBaudRate = !ui->BaudComboBox->itemData(index).isValid();
    ui->BaudComboBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->BaudComboBox->clearEditText();
        QLineEdit *edit = ui->BaudComboBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void MainWindow::on_SendData_clicked()
{
    serialsdbus->serial_clear();
    ui->textBrowser->setTextColor(QColor("green"));
    if(ui->HexSendcheckBox->isChecked())
    {
        QStringList temp = ui->textEdit->toPlainText().split(QRegExp(" "),QString::SkipEmptyParts);

        for(QStringList::const_iterator it=temp.cbegin();it!=temp.cend();++it)
        {       
            serialsdbus->serial_write((*it).toInt(0,16));
        }
    }
    else
    {
        serialsdbus->serial_write(ui->textEdit->toPlainText());
    }

    sendflag = true;
    if(!ui->textEdit->toPlainText().isEmpty())
    {
//        if(!ui->textBrowser->toPlainText().isEmpty())
//        {
//             ui->textBrowser->insertPlainText("\n");
//        }

//        ui->textBrowser->insertPlainText(localHostName+":\n"+ui->textEdit->toPlainText()+"\n");
        ui->textBrowser->append(localHostName+":\n"+ui->textEdit->toPlainText());
    }
}

void MainWindow::on_ClearDisplay_clicked()
{
    ui->textBrowser->clear();
    ui->ClearDisplay->setEnabled(false);
    ui->SaveContent->setEnabled(false);
}

void MainWindow::on_textEdit_textChanged()
{
   if(ui->HexSendcheckBox->isChecked())
   {
      QByteArray temp = ui->textEdit->toPlainText().toLatin1();
       if(!hex_Format_check(temp))
       {
            ui->textEdit->textCursor().deletePreviousChar();
       }
   }
    if(!ui->textEdit->toPlainText().isEmpty())
    {
        if(!ui->OpenCom->isEnabled())
        {
            if(!ui->TimecheckBox->isChecked())
            {
                ui->SendData->setEnabled(true);
                ui->TimecheckBox->setEnabled(true);
            }
        }
    }
    else
    {
        ui->SendData->setEnabled(false);
        ui->TimecheckBox->setChecked(false);
        time->stop();
        ui->TimecheckBox->setEnabled(false);
    }
}

void MainWindow::on_textBrowser_textChanged()
{
    ui->ClearDisplay->setEnabled(true);
    ui->SaveContent->setEnabled(true);
    ui->textBrowser->moveCursor(QTextCursor::End);
}


void MainWindow::on_TimecheckBox_clicked(bool checked)
{
    if(checked)
    {    
        time->start(ui->Time->text().toInt());
        if(!ui->OpenCom->isEnabled())
        {
            ui->SendData->setEnabled(false);
        }
    }
    else
    {
        time->stop();
        if(!ui->OpenCom->isEnabled())
        {
            ui->SendData->setEnabled(true);
        }
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
        for(int i=0;i<temp.length();++i)
        {
            ui->textEdit->insertPlainText(QString(temp.at(i)).toUpper());
            if(i%2)
            {
                ui->textEdit->insertPlainText(" ");
            }
        }
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
    }
    if(flag)
    {
      ui->TimecheckBox->setChecked(true);
      emit ui->TimecheckBox->clicked(true);
    }
}

void MainWindow::on_SendFile_clicked()
{
    serialsdbus->serial_clear();
    ui->textBrowser->setTextColor(QColor("green"));
    serialsdbus->serial_write(FileContent);
    sendflag = true;

        if(!ui->textBrowser->toPlainText().isEmpty())
        {
             ui->textBrowser->insertPlainText("\n");
        }

        ui->textBrowser->insertPlainText(localHostName+":\n"+FileContent+"\n");

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
    ui->textBrowser->setPlainText("\"串口助手\"是一个通用的串口调试软件,提供windows与Linux两个版本;\n"
                                  "\"串口助手\"提供如下功能:\n"
                                  "①ASCII码、十六进制和中文收发\n"
                                  "②自动扫描可用串口（笔记本电脑上一般没有串口，若未接“U转串”，串口号一项将无内容）\n"
                                  "③文件发送和接收\n"
                                  "④缓冲区自动清空\n"
                                  "⑤在线参数更改\n"
                                  "⑥内容存储\n"
                                  "⑦定时发送\n"
                                  "⑧互动显示\n");
    ui->textBrowser->setTextColor(QColor("red"));
    ui->textBrowser->insertPlainText( "注意：中文为UTF-8编码,与GB2312编码不兼容,中文发送过程中更改串口参数可能导致乱码！\n");
    ui->textBrowser->setTextColor(QColor("black"));
    ui->textBrowser->insertPlainText("如果出现了乱码请关闭串口后再打开，如仍然有乱码，则重启本软件。\n");
#ifdef Release
    ui->textBrowser->setTextColor(QColor("red"));
    ui->textBrowser->insertPlainText("本软件以GPL协议发布，转载或修改本软件后请注明作者并附上修改后的源码！\n");
#endif
    ui->textBrowser->setTextColor(QColor("black"));
    ui->textBrowser->insertPlainText("作者列表：\n姓名：贺宁  联系邮箱：h167297@qq.com  \n博客地址：");

    ui->textBrowser->insertHtml("http://blog.csdn.net/he_ning");
}



void MainWindow::on_CloseFile_clicked()
{
    FileContent = QByteArray("");
    ui->FilePath->clear();
    ui->SendFile->setEnabled(false);
    ui->CloseFile->setEnabled(false);
}

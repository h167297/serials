#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QHostInfo>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#ifdef Q_OS_LINUX
#include <serialsdispatcher.h>
#endif
namespace Ui {
    class MainWindow;
}

class QIntValidator;

class Thread :public QThread
{
    Q_OBJECT

public :
    Thread();
    void stop();

protected:
    void run();

signals:
    void portname_changed(const QStringList v_old_name,const QStringList v_new_name);
private:
    QStringList port_name;
    QStringList old_port_name;
    volatile bool stopped;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    #ifdef Q_OS_LINUX
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        int dataBits;
        QString stringDataBits;
        int parity;
        QString stringParity;
        int stopBits;
        QString stringStopBits;
        int flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    #endif

#ifdef Q_OS_WIN
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
#endif
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
        Settings settings() const;
        bool time_Format_check(const QString vtime );
        bool hex_Format_check(const QString vhex);
        void CRC_Parity(char *CRCArray, char nCount);
        bool eventFilter(QObject *object, QEvent *event);

private slots:
    void update_PortName(const QStringList v_old_name,const QStringList v_new_name);

    bool on_OpenFile_clicked();

    bool on_SaveContent_clicked();

    void on_OpenCom_clicked();

    void on_BaudComboBox_currentIndexChanged(int index);

    void on_CloseCom_clicked();

    void on_SendData_clicked();
    
    void on_ClearDisplay_clicked();

    void on_textEdit_textChanged();

    void on_textBrowser_textChanged();

    void updateSettings();

    void readSerial();

    void timeHandle();

    void on_TimecheckBox_clicked(bool checked);

    void on_HexSendcheckBox_clicked(bool checked);

    void on_SendFile_clicked();

    void on_Time_textChanged(const QString &arg1);

    void on_Help_clicked();

    void on_CloseFile_clicked();

private:
    Ui::MainWindow *ui;
    bool sendflag;
    bool fileopen;
#ifdef Q_OS_WIN
    QSerialPort *serial;
#endif
    Settings currentSettings;
    QIntValidator *intValidator;
    QTimer *time;
    QString currentOpenPath;
    QString openFileName;
    QByteArray FileContent;
    QString currentSavePath;
    QString saveFileName;
    QString localHostName;
    QString Welcome;
    Thread Scan_serialport;
    #ifdef Q_OS_LINUX
    serialsDispatcher *serial;
    #endif

    char CRC16H; //暂时这样用，以后再改
    char CRC16L;

    union CRC     //网上原程序是将这个联合体放在CRC_Parity函数内部
    {
        unsigned short CRCWord;
        struct
        {
          char  Lo;
          char  Hi;
        }CRCByte;
     }CRC16;
};

#endif // MAINWINDOW_H

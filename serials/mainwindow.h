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
#include "scanthread.h"

//#define HODE_SIZE


#define USE_INDICATOR

namespace Ui {
    class MainWindow;
}

class QIntValidator;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
        Settings settings() const;
        bool time_Format_check(const QString vtime );
        bool hex_Format_check(const QString vhex);
        void CRC_Parity(char *CRCArray, char nCount);
protected:
        bool eventFilter(QObject *object, QEvent *event);
        void mouseMoveEvent(QMouseEvent *e);
//        void mousePressEvent(QMouseEvent *e);
        void mouseReleaseEvent(QMouseEvent *);

signals:
    void portname_changed(const QStringList v_old_name,const QStringList v_new_name);
    void status_changed();

private slots:
    void update_PortName(const QStringList v_old_name,const QStringList v_new_name);

    bool on_OpenFile_clicked();

    bool on_SaveContent_clicked();

    void on_BaudComboBox_currentIndexChanged(int index);

    #ifndef USE_INDICATOR
    void on_OpenCom_clicked();

    void on_CloseCom_clicked();
    #endif

    void on_SendData_clicked();

    void on_ClearDisplay_clicked();

    void on_textEdit_textChanged();

    void on_textBrowser_textChanged();

    void updateSettings();

    void readSerial();

    void timeHandle();

    void display_update_handle();

    void on_TimecheckBox_clicked(bool checked);

    void on_HexSendcheckBox_clicked(bool checked);

    void on_SendFile_clicked();

    void on_Time_textChanged(const QString &arg1);

    void on_Help_clicked();

    void on_CloseFile_clicked();

    void on_btnMenu_Max_clicked();

    void on_btnMenu_clicked();

    void on_btnMenu_Min_clicked();

    void on_btnMenu_Close_clicked();

    void on_btnMenu_lock_clicked();

    #ifdef USE_INDICATOR
    void on_OpenCom_clicked();
    #endif

    void on_DTRcheckBox_clicked(bool checked);

    void on_RTScheckBox_clicked(bool checked);

    void PinoutSignals();

    void on_SerialcomboBox_currentTextChanged(const QString &arg1);

    void counter_clear();

private:
    Ui::MainWindow *ui;
    bool sendflag;
    bool fileopen;
    bool portOpened;
    bool isCustomBaudRate;
    bool isFirst;  //只在初始化时有效
    bool autoOpen;
#ifdef Q_OS_WIN
    QSerialPort *serial;
#endif
    Settings currentSettings;
    QIntValidator *intValidator;
    QTimer *time;
    QTimer *display_update;
    QString openFileName;
    QByteArray FileContent;
    QByteArray DisplayBuffer;
    QString currentOpenPath;
    QString currentSavePath;
    QString currentSettingsPath;
    QString saveFileName;
    QString localHostName;
    QString Welcome;
    ScanThread *ScanPort;
    QLabel *status_1st;
    QLabel *status_2nd;

    struct Permanent
    {
        QLabel *per_1st;
        QLabel *per_2nd;
        QPushButton *per_3rd;
    };

    Permanent *status_per;

    #ifdef Q_OS_LINUX
    serialsDispatcher *serial;
    #endif

    char CRC16H; //暂时这样用，以后再改
    char CRC16L;

    union CRC     //
    {
        unsigned short CRCWord;
        struct
        {
          char  Lo;
          char  Hi;
        }CRCByte;
     }CRC16;
   //下面是跟窗口样式有关的变量和函数
    QPoint mousePoint;
    bool mousePressed;
    bool max;
    bool lock;
    bool top;
    bool firstmove;
    bool initial;
    int RecSize;
    int SendSize;
    QRect location;
    QRect available;
    float winRateX;
    QSerialPort::PinoutSignals  new_status;
    QSerialPort::PinoutSignals  old_status;
    void InitStyle();
    int  TitleWidth(int width);
    void writeSettings();
    void readSettings();
    void imgPathToHtml(QString &path);
    void addLabelOnStatusBar(QLabel *&statusLabel, const QSize &minisize, QSizePolicy sizepolicy);
    void addPermanent(Permanent *& per);
    void setPermanent(Permanent *& per, Qt::Alignment align, QChar text,const QString &tips);
    void originPermanent(Permanent *& per);
    void sendFileTips();
#ifdef Q_OS_LINUX
    bool isInternal(QPoint mouse, QRect window);
#endif
};

#endif // MAINWINDOW_H

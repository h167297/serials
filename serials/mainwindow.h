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
#include <serialsdispatcher.h>

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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
        Settings settings() const;
        bool time_Format_check(const QString vtime );
        bool hex_Format_check(const QString vhex);

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
    QSerialPort *serial;
    Settings currentSettings;
    QIntValidator *intValidator;
    QTimer *time;
    QString currentOpenPath;
    QString openFileName;
    QString FileContent;
    QString currentSavePath;
    QString saveFileName;
    QString localHostName;
    QString Welcome;
    Thread Scan_serialport;
    serialsDispatcher *serialsdbus;
};

#endif // MAINWINDOW_H

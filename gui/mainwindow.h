#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qserialport.h>
#include <qserialportinfo.h>
#include <qstringview.h>

#include "serialport.h"
#include "ui_mainwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    // ASCII / HEX is a pure UI presentation choice — core never sees it (see DEVELOPMENT.md §10).
    enum class TextMode { Ascii, Hex };

    tessera::core::SerialPort serial_;
    QString selected_port_name_;
    qint32                   selected_baud()         const { return ui->baud_rate->currentData().toInt(); }
    QSerialPort::DataBits    selected_data_bits()    const { return ui->data_bits->currentData().value<QSerialPort::DataBits>(); }
    QSerialPort::StopBits    selected_stop_bits()    const { return ui->stop_bits->currentData().value<QSerialPort::StopBits>(); }
    QSerialPort::Parity      selected_parity()       const { return ui->parity->currentData().value<QSerialPort::Parity>(); }
    QSerialPort::FlowControl selected_flow_control() const { return ui->flow_control->currentData().value<QSerialPort::FlowControl>(); }
    TextMode                 selected_send_mode()    const { return ui->send_mode->currentData().value<TextMode>(); }
    TextMode                 selected_receive_mode() const { return ui->receive_mode->currentData().value<TextMode>(); }

private slots:
    void on_connect_clicked();
    void on_send_clicked();
    void handle_data_received(const QByteArray& bytes);

private:
    enum class LogLevel
    {
        Normal,
        Success,
        Warn,
        Error
    };

    void log_event(const QString& msg, LogLevel level = LogLevel::Normal);
};
#endif // MAINWINDOW_H

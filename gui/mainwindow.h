#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qserialport.h>
#include <qserialportinfo.h>
#include <qstringview.h>

#include "portpopup.h"
#include "serialport.h"
#include "ui_mainwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QTimer;
QT_END_NAMESPACE

class ThemeManager;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ThemeManager *theme, QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    tessera::core::SerialPort serial_;
    PortPopup *port_popup_;
    SettingsDialog *settings_;
    QTimer *repeat_timer_;   // drives repeated sends when Repeat is enabled

    // ASCII / HEX is a pure UI presentation choice — core never sees it (see DEVELOPMENT.md §10).
    enum class TextMode { Ascii, Hex };

    QString selected_port_name_;

    // tracks whether the next received char begins a fresh line, so the timestamp
    // is stamped once per real line regardless of how the byte stream is fragmented.
    bool receive_at_line_start_ = true;
    qint32                   selected_baud()         const { return ui->baud_rate->currentData().toInt(); }
    QSerialPort::DataBits    selected_data_bits()    const { return ui->data_bits->currentData().value<QSerialPort::DataBits>(); }
    QSerialPort::StopBits    selected_stop_bits()    const { return ui->stop_bits->currentData().value<QSerialPort::StopBits>(); }
    QSerialPort::Parity      selected_parity()       const { return ui->parity->currentData().value<QSerialPort::Parity>(); }
    QSerialPort::FlowControl selected_flow_control() const { return ui->flow_control->currentData().value<QSerialPort::FlowControl>(); }
    TextMode                 selected_send_mode()    const { return ui->send_mode->currentData().value<TextMode>(); }
    TextMode                 selected_receive_mode() const { return ui->receive_mode->currentData().value<TextMode>(); }

private slots:
    void on_connect_clicked();
    void on_repeat_clicked();   // Start/Stop button: toggles repeated sending
    bool do_send();             // performs one send; returns whether anything was actually sent
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

    // prefixes a timestamp at the start of each line in the received text
    QString stamp_lines(const QString& in);

    // enter / leave the repeating state, flipping the Start/Stop button and lockouts
    void start_repeat();
    void stop_repeat();

    // recompute Send / repeat enabled-state from connection, content and run state
    void refresh_send_controls();
};
#endif // MAINWINDOW_H

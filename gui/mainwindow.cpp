#include <QTime>
#include <QDateTime>
#include <QTimer>
#include <QScreen>
#include <QTextCursor>
#include <QScrollBar>

#include "mainwindow.h"
#include "portpopup.h"
#include "serialport.h"
#include "ui_mainwindow.h"
#include "core.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Call into core once to confirm gui is actually linked against it (phase 0 done-criterion).
    setWindowTitle(QStringLiteral("Tessera %1").arg(tessera::core::version()));


    // to connect port
    connect(ui->connect_button, &QPushButton::clicked, 
            this, &MainWindow::on_connect_clicked);
    // to send message (single shot)
    connect(ui->send_button, &QPushButton::clicked,
            this, &MainWindow::do_send);
    // to render message
    connect(&serial_, &tessera::core::SerialPort::data_received,
            this, &MainWindow::handle_data_received);
    // to clear receive area
    connect(ui->clear_receive, &QPushButton::clicked,
            ui->receive_area, &QPlainTextEdit::clear);
    // to clear send area
    connect(ui->clear_send, &QPushButton::clicked,
            ui->send_area, &QPlainTextEdit::clear);

    
    ui->baud_rate->setItemData(0, 1200);
    ui->baud_rate->setItemData(1, 2400);
    ui->baud_rate->setItemData(2, 4800);
    ui->baud_rate->setItemData(3, 9600);
    ui->baud_rate->setItemData(4, 19200);
    ui->baud_rate->setItemData(5, 38400);
    ui->baud_rate->setItemData(6, 57600);
    ui->baud_rate->setItemData(7, 115200);

    ui->data_bits->setItemData(0, QSerialPort::Data5);
    ui->data_bits->setItemData(1, QSerialPort::Data6);
    ui->data_bits->setItemData(2, QSerialPort::Data7);
    ui->data_bits->setItemData(3, QSerialPort::Data8);

    ui->stop_bits->setItemData(0, QSerialPort::OneStop);
    ui->stop_bits->setItemData(1, QSerialPort::OneAndHalfStop);
    ui->stop_bits->setItemData(2, QSerialPort::TwoStop);

    ui->parity->setItemData(0, QSerialPort::NoParity);
    ui->parity->setItemData(1, QSerialPort::EvenParity);
    ui->parity->setItemData(2, QSerialPort::OddParity);
    ui->parity->setItemData(3, QSerialPort::SpaceParity);
    ui->parity->setItemData(4, QSerialPort::MarkParity);

    ui->flow_control->setItemData(0, QSerialPort::NoFlowControl);
    ui->flow_control->setItemData(1, QSerialPort::HardwareControl);
    ui->flow_control->setItemData(2, QSerialPort::SoftwareControl);

    ui->receive_mode->setItemData(0, QVariant::fromValue(TextMode::Ascii));
    ui->receive_mode->setItemData(1, QVariant::fromValue(TextMode::Hex));

    ui->send_mode->setItemData(0, QVariant::fromValue(TextMode::Ascii));
    ui->send_mode->setItemData(1, QVariant::fromValue(TextMode::Hex));

    ui->line_ending->setItemData(0, "");
    ui->line_ending->setItemData(1, "\r");    // CR
    ui->line_ending->setItemData(2, "\n");    // LF
    ui->line_ending->setItemData(3, "\r\n");  // CRLF

    port_popup_ = new PortPopup(this);
    connect(ui->port_button, &QPushButton::clicked,
            this, [this] {
                port_popup_->populate();
                port_popup_->adjustSize();   // settle size before positioning

                const QRect screen = ui->port_button->screen()->availableGeometry();
                const QSize sz     = port_popup_->size();
                const QPoint btn   = ui->port_button->mapToGlobal(QPoint(0, 0));

                // Right-align the popup to the button and drop below it (§9).
                int x = btn.x() + ui->port_button->width() - sz.width();
                int y = btn.y() + ui->port_button->height();

                // Flip above the button if there isn't room below.
                if (y + sz.height() > screen.bottom())
                    y = btn.y() - sz.height();

                // Clamp into the screen so the popup never leaves it.
                x = qBound(screen.left(), x, screen.right()  - sz.width());
                y = qBound(screen.top(),  y, screen.bottom() - sz.height());

                port_popup_->move(x, y);
                port_popup_->show();
            });
    connect(port_popup_, &PortPopup::port_selected,
            this, [this](const QString& name) {
                selected_port_name_ = name;
                ui->port_button->setText(name);
                ui->port_button->setProperty("selected", true);
                ui->port_button->style()->polish(ui->port_button);

                ui->connect_button->setEnabled(true);
            });

    // status-bar wall clock, ticking once a second
    auto* clock = new QTimer(this);
    connect(clock, &QTimer::timeout, this, [this] {
        ui->status_time->setText(
            QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    });
    clock->start(1000);
    // fire once now so it doesn't sit on the placeholder for the first second
    ui->status_time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    // repeat: a timer that re-runs do_send() on a fixed interval, driven by its own
    // Start/Stop button. Send stays a pure single-shot; the two never overlap.
    repeat_timer_ = new QTimer(this);
    connect(repeat_timer_, &QTimer::timeout, this, &MainWindow::do_send);
    connect(ui->repeat_button, &QPushButton::clicked,
            this, &MainWindow::on_repeat_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_connect_clicked()
{
    if (!serial_.is_open())  // if serial is closed
    {
        bool connection_ok = serial_.open(selected_port_name_, selected_baud(), 
                                          selected_data_bits(), selected_stop_bits(),
                                          selected_parity(), selected_flow_control());

        if (connection_ok)
        {
            // enable sending and the repeat controls (all need an open port)
            ui->send_button->setEnabled(true);
            ui->repeat_button->setEnabled(true);
            ui->repeat_interval->setEnabled(true);

            // change connect button text and status
            ui->connect_button->setText(QStringLiteral("Disconnect"));
            ui->connect_button->setChecked(true);

            // disable options in serial configuration
            ui->port_button->setEnabled(false);
            ui->baud_rate->setEnabled(false);
            ui->data_bits->setEnabled(false);
            ui->stop_bits->setEnabled(false);
            ui->parity->setEnabled(false);
            ui->flow_control->setEnabled(false);

            // change connection status (at bottom inforamtion area) 
            ui->conn_dot->setText(QStringLiteral("● Connected: %1").arg(serial_.port_name()));
            ui->conn_dot->setProperty("off", false);
            ui->conn_dot->style()->polish(ui->conn_dot);  // reload style
            
            // output to EVENT LOG area
            log_event(QStringLiteral("Opened: %1").arg(serial_.port_name()), LogLevel::Success);
        }
        else
        {
            ui->connect_button->setChecked(false);
            log_event(QStringLiteral("Open failed: %1").arg(serial_.port_name()), LogLevel::Error);
        }
    }
    else  // if serial is opened
    {
        serial_.close();

        // stop any running repeat and disable its controls
        stop_repeat();
        ui->repeat_button->setEnabled(false);
        ui->repeat_interval->setEnabled(false);

        ui->send_button->setEnabled(false);

        ui->connect_button->setText(QStringLiteral("Connect"));
        ui->connect_button->setChecked(false);

        ui->port_button->setEnabled(true);
        ui->baud_rate->setEnabled(true);
        ui->data_bits->setEnabled(true);
        ui->stop_bits->setEnabled(true);
        ui->parity->setEnabled(true);
        ui->flow_control->setEnabled(true);
        
        ui->conn_dot->setText(QStringLiteral("○ DISCONNECTED"));
        ui->conn_dot->setProperty("off", true);
        ui->conn_dot->style()->polish(ui->conn_dot);  // reload style

        log_event(QStringLiteral("Closed: %1").arg(serial_.port_name()), LogLevel::Normal);
    }
}

void MainWindow::on_repeat_clicked()
{
    if (repeat_timer_->isActive()) { stop_repeat(); return; }

    // send the first frame immediately; only enter repeat mode if it actually went out
    if (!do_send()) return;
    start_repeat();
}

void MainWindow::start_repeat()
{
    repeat_timer_->start(ui->repeat_interval->value());
    ui->repeat_button->setText(QStringLiteral("Stop"));
    ui->repeat_interval->setEnabled(false);   // lock the interval while running
    ui->send_button->setEnabled(false);       // no single send during a repeat
}

void MainWindow::stop_repeat()
{
    repeat_timer_->stop();
    ui->repeat_button->setText(QStringLiteral("Start"));
    ui->repeat_interval->setEnabled(true);
    ui->send_button->setEnabled(true);
}

bool MainWindow::do_send()
{
    const QString text = ui->send_area->toPlainText();
    if (text.isEmpty()) return false;   // nothing to send; also stops repeat from spamming empties

    const QString line_ending = ui->line_ending->currentData().toString();

    // convert message into bytes (QByteArray)
    QByteArray bytes;
    if (selected_send_mode() == TextMode::Ascii) bytes = text.toUtf8();
    else /* HEX */                               bytes = QByteArray::fromHex(text.toUtf8());
    bytes.append(line_ending.toUtf8());

    // send message and give feedback
    bool send_ok = serial_.send(bytes);
    if (send_ok) ui->status_tx->setText(QStringLiteral("TX %1 B").arg(serial_.write_count()));
    else         log_event(QStringLiteral("Failed to send message: %1").arg(text), LogLevel::Error);

    return send_ok;
}

void MainWindow::handle_data_received(const QByteArray& bytes)
{
    QString text;
    if (selected_receive_mode() == TextMode::Ascii)
    {
        text = QString::fromUtf8(bytes);
        text.remove('\r');
    }
    else text = QString::fromLatin1(bytes.toHex(' ') + ' ');

    // prefix each line with an arrival timestamp when TS is on
    if (ui->timestamp_toggle->isChecked()) text = stamp_lines(text);

    // append at document end without dragging the viewport
    QTextCursor cur(ui->receive_area->document());
    cur.movePosition(QTextCursor::End);
    cur.insertText(text);

    // follow to the bottom only when AUTO is on; otherwise stay where the user scrolled
    if (ui->autoscroll_toggle->isChecked())
    {
        auto* sb = ui->receive_area->verticalScrollBar();
        sb->setValue(sb->maximum());
    }

    // update RX count
    ui->status_rx->setText(QStringLiteral("RX %1 B").arg(serial_.read_count()));
}

void MainWindow::log_event(const QString& msg, LogLevel level)
{
    QString color;
    switch (level) 
    {
        case LogLevel::Normal : color = "#9AA7B4"; break; 
        case LogLevel::Success: color = "#5BC98A"; break;
        case LogLevel::Warn   : color = "#E6C34A"; break;
        case LogLevel::Error  : color = "#E5705F"; break;
    }

    const QString ts = QTime::currentTime().toString("HH:mm:ss");
    ui->event_log->appendHtml(
        QStringLiteral("<span style='color:%1'>[%2] %3</span>").arg(color, ts, msg)
    );
}

QString MainWindow::stamp_lines(const QString& in)
{
    const QString ts = '[' + QTime::currentTime().toString("HH:mm:ss.zzz") + "] ";
    QString out;
    for (const QChar c : in)
    {
        if (receive_at_line_start_) { out += ts; receive_at_line_start_ = false; }
        out += c;
        if (c == '\n') receive_at_line_start_ = true;
    }
    return out;
}
#include <QTime>
#include <QScreen>

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
    // to send message
    connect(ui->send_button, &QPushButton::clicked,
            this, &MainWindow::on_send_clicked);
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
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_connect_clicked()
{
    if (!serial_.is_open())
    {
        bool connection_ok = serial_.open(selected_port_name_, selected_baud(), 
                                          selected_data_bits(), selected_stop_bits(),
                                          selected_parity(), selected_flow_control());

        if (connection_ok)
        {
            // enable send button
            ui->send_button->setEnabled(true);
            
            // change connect button text and status
            ui->connect_button->setText(QStringLiteral("Disconnect"));
            ui->connect_button->setChecked(true);

            // change connection status (at bottom inforamtion area) 
            ui->conn_dot->setText(QStringLiteral("● Connected: %1").arg(selected_port_name_));
            ui->conn_dot->setProperty("off", false);
            ui->conn_dot->style()->polish(ui->conn_dot);  // reload style
            
            // output to EVENT LOG area
            log_event(QStringLiteral("Opened: %1").arg(selected_port_name_), LogLevel::Success);
        }
        else
        {
            ui->connect_button->setChecked(false);
            log_event(QStringLiteral("Open failed"), LogLevel::Error);
        }
    }
    else
    {
        serial_.close();
        
        ui->send_button->setEnabled(false);

        ui->connect_button->setText(QStringLiteral("Connect"));
        ui->connect_button->setChecked(false);
        
        ui->conn_dot->setText(QStringLiteral("○ DISCONNECTED"));
        ui->conn_dot->setProperty("off", true);
        ui->conn_dot->style()->polish(ui->conn_dot);  // reload style

        log_event(QStringLiteral("Closed: %1").arg(selected_port_name_), LogLevel::Normal);
    }
}

void MainWindow::on_send_clicked()
{
    const QString text = ui->send_area->toPlainText();
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
        
    // render message to receive area
    ui->receive_area->moveCursor(QTextCursor::End);
    ui->receive_area->insertPlainText(text);
    ui->receive_area->moveCursor(QTextCursor::End);

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
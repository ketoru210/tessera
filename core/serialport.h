#pragma once

#include <QSerialPort>
#include <QObject>


namespace tessera::core {

class SerialPort : public QObject
{
    Q_OBJECT

public:
    // constructor
    explicit SerialPort(QObject* parent = nullptr) 
        : QObject(parent) 
        {
            connect(&port_, &QSerialPort::readyRead, this, &SerialPort::handle_ready_read);
        }

    // getters
    bool is_open() const { return port_.isOpen(); }
    qint64 read_count()  const { return read_count_;  }
    qint64 write_count() const { return write_count_; }

    // methods
    bool open(const QString& name, qint32 baud_rate, 
              const QSerialPort::DataBits& data_bits, const QSerialPort::StopBits& stop_bits, 
              const QSerialPort::Parity& parity, const QSerialPort::FlowControl& flow_control);
    bool close();
    bool send(const QByteArray& bytes);
              
signals:
    void data_received(const QByteArray& bytes);

private:
    QSerialPort port_;
    qint64 read_count_  = 0;
    qint64 write_count_ = 0;

    void handle_ready_read();
};

}  // namespace tessera::core
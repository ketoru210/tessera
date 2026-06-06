#pragma once

#include <QSerialPort>
#include <QObject>


namespace tessera::core {

class SerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SerialPort(QObject* parent = nullptr) : QObject(parent) {}

    bool is_open() const { return port_.isOpen(); }
    bool open(const QString& name, qint32 baud_rate, 
              const QSerialPort::DataBits& data_bits, const QSerialPort::StopBits& stop_bits, 
              const QSerialPort::Parity& parity, const QSerialPort::FlowControl& flow_control);
    bool close();
              
signals:
    void data_received(const QByteArray& bytes);

private:
    QSerialPort port_;

};

}  // namespace tessera::core
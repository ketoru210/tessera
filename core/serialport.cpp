#include "serialport.h"


bool tessera::core::SerialPort::open(const QString& name, qint32 baud_rate, 
            const QSerialPort::DataBits& data_bits, const QSerialPort::StopBits& stop_bits, 
            const QSerialPort::Parity& parity, const QSerialPort::FlowControl& flow_control)
{
    if (is_open()) return false;
    
    port_.setPortName(name);
    port_.setBaudRate(baud_rate);
    port_.setDataBits(data_bits);
    port_.setStopBits(stop_bits);
    port_.setParity(parity);
    port_.setFlowControl(flow_control);

    return port_.open(QIODeviceBase::ReadWrite);
}

bool tessera::core::SerialPort::close()
{
    if (!is_open()) return false;

    port_.close();
    return true;
}
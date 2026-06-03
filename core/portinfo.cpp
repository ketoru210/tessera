#include <QSerialPortInfo>

#include "portinfo.h"
#include <qserialport.h>


std::vector<PortInfo> tessera::core::portinfo::scan_ports()
{
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    
    std::vector<PortInfo> out;
    for (auto port : serialPortInfos)
    {
        PortInfo port_info;
        port_info.name_            = port.portName();
        port_info.description_     = port.description();
        port_info.manufacturer_    = port.manufacturer();
        port_info.serial_number_   = port.serialNumber();
        port_info.system_location_ = port.systemLocation();
        if (port.hasVendorIdentifier())  port_info.vendor_identifier_  = port.vendorIdentifier();
        if (port.hasProductIdentifier()) port_info.product_identifier_ = port.vendorIdentifier();
        out.push_back(port_info);
    }
    
    return out;
}
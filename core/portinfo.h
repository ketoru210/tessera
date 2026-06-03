#pragma once

#include <QSerialPort>
#include <qobject.h>
#include <vector>
#include <optional>

struct PortInfo
{
    QString name_;
    QString description_;
    QString manufacturer_;
    QString serial_number_;
    QString system_location_;
    std::optional<qint16> vendor_identifier_;
    std::optional<qint16> product_identifier_;
};

namespace tessera::core::portinfo
{
    std::vector<PortInfo> scan_ports();
}  // namespcae tessera::core::portinfo
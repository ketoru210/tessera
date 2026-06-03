// coretest - manual console harness for exercising core without the GUI.
//
// A console (non-WIN32) executable that links core ONLY (no Widgets): stdout/qDebug
// show in the terminal, and it proves core is usable standalone. Grow this as core
// grows - port enumeration now, open/close + send/receive in later phases.
// Not an automated test suite; a hand-driven smoke-test console.

#include <QTextStream>

#include "portinfo.h"

int main()
{
    QTextStream out(stdout);

    const auto ports = tessera::core::portinfo::scan_ports();

    out << "Found " << ports.size() << " serial port(s):\n";
    for (const auto &port : ports) {
        out << '\n'
            << "  Port name:          " << port.name_            << '\n'
            << "  Description:        " << port.description_     << '\n'
            << "  Manufacturer:       " << port.manufacturer_    << '\n'
            << "  Serial number:      " << port.serial_number_   << '\n'
            << "  System location:    " << port.system_location_ << '\n'
            << "  Vendor identifier:  0x" << Qt::hex << port.vendor_identifier_  << Qt::dec << '\n'
            << "  Product identifier: 0x" << Qt::hex << port.product_identifier_ << Qt::dec << '\n';
    }

    return 0;
}

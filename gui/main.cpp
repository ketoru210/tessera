#include <QApplication>
#include <qdebug.h>
#include <qlogging.h>
#include <iostream>

#include "mainwindow.h"
#include "portinfo.h"

int main(int argc, char *argv[])
{
    std::cout << "hi\n";
    const auto port_info = tessera::core::portinfo::scan_ports();
    for (auto port : port_info)
    {
        qDebug() << "\n"
                 << "Port name: "          << port.name_               << "\n"
                 << "Description: "        << port.description_        << "\n"
                 << "Manufacturer: "       << port.manufacturer_       << "\n"
                 << "Serial number: "      << port.serial_number_      << "\n"
                 << "System location: "    << port.system_location_    << "\n"
                 << "Vendor identifier: "  << port.vendor_identifier_  << "\n"
                 << "Product identifier: " << port.product_identifier_ << "\n";
    }
    return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}

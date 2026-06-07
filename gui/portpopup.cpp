#include <algorithm>

#include "portpopup.h"
#include "portinfo.h"
#include "ui_portpopup.h"


PortPopup::PortPopup(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Popup);

    connect(ui.reload_button, &QPushButton::clicked,
            this, &PortPopup::populate);
    connect(ui.select_button, &QPushButton::clicked,
            this, &PortPopup::on_select_clicked);
    // allow double click item to select
    connect(ui.com_list, &QListWidget::itemDoubleClicked,
            this, [this] { on_select_clicked(); });
}

void PortPopup::populate()
{
    auto ports = tessera::core::portinfo::scan_ports();
    std::sort(ports.begin(), ports.end(),
              [](const PortInfo& a, const PortInfo& b) { return a.name_ < b.name_; }
    );

    ui.com_list->clear();
    for (const auto& p : ports)
    {
        auto* item = new QListWidgetItem(p.name_ + "\n" + p.description_);
        item->setData(Qt::UserRole, p.name_);
        ui.com_list->addItem(item);
    }

    ui.com_count->setText(QStringLiteral("Available Serial Ports: %1").arg(ports.size()));
}

void PortPopup::on_select_clicked()
{
    auto* item = ui.com_list->currentItem();
    if (!item) return;
    emit port_selected(item->data(Qt::UserRole).toString());
    hide();
}
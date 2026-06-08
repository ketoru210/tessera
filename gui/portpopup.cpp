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
    // refresh the detail panel whenever the highlighted port changes
    connect(ui.com_list, &QListWidget::currentItemChanged,
            this, &PortPopup::on_current_changed);
}

void PortPopup::populate()
{
    ports_ = tessera::core::portinfo::scan_ports();
    std::sort(ports_.begin(), ports_.end(),
              [](const PortInfo& a, const PortInfo& b) { return a.name_ < b.name_; }
    );

    ui.com_list->clear();
    for (int i = 0; i < static_cast<int>(ports_.size()); ++i)
    {
        const auto& p = ports_[i];
        auto* item = new QListWidgetItem(p.name_ + "\n" + p.description_);
        item->setData(Qt::UserRole, i);   // store index into ports_
        ui.com_list->addItem(item);
    }

    ui.com_count->setText(QStringLiteral("Available Serial Ports: %1").arg(ports_.size()));
    clear_detail();   // nothing highlighted right after a fresh scan
}

void PortPopup::on_select_clicked()
{
    auto* item = ui.com_list->currentItem();
    if (!item) return;
    emit port_selected(ports_[item->data(Qt::UserRole).toInt()].name_);
    hide();
}

void PortPopup::on_current_changed(QListWidgetItem* current)
{
    if (!current) { clear_detail(); return; }
    render_detail(ports_[current->data(Qt::UserRole).toInt()]);
}

void PortPopup::render_detail(const PortInfo& info)
{
    const QString dash = QStringLiteral("—");
    auto or_dash = [&](const QString& s) { return s.isEmpty() ? dash : s; };
    auto hex4 = [&](const std::optional<quint16>& v) {
        return v ? QStringLiteral("%1").arg(*v, 4, 16, QChar('0')).toUpper() : dash;
    };

    ui.detail_v_port->setText(or_dash(info.name_));
    ui.detail_v_desc->setText(or_dash(info.description_));
    ui.detail_v_mfr->setText(or_dash(info.manufacturer_));
    ui.detail_v_serial->setText(or_dash(info.serial_number_));
    ui.detail_v_location->setText(or_dash(info.system_location_));
    ui.detail_v_vid->setText(hex4(info.vendor_identifier_));
    ui.detail_v_pid->setText(hex4(info.product_identifier_));
}

void PortPopup::clear_detail()
{
    const QString dash = QStringLiteral("—");
    ui.detail_v_port->setText(dash);
    ui.detail_v_desc->setText(dash);
    ui.detail_v_mfr->setText(dash);
    ui.detail_v_serial->setText(dash);
    ui.detail_v_location->setText(dash);
    ui.detail_v_vid->setText(dash);
    ui.detail_v_pid->setText(dash);
}
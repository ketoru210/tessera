#pragma once

#include <vector>

#include <QWidget>

#include "portinfo.h"
#include "ui_portpopup.h"

class QResizeEvent;


class PortPopup : public QWidget
{
    Q_OBJECT

public:
    explicit PortPopup(QWidget* parent = nullptr);
    // No destructor needed: ui is a value member, destroyed automatically (RAII).

    void populate();

signals:
    void port_selected(const QString& name);

protected:
    // keep the popup fully on-screen when its size changes (e.g. detail panel fills)
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void on_select_clicked();
    void on_current_changed(QListWidgetItem* current);

private:
    void render_detail(const PortInfo& info);
    void clear_detail();

    Ui::PortPopup ui;
    std::vector<PortInfo> ports_;   // backs the list; items store an index into this
};
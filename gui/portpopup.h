#pragma once

#include <QWidget>

#include "ui_portpopup.h"


class PortPopup : public QWidget
{
    Q_OBJECT

public:
    explicit PortPopup(QWidget* parent = nullptr);
    // No destructor needed: ui is a value member, destroyed automatically (RAII).

    void populate();

signals:
    void port_selected(const QString& name);

private slots:
    void on_select_clicked();

private:
    Ui::PortPopup ui;
};
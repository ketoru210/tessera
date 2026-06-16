#pragma once

#include <QWidget>

class ThemeManager;
class QComboBox;

// A centered, floating settings panel that overlays the whole main window.
//
// It is a full-window child widget (not a top-level dialog): a dim backdrop
// covers the parent and a centered card holds the options. Clicking the backdrop
// or pressing Esc dismisses it. The first option is the theme picker; more
// settings rows can be added to the card's form layout.
class SettingsDialog : public QWidget
{
    Q_OBJECT

public:
    SettingsDialog(ThemeManager* theme, QWidget* parent);

    // Resize to cover the parent, then reveal centered on top of everything.
    void open();

protected:
    // Click on the backdrop (outside the card) closes the panel.
    void mousePressEvent(QMouseEvent* event) override;
    // Esc closes the panel.
    void keyPressEvent(QKeyEvent* event) override;
    // Keep the overlay matched to the parent's size while it is open.
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void build_ui();
    void populate_themes();

    ThemeManager* theme_;
    QWidget*      card_  = nullptr;   // the centered panel; clicks here don't dismiss
    QComboBox*    theme_combo_ = nullptr;
};

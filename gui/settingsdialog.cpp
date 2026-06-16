#include "settingsdialog.h"

#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

#include "thememanager.h"


SettingsDialog::SettingsDialog(ThemeManager* theme, QWidget* parent)
    : QWidget(parent), theme_(theme)
{
    setObjectName(QStringLiteral("settings_overlay"));
    // Plain QWidget: needs WA_StyledBackground for the QSS dim backdrop to paint.
    setAttribute(Qt::WA_StyledBackground, true);
    hide();

    build_ui();

    // Track the parent's resize so the overlay always covers it while open.
    parent->installEventFilter(this);
}

void SettingsDialog::build_ui()
{
    // The overlay itself only centers the card; its layout has no margins so the
    // backdrop reaches every edge.
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);

    card_ = new QFrame(this);
    card_->setObjectName(QStringLiteral("settings_card"));
    card_->setAttribute(Qt::WA_StyledBackground, true);
    card_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    outer->addWidget(card_, 0, Qt::AlignCenter);

    auto* col = new QVBoxLayout(card_);
    col->setContentsMargins(20, 16, 20, 20);
    col->setSpacing(16);

    // ---- header: title + close (✕) -------------------------------------
    auto* header = new QHBoxLayout;
    auto* title = new QLabel(QStringLiteral("Settings"), card_);
    title->setObjectName(QStringLiteral("settings_title"));
    auto* close = new QToolButton(card_);
    close->setObjectName(QStringLiteral("settings_close"));
    close->setText(QStringLiteral("✕"));
    close->setCursor(Qt::PointingHandCursor);
    connect(close, &QToolButton::clicked, this, &QWidget::hide);
    header->addWidget(title);
    header->addStretch();
    header->addWidget(close);
    col->addLayout(header);

    // ---- options form --------------------------------------------------
    auto* form = new QFormLayout;
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    theme_combo_ = new QComboBox(card_);
    theme_combo_->setObjectName(QStringLiteral("settings_theme_combo"));
    theme_combo_->setMinimumWidth(180);
    // Apply on pick. userData holds the theme file basename (e.g. "dark-gold").
    connect(theme_combo_, QOverload<int>::of(&QComboBox::activated),
            this, [this](int i) {
                if (theme_) theme_->apply(theme_combo_->itemData(i).toString());
            });

    auto* theme_label = new QLabel(QStringLiteral("Theme"), card_);
    theme_label->setObjectName(QStringLiteral("settings_label"));
    form->addRow(theme_label, theme_combo_);

    col->addLayout(form);
}

void SettingsDialog::populate_themes()
{
    if (!theme_) return;

    theme_combo_->blockSignals(true);   // repopulating, not a user pick
    theme_combo_->clear();
    for (const QString& name : theme_->available())
        theme_combo_->addItem(theme_->display_name(name), name);

    const int cur = theme_combo_->findData(theme_->current());
    if (cur >= 0) theme_combo_->setCurrentIndex(cur);
    theme_combo_->blockSignals(false);
}

void SettingsDialog::open()
{
    populate_themes();                 // reflect the live theme list + selection
    setGeometry(parentWidget()->rect());
    raise();
    show();
    setFocus();                        // so Esc reaches keyPressEvent
}

void SettingsDialog::mousePressEvent(QMouseEvent* event)
{
    // Presses that land on the card are consumed by the card; anything reaching
    // the overlay is a click on the dim backdrop → dismiss.
    if (!card_->geometry().contains(event->pos())) hide();
}

void SettingsDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) { hide(); return; }
    QWidget::keyPressEvent(event);
}

bool SettingsDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == parentWidget() && event->type() == QEvent::Resize && isVisible())
        setGeometry(parentWidget()->rect());
    return QWidget::eventFilter(watched, event);
}

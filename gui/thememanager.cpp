#include "thememanager.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStandardPaths>

// Set by CMake to the in-tree source dirs; empty strings when not provided.
#ifndef THEME_SOURCE_DIR   // themes/  — palettes
#  define THEME_SOURCE_DIR ""
#endif
#ifndef STYLE_SOURCE_DIR   // styles/  — the qss template
#  define STYLE_SOURCE_DIR ""
#endif

ThemeManager::ThemeManager(QObject* parent) : QObject(parent)
{
    // Make sure the user themes dir exists so people have somewhere to drop their
    // own palettes (needs setApplicationName() set before us — see main.cpp).
    QDir().mkpath(user_dir());

#ifdef QT_DEBUG
    // Live-edit support: re-apply the current theme whenever a source file
    // changes. Only wired up when the in-tree themes dir is actually present.
    const QString src = QStringLiteral(THEME_SOURCE_DIR);
    if (!src.isEmpty() && QDir(src).exists())
    {
        watcher_ = new QFileSystemWatcher(this);
        connect(watcher_, &QFileSystemWatcher::fileChanged, this, [this](const QString&) {
            if (!current_.isEmpty()) apply(current_);
        });
    }
#endif
}

QString ThemeManager::template_path() const
{
    const QString src = QStringLiteral(STYLE_SOURCE_DIR);
    if (!src.isEmpty() && QFile::exists(src + QStringLiteral("/theme.qss")))
        return src + QStringLiteral("/theme.qss");
    return QStringLiteral(":/styles/theme.qss");
}

QString ThemeManager::builtin_dir() const
{
    const QString src = QStringLiteral(THEME_SOURCE_DIR);
    if (!src.isEmpty() && QDir(src).exists()) return src;
    return QStringLiteral(":/themes");
}

QString ThemeManager::user_dir() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
           + QStringLiteral("/themes");
}

QString ThemeManager::palette_path(const QString& theme_name) const
{
    // A user palette shadows a built-in one of the same name.
    const QString user = user_dir() + QStringLiteral("/") + theme_name + QStringLiteral(".json");
    if (QFile::exists(user)) return user;
    return builtin_dir() + QStringLiteral("/") + theme_name + QStringLiteral(".json");
}

QString ThemeManager::read_text(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    return QString::fromUtf8(f.readAll());
}

QStringList ThemeManager::available() const
{
    QStringList out;
    const QStringList filter{QStringLiteral("*.json")};
    // Built-in first, then user; a user theme duplicating a built-in name is listed
    // once (it still overrides on load via palette_path()).
    for (const QString& dir : {builtin_dir(), user_dir()})
        for (const QFileInfo& fi : QDir(dir).entryInfoList(filter, QDir::Files))
            if (!out.contains(fi.completeBaseName()))
                out << fi.completeBaseName();
    return out;
}

QString ThemeManager::display_name(const QString& theme_name) const
{
    const QString json = read_text(palette_path(theme_name));
    if (!json.isEmpty())
    {
        const QJsonObject obj = QJsonDocument::fromJson(json.toUtf8()).object();
        const QString name = obj.value(QStringLiteral("name")).toString();
        if (!name.isEmpty()) return name;
    }
    return theme_name;   // fall back to the file basename
}

QString ThemeManager::build_qss(const QString& theme_name) const
{
    QString       qss  = read_text(template_path());
    const QString json = read_text(palette_path(theme_name));
    if (qss.isEmpty() || json.isEmpty()) return {};

    // Palette: { "name": "...", "colors": { "token": "#rrggbb", ... } }.
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
    {
        qWarning() << "ThemeManager: bad palette" << theme_name << ":" << err.errorString();
        return {};
    }

    // Substitute every @token@. The @...@ delimiters make this order-independent:
    // "@accent@" never matches inside "@accent-hover@".
    const QJsonObject colors = doc.object().value(QStringLiteral("colors")).toObject();
    for (auto it = colors.constBegin(); it != colors.constEnd(); ++it)
        qss.replace('@' + it.key() + '@', it.value().toString());

    return qss;
}

bool ThemeManager::apply(const QString& theme_name)
{
    const QString qss = build_qss(theme_name);
    if (qss.isEmpty()) return false;

    qApp->setStyleSheet(qss);
    current_ = theme_name;

    if (watcher_)
    {
        // Re-arm the watcher: editors often replace files on save, which drops
        // the old watch. Watch the template plus the active palette.
        if (!watcher_->files().isEmpty()) watcher_->removePaths(watcher_->files());
        watcher_->addPath(template_path());
        watcher_->addPath(palette_path(theme_name));
    }

    emit changed(theme_name);
    return true;
}

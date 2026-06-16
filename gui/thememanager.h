#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class QFileSystemWatcher;

// Loads a QSS template + a per-theme color palette, substitutes every @token@
// with its palette value, and applies the result to the whole application.
//
// One template (styles/theme.qss), many palettes (<name>.json). The template is
// an app-owned skeleton; adding a theme means dropping in a new .json palette —
// no code or template changes.
//
// Palettes come from two places, kept separate on purpose:
//   - built-in: shipped in the binary (":/themes", or the in-tree source dir in
//     debug for hot-reload). Read-only, always present, updates with the app.
//   - user:     a writable dir (AppData/themes) users drop custom palettes into.
// On a name clash the user file wins on load, but the engine (template) stays in
// resources — users can only break their own data, never the theme system.
//
// The template comes from ":/styles" (or the in-tree dir in debug). Debug builds
// hot-reload both via QFileSystemWatcher.
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManager(QObject* parent = nullptr);

    // Apply the named theme (e.g. "dark"). False if its files can't be read.
    bool apply(const QString& theme_name);

    QString     current()   const { return current_; }
    QStringList available() const;   // built-in + user theme names (built-in first)
    QString     user_dir()  const;   // writable dir users drop custom .json palettes into

    // Human-readable label from a palette's "name" field (falls back to the file
    // basename). Drives the settings theme picker's option text.
    QString     display_name(const QString& theme_name) const;

signals:
    void changed(const QString& theme_name);

private:
    QString        build_qss(const QString& theme_name) const;
    QString        template_path() const;                      // styles/theme.qss (app skeleton)
    QString        builtin_dir() const;                        // shipped palettes (source dir or qrc)
    QString        palette_path(const QString& theme_name) const;  // user dir wins, else built-in
    static QString read_text(const QString& path);

    QString             current_;
    QFileSystemWatcher* watcher_ = nullptr;   // non-null only when hot-reload is active
};

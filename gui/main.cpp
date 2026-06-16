#include <QApplication>
#include <qdebug.h>
#include <qlogging.h>

#include "mainwindow.h"
#include "thememanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Names the per-user data dir (AppData/Tessera) that ThemeManager derives the
    // user themes folder from — must be set before ThemeManager is constructed.
    QApplication::setApplicationName(QStringLiteral("Tessera"));

    // Skin the whole app before the first window shows. Outlives main() so its
    // hot-reload watcher (debug builds) stays alive for the session.
    ThemeManager theme;
    theme.apply(QStringLiteral("dark-gold"));   // default theme

    MainWindow w(&theme);
    w.show();
    return QApplication::exec();
}

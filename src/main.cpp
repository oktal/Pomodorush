#include <QtGui/QApplication>
#include "mainwindow.h"
#include "sql/SqlHelper.h"
#include "settings.h"

#include <QSettings>

static const int EXIT_SQL_OPEN_ERROR = 1;
static const int EXIT_LOGIN_ERROR = 2;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Pomodorush");
    a.setApplicationName("Pomodorush");

    if ( !SqlHelper::openDatabase() ) {
        return EXIT_SQL_OPEN_ERROR;
    }

    QSettings settings(a.organizationName(), a.applicationName());
    const bool firstLaunch = settings.value(SETTING_FIRSTLAUNCH, true).toBool();
    if (firstLaunch) {
        settings.setValue(SETTING_POMODOROLENGTH, 25);
        settings.setValue(SETTING_SHORTBREAK, 5);
        settings.setValue(SETTING_LONGBREAK, 15);
        settings.setValue(SETTING_POMODOROSLONGBREAK, 4);
        settings.setValue(SETTING_TIMERTICKING, true);
        settings.setValue(SETTING_TIMERBEEP, true);
        settings.setValue(SETTING_LANG, "en");
        settings.setValue(SETTING_FIRSTLAUNCH, false);
    }

    MainWindow w;
    w.show();
    
    const int result = a.exec();
    SqlHelper::closeDatabase();
    return result;
}

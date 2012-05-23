#include <QtGui/QApplication>
#include "mainwindow.h"
#include "sql/SqlHelper.h"

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

    MainWindow w;
    w.show();
    
    const int result = a.exec();
    SqlHelper::closeDatabase();
    return result;
}

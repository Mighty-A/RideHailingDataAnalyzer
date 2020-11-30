#include "mainwindow.h"
#include "filedialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileDialog dlg;
    MainWindow w;
    dlg.show();
    return a.exec();
}

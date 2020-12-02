#include "mainwindow.h"
#include "filedialog.h"

#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileDialog dlg;
    MainWindow w;

    dlg.show();
    QObject::connect(&dlg, &FileDialog::SendData, &w, &MainWindow::ReceiveShow);
    //w.show();
    return a.exec();
}

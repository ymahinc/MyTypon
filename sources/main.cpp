#include "global.h"
#include "mainwindow.h"
#include "myapplication.h"

int main(int argc, char *argv[]){

    MYApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

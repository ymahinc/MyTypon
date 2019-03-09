#ifndef MYOPTIONS_H
#define MYOPTIONS_H

// class that handle general application options
// at this time the only option is to save or not the workspace on application quit

#include <QObject>

class MYOptions: public QObject
{
    Q_OBJECT

public:
    MYOptions(QObject *parent = 0);

    bool saveWorkSpaceOnQuit;
    QString lastSavedPath;
    QString lastOpenedPath;

public slots:
    void save();
    void load();


private:

};

#endif // MYOPTIONS_H

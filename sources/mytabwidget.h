#ifndef MYTABWIDGET_H
#define MYTABWIDGET_H

/*
 * Main tabbed widget that will be used to display open typons
 */

#include <QTabWidget>
#include "typonWidget/mytypon.h"
class MYTypon;

class MYTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    MYTabWidget(QWidget * parent = 0);
    ~MYTabWidget();
    MYTypon * typonAt(int index);
    int save(MYTypon *typon, bool forceSaveAs = false);
    QString saveAs();

public slots:
    void addNewTypon();
    int closeTypon(MYTypon * typon, bool onClose = false);
    void openFile(QString filePath);

signals:
    void currentTyponHasChanged(MYTypon* typon);

private slots:
    void onCurrentChanged(int index);
    void updateTabText(QString text);
    int closeTypon(int index, bool onClose = false);

private:
    void updateRecentFiles(QString filePath);

    MYTypon *currentTypon();
    int indexOfTypon(MYTypon *typon);
};

#endif // MYTABWIDGET_H

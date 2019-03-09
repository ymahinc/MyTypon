#ifndef RECENTFILEACTION_H
#define RECENTFILEACTION_H

#include <QAction>

class RecentFileAction : public QAction
{
    Q_OBJECT

public:
    RecentFileAction(const QString &text, QObject *parent = nullptr);
    QString filePath();

signals:
    void openFileTriggered(QString filePath);

private slots:
    void onTriggered();

private:
    QString limitString(const QString& aString, int maxLength);

    QString m_filePath;
};

#endif // RECENTFILEACTION_H

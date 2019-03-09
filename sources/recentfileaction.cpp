#include "recentfileaction.h"
#include <QFileInfo>

RecentFileAction::RecentFileAction(const QString &text, QObject *parent)
    : QAction(text, parent){
    m_filePath = text;
    setText(limitString(m_filePath,50));
    setToolTip(m_filePath);
    connect(this,SIGNAL(triggered(bool)),this,SLOT(onTriggered()));
}

QString RecentFileAction::filePath(){
    return m_filePath;
}

void RecentFileAction::onTriggered(){
    emit openFileTriggered(m_filePath);
}

QString RecentFileAction::limitString(const QString& aString, int maxLength) {
    if (aString.length() <= maxLength)
        return aString;

    float spacePerPart = (maxLength - 3) / 2.0;
    auto beforeEllipsis = aString.left(std::ceil(spacePerPart));
    auto afterEllipsis = aString.right(std::floor(spacePerPart));

    return beforeEllipsis + "..." + afterEllipsis;
}

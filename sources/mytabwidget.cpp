#include "mytabwidget.h"
#include "global.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

MYTabWidget::MYTabWidget(QWidget *parent)
    : QTabWidget(parent) {
    setTabsClosable(true);
    setMovable(true);
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTypon(int)));
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(onCurrentChanged(int)));
}

MYTabWidget::~MYTabWidget(){
}

// returns index of the typon given in parameter
int MYTabWidget::indexOfTypon(MYTypon *typon){
    for (int i = 0; i < count(); i++ ){
        MYTypon* typ = qobject_cast<MYTypon*>(widget(i));
        if ( typ == typon )
            return i;
    }
    return 0;
}

// create a new typon and add it to a new tab
void MYTabWidget::addNewTypon(){
    MYTypon *typon = new MYTypon(this);
    int index = addTab(typon,typon->typonName());
    // update tab text when typon name has changed (if it was modified or saved as)
    connect(typon,SIGNAL(typonNameHasChanged(QString)),this,SLOT(updateTabText(QString)));
    setCurrentIndex(index);
}

void MYTabWidget::updateTabText(QString text){
    setTabText(currentIndex(),text);
}

MYTypon *MYTabWidget::currentTypon(){
    return qobject_cast<MYTypon*>(currentWidget());
}

MYTypon *MYTabWidget::typonAt(int index){
    return qobject_cast<MYTypon*>(widget(index));
}

int MYTabWidget::closeTypon(int index, bool onClose){
    return closeTypon(typonAt(index),onClose);
}

//close typon given in parameter
int MYTabWidget::closeTypon(MYTypon * typon, bool onClose){
    int index = indexOfTypon(typon);
    // if typon to close has been modified, ask user what to do
    if ( ! typon->undoStack()->isClean() ){
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setText(tr("Typon \"%1\" has been modified.").arg(typonAt(index)->typonName()));
        msgBox->setInformativeText(tr("Do you want to save your changes?"));
        msgBox->setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox->setDefaultButton(QMessageBox::Save);
        int ret = msgBox->exec();
        switch (ret) {
            case QMessageBox::Save:
                if ( save(typon) > 0 )
                    return -1;
                break;
            case QMessageBox::Cancel:
                return -1;
                break;
          }
    }
    // if it is last typon and not requested from destructor, add new typon
    if ( count() == 1 && ! onClose )
        addNewTypon();
    removeTab(index);
    delete typon;
    return 0;
}

int MYTabWidget::save(MYTypon *typon, bool forceSaveAs){
    if ( typon->filePath().isEmpty() || forceSaveAs ){
        QString path = saveAs();
        if ( path.isEmpty() )
            return 1;
        typon->setFilePath(path);
        updateRecentFiles(path);
        currentTypon()->setFilePath(path);
        QFileInfo fileInfo(path);
        currentTypon()->setTyponName(fileInfo.baseName());
        currentTypon()->updateTyponName(true);
    }

    QFile file(typon->filePath());
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file

    typon->save(out);

    typon->undoStack()->setClean();
    return 0;
}

QString MYTabWidget::saveAs(){
    QString fileAbsolutePath = QFileDialog::getSaveFileName(this,tr("Save Typon"),
                                      qApp->options()->lastSavedPath ,tr("Typon (*.myt);"));
    if ( ! fileAbsolutePath.isEmpty() ){
        QFileInfo fileInfo(fileAbsolutePath);
        qApp->options()->lastSavedPath = fileInfo.absolutePath();
    }

    return fileAbsolutePath;
}

void MYTabWidget::openFile(QString filePath){
    for (int i = 0; i < count(); i++){
        if ( typonAt(i)->filePath() == filePath ){
            setCurrentIndex(i);
            return;
        }
    }

    QFile file(filePath);

    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Recent Files Example"),
                             tr("This file could not be found:\n%1.")
                             .arg(filePath));
        return;
    }


    addNewTypon();
    QDataStream in(&file);
    qApp->currentTypon()->restore(in,filePath);

    updateRecentFiles(filePath);
}

void MYTabWidget::updateRecentFiles(QString filePath){
    QSettings settings("recentfiles", QSettings::IniFormat);
    QStringList recentFilePaths =
            settings.value("recentFiles").toStringList();
    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);
    while (recentFilePaths.size() > 10)
        recentFilePaths.removeLast();
    settings.setValue("recentFiles", recentFilePaths);
}

void MYTabWidget::onCurrentChanged(int index){
    if ( index >= 0 )
        emit currentTyponHasChanged(currentTypon());
}

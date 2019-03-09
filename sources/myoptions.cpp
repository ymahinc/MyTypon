#include "myoptions.h"

#include <QFile>
#include <QSettings>
#include <QStandardPaths>

MYOptions::MYOptions(QObject *parent)
    : QObject(parent){
    // if default options file does not exists create it from factory resources, save it and then load it
    if ( ! QFile::exists("mytypon.ini") ) {
        QSettings optionsSettings(":/mytypon.ini", QSettings::IniFormat);
        saveWorkSpaceOnQuit = optionsSettings.value("saveWorkSpaceOnQuit").toBool();
        lastSavedPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0);
        lastOpenedPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0);
        save();
    }        
    load();
}

// write general options to the option ini file
void MYOptions::save(){
    QSettings settings("mytypon.ini", QSettings::IniFormat);
    settings.setValue("saveWorkSpaceOnQuit",saveWorkSpaceOnQuit);
    settings.setValue("lastSavedPath",lastSavedPath);
    settings.setValue("lastOpenedPath",lastOpenedPath);
}

// load general options from ini file
void MYOptions::load(){
    QSettings optionsSettings("mytypon.ini",QSettings::IniFormat);
    saveWorkSpaceOnQuit = optionsSettings.value("saveWorkSpaceOnQuit").toBool();
    lastSavedPath = optionsSettings.value("lastSavedPath").toString();
    lastOpenedPath = optionsSettings.value("lastOpenedPath").toString();
}


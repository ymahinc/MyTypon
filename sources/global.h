#ifndef GLOBAL_H
#define GLOBAL_H

#include "myapplication.h"
#include <QCoreApplication>
#include <QPainterPath>
#include <QColor>

#ifdef qApp
#undef qApp
#define qApp qobject_cast<MYApplication*>(QCoreApplication::instance())
#endif

#endif // GLOBAL_H

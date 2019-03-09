#-------------------------------------------------
#
# Project created by QtCreator 2017-04-24T19:09:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myTypon2017
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    workspace/mytypongrid.cpp \
    workspace/myworkspace.cpp \
    utils/mycolorbutton.cpp \
    utils/mytyponunits.cpp \
    utils/trackcurrentcalculator.cpp \
    myoptions.cpp \
    mydockwidget.cpp \
    mytabwidget.cpp \
    utils/myunitsspinbox.cpp \
    utils/myunitscombobox.cpp \
    generaloptionswidget.cpp \
    optionsdialog.cpp \
    myapplication.cpp \
    typonWidget/mytypon.cpp \
    typonWidget/mytyponscene.cpp \
    typonWidget/mytyponscrollbar.cpp \
    typonWidget/mytyponview.cpp \
    typonWidget/ruler/mytyponruler.cpp \
    layers/addlayerundocommand.cpp \
    layers/editlayerundocommand.cpp \
    layers/layer.cpp \
    layers/layeroptionsdialog.cpp \
    layers/removelayerundocommand.cpp \
    gridwidget.cpp \
    utils/myunitsmenulabel.cpp \
    typonoptionswidget.cpp \
    layers/layersstack.cpp \
    undowidget.cpp \
    layers/movelayerundocommand.cpp \
    tools/mytoolbase.cpp \
    tools/MoveTool/movetool.cpp \
    tools/SelectTool/selecttool.cpp \
    utils/myunitslabel.cpp \
    tools/ZoomTool/zoomtool.cpp \
    miniview.cpp \
    tools/MoveTool/movetoolcore.cpp \
    tools/MeasureTool/measuretool.cpp \
    items/myitembase.cpp \
    toolswidget/flowlayout.cpp \
    toolswidget/mytoolbox.cpp \
    tools/TextTool/texttool.cpp \
    items/textitem/textitem.cpp \
    tools/TextTool/addtextundocommand.cpp \
    items/groundplaneeffect.cpp \
    tools/SelectTool/selectiontreewidget.cpp \
    tools/SelectTool/typonitemtreeitem.cpp \
    tools/TextTool/edittextundocommand.cpp \
    layers/layerstableitem.cpp \
    items/paditem/holeitem.cpp \
    items/paditem/padgroup.cpp \
    items/paditem/paditem.cpp \
    tools/PadTool/addpadundocommand.cpp \
    tools/PadTool/padtool.cpp \
    layers/layerswidget.cpp \
    tools/TrackTool/tracktool.cpp \
    tools/DrawTool/drawtool.cpp \
    tools/PadTool/editpadundocommand.cpp \
    tools/GroupTool/grouptool.cpp \
    tools/GroupTool/editgroupundocommand.cpp \
    tools/GroupTool/groupundocommand.cpp \
    tools/GroupTool/ungroupundocommand.cpp \
    angledialog.cpp \
    tools/SelectTool/rotateundocommand.cpp \
    tools/SelectTool/mirrorundocommand.cpp \
    layers/layeraction.cpp \
    layers/layersmenu.cpp \
    tools/SelectTool/changelayerundocommand.cpp \
    tools/SelectTool/alignongridundocommand.cpp \
    tools/TextTool/deletetextundocommand.cpp \
    tools/SelectTool/moveundocommand.cpp \
    tools/PadTool/deletepadundocommand.cpp \
    items/trackitem/angle.cpp \
    items/trackitem/track.cpp \
    tools/TrackTool/addtrackundocommand.cpp \
    tools/TrackTool/edittrackundocommand.cpp \
    tools/TrackTool/deletetrackundocommand.cpp \
    tools/SelectTool/removeangleundocommand.cpp \
    tools/SelectTool/alignangleongridundocommand.cpp \
    tools/SelectTool/splittrackundocommand.cpp \
    tools/SelectTool/mergetrackundocommand.cpp \
    tools/SelectTool/moveangleundocommand.cpp \
    tools/SelectTool/insertangleundocommand.cpp \
    items/drawitem.cpp \
    tools/DrawTool/adddrawitemundocommand.cpp \
    tools/DrawTool/deletedrawundocommand.cpp \
    tools/DrawTool/editdrawundocommand.cpp \
    tools/ComponentTool/componenttool.cpp \
    librarieswidget.cpp \
    tools/ComponentTool/componentstreewidget.cpp \
    tools/ComponentTool/componentstreewidgetitem.cpp \
    tools/ComponentTool/componentslistwidgetitem.cpp \
    recentfileaction.cpp \
    layersoptionwidget.cpp

HEADERS  += mainwindow.h \
    workspace/mytypongrid.h \
    workspace/myworkspace.h \
    utils/mycolorbutton.h \
    utils/mytyponunits.h \
    utils/trackcurrentcalculator.h \
    myoptions.h \
    optionsdialog.h \
    mydockwidget.h \
    mytabwidget.h \
    utils/myunitsspinbox.h \
    utils/myunitscombobox.h \
    generaloptionswidget.h \
    myapplication.h \
    global.h \
    typonWidget/mytypon.h \
    typonWidget/mytyponscene.h \
    typonWidget/mytyponscrollbar.h \
    typonWidget/mytyponview.h \
    typonWidget/ruler/mytyponruler.h \
    layers/addlayerundocommand.h \
    layers/editlayerundocommand.h \
    layers/layer.h \
    layers/layeroptionsdialog.h \
    layers/layerswidget.h \
    layers/removelayerundocommand.h \
    gridwidget.h \
    utils/myunitsmenulabel.h \
    typonoptionswidget.h \
    layers/layersstack.h \
    undowidget.h \
    layers/movelayerundocommand.h \
    tools/mytoolbase.h \
    tools/MoveTool/movetool.h \
    tools/SelectTool/selecttool.h \
    utils/myunitslabel.h \
    tools/ZoomTool/zoomtool.h \
    miniview.h \
    tools/MoveTool/movetoolcore.h \
    tools/MeasureTool/measuretool.h \
    items/myitembase.h \
    toolswidget/flowlayout.h \
    toolswidget/mytoolbox.h \
    tools/TextTool/texttool.h \
    items/textitem/textitem.h \
    tools/TextTool/addtextundocommand.h \
    items/groundplaneeffect.h \
    tools/SelectTool/selectiontreewidget.h \
    tools/SelectTool/typonitemtreeitem.h \
    tools/TextTool/edittextundocommand.h \
    layers/layerstableitem.h \
    items/paditem/holeitem.h \
    items/paditem/padgroup.h \
    items/paditem/paditem.h \
    tools/PadTool/addpadundocommand.h \
    tools/PadTool/padtool.h \
    layers/layerstableitem.h \
    tools/TrackTool/tracktool.h \
    tools/DrawTool/drawtool.h \
    tools/PadTool/editpadundocommand.h \
    tools/GroupTool/grouptool.h \
    tools/GroupTool/editgroupundocommand.h \
    tools/GroupTool/groupundocommand.h \
    tools/GroupTool/ungroupundocommand.h \
    angledialog.h \
    tools/SelectTool/rotateundocommand.h \
    tools/SelectTool/mirrorundocommand.h \
    layers/layeraction.h \
    layers/layersmenu.h \
    tools/SelectTool/changelayerundocommand.h \
    tools/SelectTool/alignongridundocommand.h \
    tools/TextTool/deletetextundocommand.h \
    tools/SelectTool/moveundocommand.h \
    tools/PadTool/deletepadundocommand.h \
    items/trackitem/angle.h \
    items/trackitem/track.h \
    tools/TrackTool/addtrackundocommand.h \
    tools/TrackTool/edittrackundocommand.h \
    tools/TrackTool/deletetrackundocommand.h \
    tools/SelectTool/removeangleundocommand.h \
    tools/SelectTool/alignangleongridundocommand.h \
    tools/SelectTool/splittrackundocommand.h \
    tools/SelectTool/mergetrackundocommand.h \
    tools/SelectTool/moveangleundocommand.h \
    tools/SelectTool/insertangleundocommand.h \
    items/drawitem.h \
    tools/DrawTool/adddrawitemundocommand.h \
    tools/DrawTool/deletedrawundocommand.h \
    tools/DrawTool/editdrawundocommand.h \
    tools/ComponentTool/componenttool.h \
    librarieswidget.h \
    tools/ComponentTool/componentstreewidget.h \
    tools/ComponentTool/componentstreewidgetitem.h \
    tools/ComponentTool/componentslistwidgetitem.h \
    recentfileaction.h \
    layersoptionwidget.h

FORMS    += mainwindow.ui \
    optionsdialog.ui \
    generaloptionswidget.ui \
    typonWidget/mytypon.ui \
    layers/layeroptionsdialog.ui \
    layers/layerswidget.ui \
    gridwidget.ui \
    typonoptionswidget.ui \
    undowidget.ui \
    tools/mytoolbase.ui \
    tools/MoveTool/movetool.ui \
    tools/SelectTool/selecttool.ui \
    tools/ZoomTool/zoomtool.ui \
    tools/MeasureTool/measuretool.ui \
    toolswidget/mytoolbox.ui \
    tools/TextTool/texttool.ui \
    tools/PadTool/padtool.ui \
    tools/TrackTool/tracktool.ui \
    tools/DrawTool/drawtool.ui \
    tools/GroupTool/grouptool.ui \
    angledialog.ui \
    tools/ComponentTool/componenttool.ui \
    librarieswidget.ui \
    layersoptionwidget.ui

RESOURCES += \
    mytypon.qrc

DISTFILES += \
    todo.txt \
    algosdivers.txt

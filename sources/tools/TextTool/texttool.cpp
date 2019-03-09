#include "texttool.h"
#include "ui_texttool.h"
#include "global.h"
#include "addtextundocommand.h"
#include "edittextundocommand.h"
#include <QDebug>

TextTool::TextTool(QWidget *parent, TextItem *item)
    : MYToolBase(parent),ui(new Ui::TextTool){
    ui->setupUi(this);
    setToolName(tr("Text"));
    setToolIcon(QIcon(":/pictures/insert_text.png"));
    m_cursor = Qt::ArrowCursor;

    // fill the table with widgets (hidden in page2 of stacked widget)
    ui->tableWidget->setCellWidget(0,1,ui->nameLineEdit);
    ui->tableWidget->setCellWidget(1,1,ui->heightSpinBox);
    ui->tableWidget->setCellWidget(2,1,ui->fontComboBox);
    ui->tableWidget->setCellWidget(3,1,ui->boldCheckBox);
    ui->tableWidget->setCellWidget(4,1,ui->italicCheckBox);
    ui->tableWidget->setCellWidget(5,1,ui->underlineCheckBox);
    ui->tableWidget->setCellWidget(6,1,ui->lineEdit);

    // if no item is passed in the constructor then it is a tool item
    if ( item == 0 ){
        // set default values
        ui->heightSpinBox->setRawValue(150);
        m_font = ui->fontComboBox->font();
        m_textItem = new TextItem(0);
        m_textItem->setToolItem(true);
        m_textItem->setFont(ui->fontComboBox->currentFont());
        m_textItem->setWeight(ui->heightSpinBox->rawValue());
        // as it is a tool item hide name widget, we dont want to edit this now
        ui->tableWidget->setRowHidden(0,true);
        connect(ui->fontComboBox,SIGNAL(currentFontChanged(QFont)),this,SLOT(onCurrentFontChanged(QFont)));
        connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(onTextChanged()));
        connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
        connect(ui->heightSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWeight()));
        connect(ui->boldCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_boldCheckBox_toggled(bool)));
        connect(ui->italicCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_italicCheckBox_toggled(bool)));
        connect(ui->underlineCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_underlineCheckBox_toggled(bool)));
        ui->lineEdit->setText(tr("Text"));
    }else{
        // item is an existing one
        m_textItem = item;
        // update widget values
        updateTool();
    }

    // set a pointer to this on the item so we can update widgets values when item properties change
    m_textItem->setTool(this);
}

TextTool::~TextTool(){
    delete ui;
    m_textItem->setTool(0);
    // if m_textItem isToolItem, the tool is the application one, so we are quitting app, so delete item
    // else tool is one of those used in selection treeWidget, do not delete linked textItem
    if ( m_textItem->isToolItem() && ! m_textItem->parentItem() )
        delete m_textItem;
}

// udpdate widget values with texItem values
// disconnect and reconnect signals to avoid looping
void TextTool::updateTool(){
    disconnect(ui->fontComboBox,SIGNAL(currentFontChanged(QFont)),this,SLOT(onCurrentFontChanged(QFont)));
    disconnect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(onTextChanged()));
    disconnect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
    disconnect(ui->heightSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWeight()));
    disconnect(ui->boldCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_boldCheckBox_toggled(bool)));
    disconnect(ui->italicCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_italicCheckBox_toggled(bool)));
    disconnect(ui->underlineCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_underlineCheckBox_toggled(bool)));

    if ( ui->heightSpinBox->rawValue() != m_textItem->width() )
        ui->heightSpinBox->setRawValue(m_textItem->width());
    m_font = m_textItem->font();
    ui->boldCheckBox->setChecked(m_font.bold());
    ui->italicCheckBox->setChecked(m_font.italic());
    ui->underlineCheckBox->setChecked(m_font.underline());
    ui->lineEdit->setText(m_textItem->text());
    ui->fontComboBox->setCurrentFont(m_font);
    ui->nameLineEdit->setText(m_textItem->itemName());
    emit nameChanged(m_textItem);

    connect(ui->fontComboBox,SIGNAL(currentFontChanged(QFont)),this,SLOT(onCurrentFontChanged(QFont)));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(onTextChanged()));
    connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
    connect(ui->heightSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWeight()));
    connect(ui->boldCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_boldCheckBox_toggled(bool)));
    connect(ui->italicCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_italicCheckBox_toggled(bool)));
    connect(ui->underlineCheckBox,SIGNAL(toggled(bool)),this,SLOT(on_underlineCheckBox_toggled(bool)));
}

void TextTool::handleMousePressEvent(QMouseEvent *event){
    // if tool is enable and there is a valid text, add a new item to scene
    if ( isEnabled() && ! ui->lineEdit->text().isEmpty() ){
        if ( event->button() == Qt::LeftButton ){
            m_textItem->setPos(qApp->currentTypon()->typonView()->nearestGridPoint(
                              qApp->currentTypon()->typonView()->mapToScene(event->pos())));
            AddTextUndoCommand *addTextCommand = new AddTextUndoCommand(m_textItem);
            qApp->undoGroup()->activeStack()->push(addTextCommand);
        }
    }
    MYToolBase::mousePressEventTool(event);
}

void TextTool::handleMouseReleaseEvent(QMouseEvent *event){
    MYToolBase::mouseReleaseEventTool(event);
}

// move the tool item under mouse cursor
void TextTool::handleMouseMoveEvent(QMouseEvent *event){
    if ( isEnabled() ){
        m_textItem->setPos(qApp->currentTypon()->typonView()->nearestGridPoint(
                          qApp->currentTypon()->typonView()->mapToScene(event->pos())));
    }
    MYToolBase::mouseMoveEventTool(event);
}

// update pos of tool item when wheel event occure
void TextTool::handleWheelEvent(QWheelEvent *event){    
    MYToolBase::mouseWheelEventTool(event);
    if ( isEnabled() ){
        m_textItem->setPos(qApp->currentTypon()->typonView()->nearestGridPoint(
                          qApp->currentTypon()->typonView()->mapToScene(event->pos())));
    }
}

void TextTool::handleKeyPressEvent(QKeyEvent *event){
    // if tool is deisable, do nothing
    if ( isEnabled() ){
        // update width if w key is pressed
        if (event->key() == Qt::Key_W ) {
            if ( event->modifiers().testFlag(Qt::ShiftModifier) )
                ui->heightSpinBox->stepUp();
            else
                ui->heightSpinBox->stepDown();
        }
        // rotate by 45° on r key
        if (event->key() == Qt::Key_R ) {
            if ( event->modifiers().testFlag(Qt::ShiftModifier) )
                m_textItem->rotate(45,m_textItem->sceneBoundingRect().center());
            else
                m_textItem->rotate(-45,m_textItem->sceneBoundingRect().center());
        }
        // mirror on M key
        if (event->key() == Qt::Key_M ) {
            if ( event->modifiers().testFlag(Qt::ShiftModifier) )
                m_textItem->verticalMirror(m_textItem->sceneBoundingRect().center());
            else
                m_textItem->horizontalMirror(m_textItem->sceneBoundingRect().center());
        }
    }
    MYToolBase::keyPressEventTool(event);
}

void TextTool::handleKeyReleaseEvent(QKeyEvent *event){
    MYToolBase::keyReleaseEventTool(event);
}

// if mouse enter the viewport, add the tool item under mouse in the scene
void TextTool::handleEnterEvent(QEvent *event){
    if ( isEnabled() && ! ui->lineEdit->text().isEmpty() )
        m_textItem->setParentItem(qApp->currentTypon()->typonScene()->layers()->activeLayer());
    MYToolBase::enterEventTool(event);
}

// if we leave viewport, remove tool item from the scene
void TextTool::handleLeaveEvent(QEvent *event){
    if ( isEnabled() ){
        if ( m_textItem->parentItem() )
            qApp->currentTypon()->typonScene()->removeTyponItem(m_textItem);
    }
    MYToolBase::leaveEventTool(event);
}

// change bold value, if tool item do not create undo command
void TextTool::on_boldCheckBox_toggled(bool checked){
    if ( m_textItem->isToolItem() ){
        m_textItem->setBold(checked);
    }else{
        QFont font = m_font;
        font.setBold(checked);
        EditTextUndoCommand *textEditCommand = new EditTextUndoCommand(m_textItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->lineEdit->text(),
                                                                       font,
                                                                       ui->heightSpinBox->rawValue());
        qApp->undoGroup()->activeStack()->push(textEditCommand);
    }
}

// change italic value, if tool item do not create undo command
void TextTool::on_italicCheckBox_toggled(bool checked){   
    if ( m_textItem->isToolItem() ){
        m_textItem->setItalic(checked);
    }else{
        QFont font = m_font;
        font.setItalic(checked);
        EditTextUndoCommand *textEditCommand = new EditTextUndoCommand(m_textItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->lineEdit->text(),
                                                                       font,
                                                                       ui->heightSpinBox->rawValue());
        qApp->undoGroup()->activeStack()->push(textEditCommand);
    }
}

// change underline value, if tool item do not create undo command
void TextTool::on_underlineCheckBox_toggled(bool checked){
    if ( m_textItem->isToolItem() ){
        m_textItem->setUnderlined(checked);
    }else{
        QFont font = m_font;
        font.setUnderline(checked);
        EditTextUndoCommand *textEditCommand = new EditTextUndoCommand(m_textItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->lineEdit->text(),
                                                                       font,
                                                                       ui->heightSpinBox->rawValue());
        qApp->undoGroup()->activeStack()->push(textEditCommand);
    }
}

// change font, if tool item do not create undo command
void TextTool::onCurrentFontChanged(QFont font){    
    if ( m_textItem->isToolItem() ){
        m_font = font;
        m_font.setBold(ui->boldCheckBox->isChecked());
        m_font.setItalic(ui->italicCheckBox->isChecked());
        m_font.setUnderline(ui->underlineCheckBox->isChecked());
        m_textItem->setFont(m_font);
        m_textItem->setWeight(ui->heightSpinBox->rawValue());
    }else{
        EditTextUndoCommand *textEditCommand = new EditTextUndoCommand(m_textItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->lineEdit->text(),
                                                                       ui->fontComboBox->currentFont(),
                                                                       ui->heightSpinBox->rawValue());
        qApp->undoGroup()->activeStack()->push(textEditCommand);
    }
}

// change text value, if tool item do not create undo command
void TextTool::onTextChanged(){
    if ( m_textItem->isToolItem() ){
        m_textItem->setText(ui->lineEdit->text());
    }else{
        // cancel if text is empty (no empty text item: could not select it as they wouldn't have any shape)
        if ( ui->lineEdit->text().isEmpty() )
            return;
        EditTextUndoCommand *textEditCommand = new EditTextUndoCommand(m_textItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->lineEdit->text(),
                                                                       ui->fontComboBox->currentFont(),
                                                                       ui->heightSpinBox->rawValue());
        qApp->undoGroup()->activeStack()->push(textEditCommand);
    }
}

// change weight (width) value, if tool item do not create undo command
void TextTool::updateWeight(){   
    if ( m_textItem->isToolItem() ){
        m_textItem->setWeight(ui->heightSpinBox->rawValue());
    }else{
        EditTextUndoCommand *textEditCommand = new EditTextUndoCommand(m_textItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->lineEdit->text(),
                                                                       ui->fontComboBox->currentFont(),
                                                                       ui->heightSpinBox->rawValue());
        qApp->undoGroup()->activeStack()->push(textEditCommand);
    }
}

// change name value, can only occured on an existing item on the scene
void TextTool::onNameChanged(){
    EditTextUndoCommand *textEditCommand = new EditTextUndoCommand(m_textItem,
                                                                   ui->nameLineEdit->text(),
                                                                   ui->lineEdit->text(),
                                                                   ui->fontComboBox->currentFont(),
                                                                   ui->heightSpinBox->rawValue());
    qApp->undoGroup()->activeStack()->push(textEditCommand);
    emit nameChanged(m_textItem);
}

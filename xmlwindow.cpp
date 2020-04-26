#include "xmlwindow.h"
#include "ui_xmlwindow.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSplitter>
#include <QFileDialog>
#include <QSaveFile>
#include <QTextStream>
#include <QMenu>

XMLWindow::XMLWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XMLWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
}

XMLWindow::~XMLWindow()
{
    delete ui;
}

bool XMLWindow::write(QIODevice *device) const
{
    const int IndentSize = 4;

    QTextStream out(device);
    model->getDomDocument().save(out, IndentSize);
    //domDocument.save(out, IndentSize);
    return true;
}

bool XMLWindow::loadFile(QIODevice *device)
{
    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!domDocument.setContent(device, true, &errorStr, &errorLine,
                                &errorColumn)) {
        QMessageBox::information(window(), tr("XML Editor"),
                                 tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return false;
    }

    model = new TreeModel(domDocument);

    ui->treeView->setModel(model);

    return true;
}

void XMLWindow::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
}

bool XMLWindow::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool XMLWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool XMLWindow::saveFile(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("XML editor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    if (!write(&file)) {
        QMessageBox::warning(this, tr("XML editor"),
                             tr("Cannot read file %1:\nUnknown XML ecoding error.")
                             .arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    return true;
}

QString XMLWindow::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void XMLWindow::resizeEvent(QResizeEvent *event)
{
    QRect rec(0, 0, event->size().width(), event->size().height());
    ui->horizontalLayoutWidget->setGeometry(0,0,event->size().width(),event->size().height());
    ui->treeView->setGeometry(0,0,event->size().width(),event->size().height());
}

void XMLWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);
    isUntitled = false;
    QString shownName = curFile;
    if (curFile.isEmpty())
    {
        shownName = "untitled.xml";
        isUntitled = true;
    }
    setWindowFilePath(shownName);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

void XMLWindow::refresh()
{

}

void XMLWindow::slotCustomMenuRequested(QPoint pos)
{
    /* Создаем объект контекстного меню */
    QMenu * menu = new QMenu(this);
    /* Создаём действия для контекстного меню */

    QMenu* submenuElement = menu->addMenu(tr("Element"));
    QMenu* submenuAttribute = menu->addMenu(tr("Attribute"));
    QMenu* submenuText = menu->addMenu(tr("Text"));
    QMenu* submenuComment = menu->addMenu(tr("Comment"));

    QAction* actionElementBefore = submenuElement->addAction(tr("Before"));
    QAction* actionElementAfter = submenuElement->addAction(tr("After"));
    QAction* actionElementChild = submenuElement->addAction(tr("Child"));

    QAction* actionAttributeBefore = submenuAttribute->addAction(tr("Before"));
    QAction* actionAttributeAfter = submenuAttribute->addAction(tr("After"));
    QAction* actionAttributeChild = submenuAttribute->addAction(tr("Child"));

    QAction* actionTextBefore = submenuText->addAction(tr("Before"));
    QAction* actionTextAfter = submenuText->addAction(tr("After"));
    QAction* actionTextChild = submenuText->addAction(tr("Child"));

    QAction* actionCommentBefore = submenuComment->addAction(tr("Before"));
    QAction* actionCommentAfter = submenuComment->addAction(tr("After"));
    QAction* actionCommentChild = submenuComment->addAction(tr("Child"));

    QAction * deleteElement = new QAction(tr("Delete"), this);
    QAction * createElement = new QAction(tr("Create Element"),this);
    QAction * createAttr = new QAction(tr("Create Attribute"),this);
    QAction * createText = new QAction(tr("Create Text"),this);
    QAction * createComment = new QAction(tr("Create Comment"),this);

    /* Подключаем СЛОТы обработчики для действий контекстного меню */
    connect(deleteElement, SIGNAL(triggered()), this, SLOT(slotRemoveRecord())); // Обработчик удаления записи
    connect(createElement, SIGNAL(triggered()), this, SLOT(slotCreateElement()));

    connect(actionElementBefore, SIGNAL(triggered()),this, SLOT(slotCreateElementBefore()));
    connect(actionElementAfter, SIGNAL(triggered()),this, SLOT(slotCreateElementAfter()));
    connect(actionElementChild, SIGNAL(triggered()),this, SLOT(slotCreateElementChild()));

    connect(actionAttributeBefore, SIGNAL(triggered()),this, SLOT(slotCreateAttributeBefore()));
    connect(actionAttributeAfter, SIGNAL(triggered()),this, SLOT(slotCreateAttributeAfter()));
    connect(actionAttributeChild, SIGNAL(triggered()),this, SLOT(slotCreateAttributeChild()));

    connect(actionTextBefore, SIGNAL(triggered()),this, SLOT(slotCreateTextBefore()));
    connect(actionTextAfter, SIGNAL(triggered()),this, SLOT(slotCreateTextAfter()));
    connect(actionTextChild, SIGNAL(triggered()),this, SLOT(slotCreateTextChild()));


    connect(actionCommentBefore, SIGNAL(triggered()),this, SLOT(slotCreateCommentBefore()));
    connect(actionCommentAfter, SIGNAL(triggered()),this, SLOT(slotCreateCommentAfter()));
    connect(actionCommentChild, SIGNAL(triggered()),this, SLOT(slotCreateCommentChild()));

    /* Устанавливаем действия в меню */
    menu->addAction(deleteElement);
    menu->addAction(createElement);
    menu->addAction(createAttr);
    menu->addAction(createText);
    menu->addAction(createComment);

    /* Вызываем контекстное меню */
    menu->popup(ui->treeView->viewport()->mapToGlobal(pos));
}

void XMLWindow::slotRemoveRecord()
{
    /* Выясняем, какая из строк была выбрана
         * */
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    /* Проверяем, что строка была действительно выбрана
         * */
    if(index.row() >= 0){
        /* Задаём вопрос, стоит ли действительно удалять запись.
             * При положительном ответе удаляем запись
             * */
        if (QMessageBox::warning(this,
                                 tr("Delete record"),
                                 tr("Are you sure you want to delete this entry?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            return;
        } else {
            /* В противном случае производим удаление записи.
                 * При успешном удалении обновляем таблицу.
                 * */
            if(!model->removeRow(index.row(),index.parent())){
                QMessageBox::warning(this,tr("Notification"),
                                     tr("Failed to delete entry.\n"));
            }
            ui->treeView->setCurrentIndex(model->index(-1, -1));

        }
    }
}

void XMLWindow::slotCreateElement()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    if(index.row() >= 0){
        /* В противном случае производим удаление записи.
            * При успешном удалении обновляем таблицу.
            * */
        if(!model->insertRow(index.row(),index.parent())){
            QMessageBox::warning(this,tr("Notification"),
                                 tr("Failed to insert entry.\n"));
        }
        ui->treeView->setCurrentIndex(model->index(-1, -1));
    }
}

void XMLWindow::slotCreateElementBefore()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    if(index.row() >= 0){
        /*if(!model->insertRow(index.row(),index.parent())){
            QMessageBox::warning(this,tr("Notification"),
                                 tr("Failed to insert entry.\n"));
        }*/
        model->createElement(index,TreeModel::Before);
    }
    ui->treeView->setCurrentIndex(index.siblingAtRow(index.row()-1));
}

void XMLWindow::slotCreateElementAfter()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    if(index.row() >= 0){
        model->createElement(index,TreeModel::After);
    }
    ui->treeView->setCurrentIndex(index.siblingAtRow(index.row()+1));
}

void XMLWindow::slotCreateElementChild()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    if(index.row() >= 0){
        model->createElement(index,TreeModel::Child);
    }
    ui->treeView->setCurrentIndex(index.child(0,0));
}

void XMLWindow::slotCreateAttributeBefore()
{

}

void XMLWindow::slotCreateAttributeAfter()
{

}

void XMLWindow::slotCreateAttributeChild()
{

}

void XMLWindow::slotCreateTextBefore()
{

}

void XMLWindow::slotCreateTextAfter()
{

}

void XMLWindow::slotCreateTextChild()
{

}

void XMLWindow::slotCreateCommentBefore()
{

}

void XMLWindow::slotCreateCommentAfter()
{

}

void XMLWindow::slotCreateCommentChild()
{

}


QString XMLWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

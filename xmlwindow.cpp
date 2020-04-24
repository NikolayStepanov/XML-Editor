#include "xmlwindow.h"
#include "ui_xmlwindow.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSplitter>
#include <QFileDialog>
#include <QSaveFile>
#include <QTextStream>

XMLWindow::XMLWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XMLWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
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

QString XMLWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

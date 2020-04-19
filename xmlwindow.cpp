#include "xmlwindow.h"
#include "ui_xmlwindow.h"

#include <QFileInfo>

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

bool XMLWindow::loadFile(const QString &fileName)
{

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

}

QString XMLWindow::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

QString XMLWindow::strippedName(const QString &fullFileName)
{
     return QFileInfo(fullFileName).fileName();
}

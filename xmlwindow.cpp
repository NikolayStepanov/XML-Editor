#include "xmlwindow.h"
#include "ui_xmlwindow.h"

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

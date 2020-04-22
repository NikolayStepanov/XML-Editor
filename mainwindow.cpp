#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QtWidgets>

#include <QSettings>
#include <QCloseEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <xmlwindow.h>


class XMLWindow;
namespace
{
QString strippedName(const QString &rcFullFileName)
{
    return QFileInfo(rcFullFileName).fileName();
}
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //MDIArea
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(ui->mdiArea);

    connect(ui->mdiArea, &QMdiArea::subWindowActivated,this, &MainWindow::updateMenus);

    createActions();
    updateMenus();
    readSettings();
    updateRecentFileActions();

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::openFile(const QString &fileName)
{
    if (QMdiSubWindow *existing = findXMLWindow(fileName)) {
        ui->mdiArea->setActiveSubWindow(existing);
        return true;
    }
    const bool succeeded = loadFile(fileName);
    if (succeeded)
        statusBar()->showMessage(tr("File loaded"), 2000);
    return succeeded;
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    m_listRecentFiles = readRecentFiles(settings);

    QMutableStringListIterator i(m_listRecentFiles);
    while (i.hasNext())
        if (!QFile::exists(i.next()))
            i.remove();
    for (int j = 0; j < enMaxRecentFiles; ++ j)
        if (j < m_listRecentFiles.count())
        {
            QString text = QString("&%1 %2")
                    .arg(j + 1)
                    .arg(strippedName(m_listRecentFiles[j]));
            m_apActionsRecent[j]->setText(text);
            m_apActionsRecent[j]->setData(m_listRecentFiles[j]);
            m_apActionsRecent[j]->setVisible(true);
        }
        else
            m_apActionsRecent[j]->setVisible(false);
}

void MainWindow::updateWindowMenu()
{
    if(!vecMenuWindows.isEmpty())
    {
        for(auto val : vecMenuWindows)
        {
            ui->menuWindows->removeAction(val);
        }
    }

    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i)
    {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        XMLWindow *child = qobject_cast<XMLWindow *>(mdiSubWindow->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                    .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                    .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = ui->menuWindows->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
            ui->mdiArea->setActiveSubWindow(mdiSubWindow);
        });
        vecMenuWindows.push_back(action);
        action->setCheckable(true);
        action ->setChecked(child == activeXMLWindow());
    }
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

void MainWindow::updateMenus()
{
    bool hasXMLWindow = (activeXMLWindow() != nullptr);
    ui->actionSave->setEnabled(hasXMLWindow);
    ui->actionTile->setEnabled(hasXMLWindow);
    ui->actionCascade->setEnabled(hasXMLWindow);
    ui->action_Close_all_windows->setEnabled(hasXMLWindow);
}

XMLWindow *MainWindow::activeXMLWindows() const
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<XMLWindow*>(activeSubWindow->widget());
    return nullptr;
}

XMLWindow *MainWindow::createXMLWindows()
{
    XMLWindow *child = new XMLWindow(this);
    ui->mdiArea->addSubWindow(child);

    return child;
}

void MainWindow::createActions()
{
    //QAction
    for (int i = 0; i < enMaxRecentFiles; ++ i)
    {
        m_apActionsRecent[i]=this->findChild<QAction*>("actionFileOpenRecent" + QString::number(i+1));
        m_apActionsRecent[i]->setVisible(false);

        connect(m_apActionsRecent[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
    }

    updateWindowMenu();

    connect(ui->menuWindows, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);
    connect(ui->menuFile, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

QMdiSubWindow *MainWindow::findXMLWindow(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    const QList<QMdiSubWindow *> subWindows = ui->mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        XMLWindow *mdiChild = qobject_cast<XMLWindow *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return nullptr;
}

bool MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("XML editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    XMLWindow *child = createXMLWindows();
    const bool succeeded = child->loadFile(&file);
    child->setCurrentFile(fileName);
    if (succeeded)
        child->show();
    else
        child->close();
    MainWindow::prependToRecentFiles(fileName);
    return succeeded;
}

XMLWindow *MainWindow::activeXMLWindow() const
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<XMLWindow *>(activeSubWindow->widget());
    return nullptr;
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("recentFiles", m_listRecentFiles);
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Application"),
                       tr("The editor of the XML file, presented in the form of a tree of nodes and attributes. "
                          "You must use the Qt classes QDomDocument, QDomNode, and so on."
                          "At a minimum, support is required for nodes of the DocumentTypeNode, ElementNode, "
                          "and AttributeNode types, integer and string attributes. In the model, attributes must"
                          "to be children in relation to their elements."));
}

void MainWindow::on_actionOpen_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

void MainWindow::on_actionNew_triggered()
{
    XMLWindow *child = createXMLWindows();
    child->newFile();
    child->show();
}

void MainWindow::on_actionSave_triggered()
{
    if (activeXMLWindow() && activeXMLWindow()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();

    ui->mdiArea->closeAllSubWindows();
    if (ui->mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}


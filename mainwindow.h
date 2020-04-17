#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>

class XMLWindow;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool openFile(const QString &fileName);

protected:
    virtual void closeEvent(QCloseEvent *pEvent);

private slots:
    void updateRecentFileActions();
    void updateWindowMenu();
    void openRecentFile();
    XMLWindow *createXMLWindows();

    void on_actionAbout_triggered();

private:

    enum
    {enMaxRecentFiles = 5};
    void createActions();
    void prependToRecentFiles(const QString &fileName);
    QMdiSubWindow *findXMLWindow(const QString &fileName) const;
    bool loadFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    QAction *m_apActionsRecent[enMaxRecentFiles];
    QStringList m_listRecentFiles;
};
#endif // MAINWINDOW_H

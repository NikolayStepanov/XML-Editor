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
    void closeEvent(QCloseEvent *event);

private slots:
    void updateRecentFileActions();
    void updateWindowMenu();
    void openRecentFile();

    XMLWindow *createXMLWindows();

    void on_actionAbout_triggered();

    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_triggered();

private:

    enum
    {enMaxRecentFiles = 5};
    void createActions();
    void prependToRecentFiles(const QString &fileName);
    static bool hasRecentFiles();
    QMdiSubWindow *findXMLWindow(const QString &fileName) const;
    bool loadFile(const QString &fileName);
    XMLWindow *activeXMLWindow() const;

    void readSettings();
    void writeSettings();


private:
    Ui::MainWindow *ui;
    QAction *m_apActionsRecent[enMaxRecentFiles];
    QVector<QAction*>vecMenuWindows;
    QStringList m_listRecentFiles;
};
#endif // MAINWINDOW_H

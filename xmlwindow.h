#ifndef XMLWINDOW_H
#define XMLWINDOW_H

#include <QWidget>

namespace Ui {
class XMLWindow;
}

class XMLWindow : public QWidget
{
    Q_OBJECT

public:
    explicit XMLWindow(QWidget *parent = nullptr);
    ~XMLWindow();

    QString currentFile() { return curFile; }
    bool loadFile(const QString &fileName);
    void newFile();
    bool save();
    QString userFriendlyCurrentFile();

private:
    QString strippedName(const QString &fullFileName);

private:
    Ui::XMLWindow *ui;

    QString curFile;
    bool isUntitled;
};

#endif // XMLWINDOW_H

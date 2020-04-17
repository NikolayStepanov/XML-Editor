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

private:
    Ui::XMLWindow *ui;

    QString curFile;
};

#endif // XMLWINDOW_H

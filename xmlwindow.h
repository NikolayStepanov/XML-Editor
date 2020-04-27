#ifndef XMLWINDOW_H
#define XMLWINDOW_H

#include <QWidget>
#include <QDomDocument>
#include <treemodel.h>

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

    bool write(QIODevice *device) const;

    bool loadFile(QIODevice *device);
    void newFile();
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    void resizeEvent(QResizeEvent *event) override;

    void setCurrentFile(const QString &fileName);
    void refresh();

private slots:
    void slotCustomMenuRequested(QPoint pos);
    void slotRemoveRecord();

    void slotCreateElementBefore();
    void slotCreateElementAfter();
    void slotCreateElementChild();

    void slotCreateAttributeBefore();
    void slotCreateAttributeAfter();
    void slotCreateAttributeChild();

    void slotCreateTextBefore();
    void slotCreateTextAfter();
    void slotCreateTextChild();

    void slotCreateCommentBefore();
    void slotCreateCommentAfter();
    void slotCreateCommentChild();

private:
    QString strippedName(const QString &fullFileName);


private:
    Ui::XMLWindow *ui;
    QString curFile;
    bool isUntitled;
    QDomDocument domDocument;
    TreeModel* model;
};

#endif // XMLWINDOW_H

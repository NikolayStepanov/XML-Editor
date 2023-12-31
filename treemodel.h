#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNamedNodeMap>

#include <treeitem.h>

class TreeModel: public QAbstractItemModel
{
    Q_OBJECT
public:

    enum wayInsertItem {Before=1,After,Child};

    TreeModel(const QDomDocument &doc, QObject *parent = 0);
    ~TreeModel();
    QVariant data(const QModelIndex &index, int role) const override;
    //получить данные из модельного индекса index с ролью role

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    //получить флаги выбора

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    //получить данные заголовка

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    //получить модельный индекс по строке и столбцу

    QModelIndex parent(const QModelIndex &index) const override;
    //получить модельный индекс родителя

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    //получить количество строк и столбцов для элемента с заданным модельным индексом

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    //установить данные узла с индексом index в значение value

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole) override;
    //установить данные заголовка столбца

    bool insertColumns(int position, int columns,const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    //вставка и удаление столбцов и строк

    bool createElement(const QModelIndex &index);
    bool createElement(const QModelIndex &index,wayInsertItem);
    bool createAttribute(const QModelIndex &index,wayInsertItem);

    void traverseXmlNode(const QDomNode& node, TreeItem* parent);
    QDomDocument getDomDocument(){return domDocument;};
private:

    void setupModelData(const QDomDocument &domDocument, TreeItem *parent);
    //внутренний метод для установки данных модели

    TreeItem *getItem(const QModelIndex &index) const;
    //внутренний метод для получения элемента

    TreeItem *rootItem; //ссылка на корневой узел

    QDomDocument domDocument;

    int tabCount; // level
};

#endif // TREEMODEL_H

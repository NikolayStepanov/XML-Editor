#ifndef TREEITEM_H
#define TREEITEM_H
#include <QList>
#include <QVector>
#include <QVariant>
#include <QDomNode>

class TreeItem
{
public:
    explicit TreeItem (const QVector<QVariant> &data, QDomNode &node, TreeItem *parentItem = nullptr); //Конструктор узла дерева
    ~TreeItem();

    void appendChild(TreeItem *child); //Добавить узел-потомок
    void insertChild(int index, TreeItem* child);
    TreeItem *child(int row); //Вернуть дочерний элемент
    int childCount() const; //Количество дочерних элементов
    int columnCount() const; //Вернуть количество столбцов элемента
    QVariant data(int column) const; //Вернуть данные указанного столбца
    int childNumber() const; //Вернуть номер строки элемента
    TreeItem *parentItem(); //Вернуть родительский элемент
    bool insertChildren(int position, int count, int columns); //Вставить потомков (строки)
    bool insertColumns(int position, int columns); //Вставить столбцы
    bool removeChildren(int position, int count);  //Удалить потомков
    bool setData(int column, const QVariant &value); //Установить данные
    QDomNode& getDomNode();
    void setDomNode(QDomNode &node);

private:
    bool setNameNode(const QVariant &value);
    bool setValueNode(const QVariant &value);

private:
    QList <TreeItem*> m_childItems; //Список дочерних элементов
    QVector <QVariant> m_itemData; //Список данных текущего узла
    TreeItem *m_parentItem; //Ссылка на родительский узел
    QDomNode m_node;
};

#endif // TREEITEM_H

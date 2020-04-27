#include "treeitem.h"
#include <QString>

TreeItem::TreeItem (const QVector<QVariant> &data,QDomNode &node,TreeItem *parent)
{
    m_itemData = data;
    m_parentItem = parent;
    m_node = node;
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}


void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
    //Добавить узел в список потомков
}

void TreeItem::insertChild(int index, TreeItem *child)
{
    m_childItems.insert(index,child);
}

TreeItem *TreeItem::child (int row)
{
    return m_childItems.value(row);
    //По номеру строки выбрать нужного потомка из списка
}

int TreeItem::childCount() const
{
    return m_childItems.count();
    //Количество потомков узла = длине списка потомков
}

int TreeItem::columnCount() const
{
    return 2;
    //Количество столбцов в узле = длине списка данных узла
}

QVariant TreeItem::data (int col) const
{
    return m_itemData.value(col);
    //Взять данные из нужной строки
}

TreeItem *TreeItem::parentItem() {
    return m_parentItem; //Вернуть ссылку на родителя
}

int TreeItem::childNumber() const
{
    //Если есть родитель - найти свой номер в списке его потомков
    if (m_parentItem) return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));
    return 0; //Иначе вернуть 0*/
}

/*
 Следующие 4 метода просто управляют контейнерами класса m_childItems и m_itemData,
 предназначенными для хранения данных
*/

bool TreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > m_childItems.size()) return false;
    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        static int numberElement = 1;
        data[0]="element"+QString::number(numberElement);
        data[1]="";
        QDomElement node;
        TreeItem *item = new TreeItem(data,node,this);
        m_childItems.insert(position, item);
    }
    return true;
}

bool TreeItem::insertColumns(int position, int columns) {
    if (position < 0 || position > m_itemData.size()) return false;
    for (int column = 0; column < columns; ++column)
        m_itemData.insert(position, QVariant());
    foreach (TreeItem *child, m_childItems) child->insertColumns(position, columns);
    return true;
}

bool TreeItem::removeChildren(int position, int count) {
    if (position < 0 || position + count > m_childItems.size())
    {
        return false;
    }

    for (int row = 0; row < count; ++row)
    {
        QDomNode remNode = m_childItems.takeAt(position)->getDomNode();

        switch(remNode.nodeType())
        {
        case QDomNode::ElementNode:{
            m_node.removeChild(remNode);
        }break;
        case QDomNode::AttributeNode:{
            m_node.toElement().removeAttributeNode(remNode.toAttr());
        }break;
        case QDomNode::CDATASectionNode:{
        }break;
        case QDomNode::EntityReferenceNode:{
        }break;
        case QDomNode::EntityNode:{
        }break;
        case QDomNode::ProcessingInstructionNode:{

        }break;
        case QDomNode::TextNode:{
            m_node.removeChild(remNode);
        }break;
        case QDomNode::CommentNode:{
            m_node.removeChild(remNode);
        }break;
        case QDomNode::DocumentNode: {}break;
        case QDomNode::DocumentTypeNode: {}break;
        case QDomNode::DocumentFragmentNode: {}break;
        case QDomNode::NotationNode: {}break;
        case QDomNode::BaseNode: {}break;
        case QDomNode::CharacterDataNode: {}break;
        }
    }
    return true;
}

//А этот метод ставит значение value в столбец column элемента:
bool TreeItem::setData(int column, const QVariant &value) {
    bool isSuccess = false;
    if (column < 0 || column >= m_itemData.size()) return false;

    if(column==0)
    {
        isSuccess=setNameNode(value);
    }
    else
    {
        isSuccess=setValueNode(value);
    }

    if(isSuccess)
    {
        m_itemData[column] = value;
    }

    return isSuccess;
}

QDomNode &TreeItem::getDomNode()
{
    return m_node;
}

void TreeItem::setDomNode(QDomNode &node)
{
    m_node=node.toElement();
}

bool TreeItem::setNameNode(const QVariant &value)
{
    bool needChangeName = false;

    switch(m_node.nodeType())
    {
    case QDomNode::ElementNode:{
        m_node.toElement().setTagName(value.toString());
        needChangeName = true;
    }break;
    case QDomNode::AttributeNode:{
        QString nameNode = value.toString();
        QDomElement element = m_parentItem->m_node.toElement();
        element.setAttribute(nameNode,m_node.nodeValue());
        element.removeAttribute(m_node.nodeName());
        m_node = element.attributeNode(nameNode);
        needChangeName = true;
    }break;
    case QDomNode::CDATASectionNode:{
    }break;
    case QDomNode::EntityReferenceNode:{
    }break;
    case QDomNode::EntityNode:{
    }break;
    case QDomNode::ProcessingInstructionNode:{

    }break;
    case QDomNode::TextNode:{

    }break;
    case QDomNode::CommentNode:{

    }break;
    case QDomNode::DocumentNode: {}break;
    case QDomNode::DocumentTypeNode: {}break;
    case QDomNode::DocumentFragmentNode: {}break;
    case QDomNode::NotationNode: {}break;
    case QDomNode::BaseNode: {}break;
    case QDomNode::CharacterDataNode: {}break;
    }
    return needChangeName;
}

bool TreeItem::setValueNode(const QVariant &value)
{
    bool needChangeValue = false;
    switch(m_node.nodeType())
    {
    case QDomNode::ElementNode:{
        //m_node.setNodeValue(value.toString());
        //needChangeValue = true;
    }break;
    case QDomNode::AttributeNode:{
        m_node.setNodeValue(value.toString());
        needChangeValue = true;
    }break;
    case QDomNode::CDATASectionNode:{
    }break;
    case QDomNode::EntityReferenceNode:{
    }break;
    case QDomNode::EntityNode:{
    }break;
    case QDomNode::ProcessingInstructionNode:{

    }break;
    case QDomNode::TextNode:{
        m_node.setNodeValue(value.toString());
        needChangeValue = true;
    }break;
    case QDomNode::CommentNode:{
        m_node.setNodeValue(value.toString());
        needChangeValue = true;
    }break;
    case QDomNode::DocumentNode: {}break;
    case QDomNode::DocumentTypeNode: {}break;
    case QDomNode::DocumentFragmentNode: {}break;
    case QDomNode::NotationNode: {}break;
    case QDomNode::BaseNode: {}break;
    case QDomNode::CharacterDataNode: {}break;
    }
    return needChangeValue;
}

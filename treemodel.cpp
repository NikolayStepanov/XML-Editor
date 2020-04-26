#include "treemodel.h"

TreeModel::TreeModel(const QDomDocument &doc, QObject *parent)
    : QAbstractItemModel(parent)
{
    domDocument = doc;
    //Создали корневой элемент
    rootItem = new TreeItem({tr("Component"),tr("Value")},domDocument);
    setupModelData(doc, rootItem);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}


static const QString &nodeTypeName(QDomNode::NodeType nodeType) {
    static QString Element("Element"), Attribute("Attribute"), Text("Text"),
            CDATA("CData"), EntityReference("Entity Reference"), Entity("Entity"),
            ProcessingInstruction("Processing Instruction"), Comment("Comment"),
            Document("Document"), DocumentType("Document Type"),
            DocumentFragment("DocumentFragment"), Notation("Notation"),
            Base("Untyped?"), CharacterData("CharacterData");
    switch(nodeType) {
    case QDomNode::ElementNode:               return Element;
    case QDomNode::AttributeNode:             return Attribute;
    case QDomNode::TextNode:                  return Text;
    case QDomNode::CDATASectionNode:          return CDATA;
    case QDomNode::EntityReferenceNode:       return EntityReference;
    case QDomNode::EntityNode:                return Entity;
    case QDomNode::ProcessingInstructionNode: return ProcessingInstruction;
    case QDomNode::CommentNode:               return Comment;
    case QDomNode::DocumentNode:              return Document;
    case QDomNode::DocumentTypeNode:          return DocumentType;
    case QDomNode::DocumentFragmentNode:      return DocumentFragment;
    case QDomNode::NotationNode:              return Notation;
    case QDomNode::BaseNode:                  return Base;
    case QDomNode::CharacterDataNode:         return CharacterData;
    }
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return rootItem->columnCount();
}

QVariant TreeModel::data (const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole  && role != Qt::EditRole) return QVariant();
    TreeItem *item = getItem(index);
    return item->data(index.column());
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) return rootItem->data(section);
    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0) return QModelIndex();
    TreeItem *parentItem = getItem(parent);
    TreeItem *childItem = parentItem->child(row);
    if (childItem) return createIndex(row, column, childItem);
    else return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return QModelIndex();
    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parentItem();
    if (parentItem == rootItem) return QModelIndex();
    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

void TreeModel::setupModelData(const QDomDocument &domDocument, TreeItem *parent)
{
    traverseXmlNode(domDocument,parent);
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) return false;
    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);
    if (result) {
        emit dataChanged(index, index);
    }
    return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role) {
    if (role != Qt::EditRole || orientation != Qt::Horizontal) return false;
    bool result = rootItem->setData(section, value);
    if (result) {
        emit headerDataChanged(orientation, section, section);
    }
    return result;
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;
    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();
    return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success;
    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();
    return success;
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    return true;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success = true;
    beginRemoveRows(parent, position, position + rows-1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();
    return success;
}

bool TreeModel::createElement(const QModelIndex &index)
{
    if(index.row()!=0)
    {
        QModelIndex siblingIndex=index.siblingAtRow(index.row()-1);

        TreeItem * item = getItem(index);
        TreeItem * siblingItem = getItem(siblingIndex);

        QDomElement element = domDocument.createElement("element");
        QDomElement siblingNode = siblingItem->getDomNode().toElement();
        QDomNode parentNode = item->parentItem()->getDomNode();
        parentNode.insertAfter(element,siblingNode);

        QDomNode itemNode = item->getDomNode();
        itemNode =element;
    }
    return true;
}

bool TreeModel::createElement(const QModelIndex &index, TreeModel::wayInsertItem wayInsert)
{
    switch (wayInsert) {
    case Before:{
        beginInsertRows(index.parent(), index.row(), index.row());

        TreeItem * item = getItem(index);
        TreeItem * parentItem = item->parentItem();

        QVector<QVariant> data(2);

        data[0]="element";
        data[1]="";

        QDomElement element = domDocument.createElement(data[0].toString());
        QDomNode parentNode = parentItem->getDomNode();
        parentNode.insertBefore(element,item->getDomNode());

        TreeItem * newItem= new TreeItem(data,element,parentItem);
        parentItem->insertChild(index.row(),newItem);

        endInsertRows();
    }break;
    case After:{
        beginInsertRows(index.parent(), index.row()+1, index.row()+1);

        TreeItem * item = getItem(index);
        TreeItem * parentItem = item->parentItem();

        QVector<QVariant> data(2);
        data[0]="element";
        data[1]="";

        QDomElement element = domDocument.createElement(data[0].toString());
        QDomNode parentNode = parentItem->getDomNode();
        parentNode.insertAfter(element,item->getDomNode());

        TreeItem * newItem= new TreeItem(data,element,parentItem);
        parentItem->insertChild(index.row()+1,newItem);
        endInsertRows();
    }break;
    case Child:{
        beginInsertRows(index,0,0);

        TreeItem * item = getItem(index);

        QVector<QVariant> data(2);
        data[0]="element";
        data[1]="";

        QDomElement element = domDocument.createElement(data[0].toString());
        QDomNode itemNode = item->getDomNode();
        if(itemNode.isElement())
        {
            itemNode.appendChild(element);
            TreeItem * newItem= new TreeItem(data,element,item);
            item->appendChild(newItem);
        }
        endInsertRows();
    }break;
    }
}

void TreeModel::traverseXmlNode(const QDomNode& node, TreeItem* parent)
{
    QDomNode domNode = node.firstChild();

    tabCount++; // increase current tab count
    while(!(domNode.isNull()))
    {
        TreeItem* ptrNewNode = NULL;
        QVector <QVariant> columnData;
        switch(domNode.nodeType())
        {
        case QDomNode::ElementNode:{
            QDomElement element = domNode.toElement();
            columnData.push_back(element.tagName());
            ptrNewNode = new TreeItem(columnData,domNode,parent);
            columnData.push_back(QVariant());
            QDomNamedNodeMap map = element.attributes();
            for (int id = 0 ; id < map.size() ; id++ )
            {
                QDomNode node = map.item( id );
                QVector <QVariant> columnData;

                columnData.push_back(node.nodeName());
                columnData.push_back(node.nodeValue());

                TreeItem* ptrNewNodeAttr = new TreeItem(columnData, node, ptrNewNode);
                ptrNewNode->appendChild(ptrNewNodeAttr);
            }
            parent->appendChild(ptrNewNode);
        }break;
        case QDomNode::AttributeNode:{
            QDomAttr attr = domNode.toAttr();
            columnData.push_back(attr.name());
            columnData.push_back(attr.value());
            ptrNewNode = new TreeItem(columnData, domNode, parent);
            parent->appendChild(ptrNewNode );
        }break;
        case QDomNode::CDATASectionNode:{}break;
        case QDomNode::EntityReferenceNode:{}break;
        case QDomNode::EntityNode:{}break;
        case QDomNode::ProcessingInstructionNode:{}break;
        case QDomNode::TextNode:{
            QDomText text = domNode.toText();
            columnData.push_back("#text");
            columnData.push_back(text.data());
            ptrNewNode = new TreeItem(columnData, domNode, parent);
            parent->appendChild(ptrNewNode );
        }break;
        case QDomNode::CommentNode:{
            QDomComment comment = domNode.toComment();
            columnData.push_back("#comment");
            columnData.push_back(comment.data());
            ptrNewNode = new TreeItem(columnData,domNode, parent);
            parent->appendChild(ptrNewNode );
        }break;
        case QDomNode::DocumentNode: {}break;
        case QDomNode::DocumentTypeNode: {}break;
        case QDomNode::DocumentFragmentNode: {}break;
        case QDomNode::NotationNode: {}break;
        case QDomNode::BaseNode: {}break;
        case QDomNode::CharacterDataNode: {}break;
        }

        columnData.push_back(nodeTypeName(domNode.nodeType()));

        traverseXmlNode(domNode, ptrNewNode); // recursive function
        domNode = domNode.nextSibling();
    }
    /* while(!(domNode.isNull()))
    {
        TreeItem* ptrNewNode = NULL;

        if(domNode.isElement())
        {
            domElement = domNode.toElement();
            if ( domElement.isNull() )
                continue;
            QVector <QVariant> columnData;
            columnData.push_back(domElement.tagName());
            ptrNewNode = new TreeItem(columnData, parent);// set parent

            // attribute
            QDomNamedNodeMap map = domElement.attributes();
            for (int id = 0 ; id < map.size() ; id++ )
            {
                QDomNode node = map.item( id );
                QVector <QVariant> columnData;

                columnData.push_back(node.nodeName());
                columnData.push_back(node.nodeValue());

                TreeItem* ptrNewNodeAttr = new TreeItem(columnData, ptrNewNode);
                ptrNewNode->appendChild(ptrNewNodeAttr);
            }
            parent->appendChild(ptrNewNode ); // append child
        }
        traverseXmlNode(domNode, ptrNewNode); // recursive function
        domNode = domNode.nextSibling();
    }*/
    tabCount--; // decrease current tab count
}


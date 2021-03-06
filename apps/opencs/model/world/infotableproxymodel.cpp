#include "infotableproxymodel.hpp"

#include "idtablebase.hpp"
#include "columns.hpp"

CSMWorld::InfoTableProxyModel::InfoTableProxyModel(CSMWorld::UniversalId::Type type, QObject *parent)
    : IdTableProxyModel(parent),
      mType(type),
      mSourceModel(NULL)
{
    Q_ASSERT(type == UniversalId::Type_TopicInfos || type == UniversalId::Type_JournalInfos);
}

void CSMWorld::InfoTableProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    IdTableProxyModel::setSourceModel(sourceModel);
    mSourceModel = dynamic_cast<IdTableBase *>(sourceModel);
    connect(mSourceModel, 
            SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, 
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
    mFirstRowCache.clear();
}

bool CSMWorld::InfoTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QModelIndex first = mSourceModel->index(getFirstInfoRow(left.row()), left.column());
    QModelIndex second = mSourceModel->index(getFirstInfoRow(right.row()), right.column());
    return IdTableProxyModel::lessThan(first, second);
}

int CSMWorld::InfoTableProxyModel::getFirstInfoRow(int currentRow) const
{
    Columns::ColumnId columnId = Columns::ColumnId_Topic;
    if (mType == UniversalId::Type_JournalInfos)
    {
        columnId = Columns::ColumnId_Journal;
    }

    int column = mSourceModel->findColumnIndex(columnId);
    QString info = mSourceModel->data(mSourceModel->index(currentRow, column)).toString();

    if (mFirstRowCache.contains(info))
    {
        return mFirstRowCache[info];
    }

    while (--currentRow >= 0 &&
           mSourceModel->data(mSourceModel->index(currentRow, column)) == info);

    mFirstRowCache[info] = currentRow + 1;
    return currentRow + 1;
}

void CSMWorld::InfoTableProxyModel::modelDataChanged(const QModelIndex &/*topLeft*/, const QModelIndex &/*bottomRight*/)
{
    mFirstRowCache.clear();
}

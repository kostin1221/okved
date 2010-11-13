#include "okvedssortfilterproxymodel.h"

#include <QSettings>
#include <QDebug>
#include <QSqlTableModel>

OkvedsSortFilterProxyModel::OkvedsSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
	hide_not_checked = false;
    qRegisterMetaTypeStreamOperators<CheckedList>("CheckedList");
    QSettings settings("qokved", "qokved");
    QVariant var = settings.value("user_filter");
    check = qvariant_cast<CheckedList>(var);
    check = var.value<CheckedList>();
}


OkvedsSortFilterProxyModel::~OkvedsSortFilterProxyModel()
{
    QSettings settings("qokved", "qokved");
    QVariant var;
    var.setValue<CheckedList>(check);
    settings.setValue("user_filter", var);
}

bool OkvedsSortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    int check_state = check.value(sourceModel()->index(sourceRow, 0, sourceParent).data(Qt::DisplayRole).toString(), Qt::Unchecked);
    if (hide_not_checked && check_state == Qt::Unchecked)
        return false;

    if (hide_not_checked) {
        qDebug() << check.value(sourceModel()->data(sourceModel()->index(sourceRow, 0), Qt::DisplayRole).toString(), Qt::Unchecked);
        qDebug() << sourceModel()->data(sourceModel()->index(sourceRow, 1)).toString();
        qDebug() << sourceRow;
    }
   
    switch (filter_type){
    case NAME:
	if (!sourceModel()->data(sourceModel()->index(sourceRow, 2)).toString().contains(filter_string, Qt::CaseInsensitive))
	    return false;
	break;

    case NUMBER:
	QString sm = sourceModel()->data(sourceModel()->index(sourceRow, 1)).toString();
	sm.remove(".");
	qDebug() << filter_string;
	qDebug() << sm;
	if (!sm.startsWith(filter_string)){
	    qDebug() << filter_string;
	    qDebug() << sm;
	    return false;}
	break;

    }


    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

bool OkvedsSortFilterProxyModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if (index.column() == 1  && role==Qt::CheckStateRole)
    {
        if ( value.type() == QVariant::Int ) {
	   // qDebug() << sourceModel()->data(sourceModel()->index(index.row(), 0), 0).toString();
	    //qDebug() << check[data(mapFromSource(index), 0).toString()];
	    check[sourceModel()->index(mapToSource(index).row(), 0).data().toString()] = value.toInt();
            return true;
        }

    }
       // return QSortFilterProxyModel::setData ( index, value, role );
       return  sourceModel()->setData ( index, value, role );
}

QVariant OkvedsSortFilterProxyModel::data(const QModelIndex &item, int role) const
{
    if (item.column() == 1 && role==Qt::CheckStateRole)
	return check[sourceModel()->index(mapToSource(item).row(), 0).data().toString()];
             
    return sourceModel()->data(mapToSource(item), role);
   
}

Qt::ItemFlags OkvedsSortFilterProxyModel::flags(const QModelIndex &index) const
{
        Qt::ItemFlags flags = QSortFilterProxyModel::flags(index);
        if (index.column() == 1)
                flags |= Qt::ItemIsUserCheckable;
        return flags;
}

void OkvedsSortFilterProxyModel::setFilter(QString value)
{
    //Если есть буквы - фильтруем по наименованию, если только цифры - по номеру, если пустая строка - выключаем фильтр
        if (value.contains(QRegExp(QString::fromUtf8("(?:[a-z]|[A-Z]|[а-я]|[А-Я])")))) 
        {
            filter_string = value;
            filter_type = NAME;
        } else if (value.count() > 0){
            filter_string = value.remove(".");
            filter_type = NUMBER;
        } else {
            filter_string = "";
            filter_type = NONE;
        }
        
        invalidateFilter();
}


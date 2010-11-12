#ifndef LIBQOKVED_H
#define LIBQOKVED_H

#include <QtSql>
#include <QDebug>
#include <QStringList>

typedef QHash<QString, int> CheckedList;
Q_DECLARE_METATYPE(CheckedList)


class myQSqlQueryModel:public QSqlTableModel
	{
		Q_OBJECT

	public:
                explicit myQSqlQueryModel ( QObject * parent = 0, QSqlDatabase db = QSqlDatabase() );
                ~myQSqlQueryModel ( );
                CheckedList check;
		QVariant data(const QModelIndex &item, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	};

class Libqokved: public QObject {
    Q_OBJECT

public:
    explicit Libqokved(QObject* parent);
    ~Libqokved();
    bool setDbPath(QString db_path);
    void create_tables();
    void fill_db_from_zakon(QString zakon);
    QSqlTableModel* razdels_model();
    myQSqlQueryModel* okveds_model(int rid);

    bool setActiveVersion(int ver);
    int createVersion(QString name);
    QMap<int, QString> versions();

private:
    int active_version;
    QSqlDatabase db;
    QSqlQuery *query;
    myQSqlQueryModel *okved_mdl;
    QSqlTableModel *razdels_mdl;

public slots:
    void update_db_date();

};

#endif // LIBQOKVED_H

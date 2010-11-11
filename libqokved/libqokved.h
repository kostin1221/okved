#ifndef LIBQOKVED_H
#define LIBQOKVED_H

#include <QtSql>
#include <QDebug>
#include <QStringList>

class myQSqlQueryModel:public QSqlTableModel
	{
		Q_OBJECT
	public:
		QHash<int, bool> check;
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
    myQSqlQueryModel* razdels_model();
    myQSqlQueryModel* okveds_model(int rid);

    bool setActiveVersion(int ver);
    int createVersion(QString name);
    QMap<int, QString> versions();

private:
    int active_version;
    QSqlDatabase db;
    QSqlQuery *query;

public slots:
    void update_db_date();

};

#endif // LIBQOKVED_H

#ifndef LIBQOKVED_H
#define LIBQOKVED_H

#include "okvedssortfilterproxymodel.h"

#include <QtSql>
#include <QDebug>
#include <QStringList>

class Libqokved: public QObject {
    Q_OBJECT

public:
    explicit Libqokved(QObject* parent);
    ~Libqokved();
    bool setDbPath(QString db_path);
    void create_tables();
    void fill_db_from_zakon(QString zakon);
    QSqlTableModel* razdels_model();
    QSqlTableModel* okveds_model(int rid);

    bool setActiveVersion(int ver);
    int createVersion(QString name);
    QMap<int, QString> versions();

    void save_global_list(CheckedList checks, QString name);
    CheckedList getGlobalList(QString name);
    QStringList globalLists();

private:
    int active_version;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *okved_mdl;
    QSqlTableModel *razdels_mdl;

public slots:
    void update_db_date();

};

#endif // LIBQOKVED_H

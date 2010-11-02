#ifndef LIBQOKVED_H
#define LIBQOKVED_H

#include <QtSql>
#include <QDebug>
#include <QStringList>

struct Razdel {
    int rid;
    QString name;
};

struct Okved {
    int oid;
    QString name;
    QString addition;
    int razdel_id;
};

class Libqokved {
public:
    Libqokved();
    ~Libqokved();
    bool setDbPath(QString db_path);
    void create_tables();
    void fill_db_from_zakon(QString zakon);
    QSqlTableModel* razdels_model();
    QList<Okved> okved_in_razdel_list(int razdel);

private:
    QSqlDatabase db;
    QSqlQuery *query;
};

#endif // LIBQOKVED_H

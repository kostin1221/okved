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
    bool createVersion(QString name);
    QMap<int, QString> versions();

private:
    int active_version;
    QSqlDatabase db;
    QSqlQuery *query;

public slots:
    void update_db_date();

};

#endif // LIBQOKVED_H

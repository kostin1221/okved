#include "libqokved.h"


Libqokved::Libqokved()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    //QSqlQuery *query = new QSqlQuery(db);
}

Libqokved::~Libqokved()
{
  //  delete query;

}

bool Libqokved::setDbPath(QString db_path)
{
    if (db.isOpen()) db.close();
    db.setDatabaseName(db_path);
    if (!db.open())
    {
        qDebug() << db.lastError().text();
        return false;
    }
    if ( !db.tables().contains("razdelz") ) create_tables();
    return true;
}

void Libqokved::create_tables()
{ 
    QSqlQuery query;
    if (!query.exec("CREATE TABLE razdelz (\"rid\" INTEGER PRIMARY KEY, \"name\" TEXT)"))
      qDebug() << query.lastError();
    if (!query.exec("CREATE TABLE okveds (\"oid\" INTEGER PRIMARY KEY, \"name\" TEXT, \"addition\" TEXT, \"razdel_id\" INTEGER )"))
      qDebug() << query.lastError();
}

QList<Razdel> Libqokved::razdels_list()
{
    QSqlQuery query("SELECT rid, name FROM razdelz");
    QList<Razdel> razdel_list;
    while (query.next()) {
        Razdel razdel;
        razdel.rid = query.value(0).toInt();
        razdel.name = query.value(1).toString();
        razdel_list.append(razdel);

    }
    return razdel_list;

}


void Libqokved::fill_db_from_zakon(QString zakon)
{

        QFile file(zakon);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
    
        int i = 0;
        int last_razdel = 0;
    
        QString id_str;
        QString add_str;
        bool add_info=false;
        while (!file.atEnd()) {
    
           // if ( i> 3000 ) continue;
            QByteArray line = file.readLine();
            QString line_str = QString::fromUtf8(line.data()).simplified();
    
            if (line_str.startsWith(QString::fromUtf8("РАЗДЕЛ "))){
                QSqlQuery query;
                query.prepare("INSERT INTO razdelz (name) "
                                   "VALUES (:name)");
                query.bindValue(":name", line_str);
                query.exec();
                last_razdel = query.lastInsertId().toInt();
    
            } else if (line_str.startsWith(QString::fromUtf8("Эта группировка "))){
                add_info = true;
                if (!add_str.isEmpty()) add_str.append("\n");
    
                add_str.append(line_str);
    
            } else if (line_str.left(2).toInt() != 0 && line_str.contains(" ")){
                if (id_str.isEmpty()) continue;

                QSqlQuery query;
                query.prepare("INSERT INTO okveds (name, addition, razdel_id) "
                              "VALUES (:name, :addition, :razdel_id)");
                query.bindValue(":name", id_str);
                query.bindValue(":razdel_id", last_razdel);


              //  qDebug() << "razdel: " << id_str;
                id_str.clear();
                id_str.append(line_str);
                if (add_info){
                 //   qDebug() << "dop razdel: " << add_str;
                    query.bindValue(":addition", add_str);
                    add_str.clear();
                    add_info = false;
                } else {query.bindValue(":addition", "");}

                query.exec();
            }
            else if (add_info) {
                if (line_str.startsWith(QString::fromUtf8("- "))){
                    add_str.append("\n" + line_str);
                } else add_str.append(" " + line_str);
            }
            else {
                id_str.append(" " + line_str);
            }
    
            i++;
        }
}

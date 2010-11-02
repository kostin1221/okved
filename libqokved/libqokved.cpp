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
    if (!query.exec("CREATE TABLE razdelz (\"rid\" INTEGER PRIMARY KEY, \"name\" TEXT, \"caption\" TEXT, \"father\" INTEGER)"))
      qDebug() << query.lastError();
    if (!query.exec("CREATE TABLE okveds (\"oid\" INTEGER PRIMARY KEY, \"name\" TEXT, \"number\" TEXT, \"addition\" TEXT, \"razdel_id\" INTEGER )"))
      qDebug() << query.lastError();
}

QSqlTableModel* Libqokved::razdels_model()
{
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("razdelz");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->removeColumn(0);
    model->removeColumn(1);
    model->removeColumn(1);
    model->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("Раздел"));

    return model;
    //model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

}

QList<Okved> Libqokved::okved_in_razdel_list(int razdel)
{
    QSqlQuery query("SELECT oid, name, addition FROM okveds WHERE razdel_id=");
    QList<Okved> okved_list;
    while (query.next()) {
        Okved okved;
     //  okved.rid = query.value(0).toInt();
     //   okved.name = query.value(1).toString();
        okved_list.append(okved);

    }
    return okved_list;
}

void Libqokved::fill_db_from_zakon(QString zakon)
{

        QFile file(zakon);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
    
        int i = 0;
        int last_razdel = 0;
        int last_podrazdel = 0;
    
        QString id_str;
        QString add_str;
        QString pordazdel_str;
        QString okved_number;
        bool prilozhenie=false;
        bool skobki=false;
        bool add_info=false;
        bool podrazdel_info=false;

        while (!file.atEnd()) {
            if (prilozhenie) break;
           // if ( i> 3000 ) continue;
            QByteArray line = file.readLine();
            QString line_str = QString::fromUtf8(line.data()).simplified();
            QString old_line_str = line_str;
            line_str.replace("  ", " ");
            while (old_line_str != line_str) { line_str.replace("  ", " "); };

            if (line_str.isEmpty()) continue;

            if (line_str.startsWith(QString::fromUtf8("Приложение А")))
            {
              prilozhenie = true;
            } else if (line_str.startsWith(QString::fromUtf8("("))){
                skobki = true;
            }
            else if (line_str.endsWith(QString::fromUtf8(")"))){
                 skobki = false;
            }
            else if (skobki){ continue;}
            else if (line_str.startsWith(QString::fromUtf8("РАЗДЕЛ "))){
                QSqlQuery query;
                query.prepare("INSERT INTO razdelz (name, father) "
                                   "VALUES (:name, 0)");
                query.bindValue(":name", line_str);
                query.exec();
                last_razdel = query.lastInsertId().toInt();
                last_podrazdel = 0;
    
            } else if (line_str.startsWith(QString::fromUtf8("Подраздел "))){
                qDebug() << pordazdel_str;
                pordazdel_str.clear();

                podrazdel_info = true;
                QSqlQuery query;
                query.prepare("INSERT INTO razdelz (name, father) "
                                   "VALUES (:name, :father)");
                query.bindValue(":name", line_str);
                query.bindValue(":father", last_razdel);
                query.exec();


                last_podrazdel = query.lastInsertId().toInt();

            } else if (line_str.startsWith(QString::fromUtf8("Эта группировка ")) || line_str.startsWith(QString::fromUtf8("В группировке "))){
                add_info = true;
                if (!add_str.isEmpty()) add_str.append("\n");
    
                add_str.append(line_str);
    
            } else if (line_str.left(2).toInt() != 0 && line_str.contains(" ")){
                int prob_index = line_str.indexOf(" ");
                //QStringList num_text = line_str.split("\t");

                if (id_str.isEmpty()) {
                    okved_number = line_str.left(prob_index);
                    id_str = line_str.right(line_str.count() - prob_index-1);
                    continue;
                }

                podrazdel_info = false;
                QSqlQuery query;
                query.prepare("INSERT INTO okveds (name, addition, razdel_id) "
                              "VALUES (:number, :name, :addition, :razdel_id)");
                query.bindValue(":number", okved_number);
                query.bindValue(":name", id_str);

                if (last_podrazdel!=0) {
                    query.bindValue(":razdel_id", last_podrazdel);
                } else query.bindValue(":razdel_id", last_razdel);

                if (add_info){
                 //   qDebug() << "dop razdel: " << add_str;
                    query.bindValue(":addition", add_str);
                    add_str.clear();
                    add_info = false;
                } else {query.bindValue(":addition", "");}

                query.exec();

                okved_number = line_str.left(prob_index);
                id_str = line_str.right(line_str.count() - prob_index-1);
            }
            else if (podrazdel_info) {
                pordazdel_str.append(line_str);

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

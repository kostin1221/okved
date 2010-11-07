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

//    QVariant v = db.driver()->handle();
//    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0) {
//        // v.data() returns a pointer to the handle
//        sqlite3 *handle = *static_cast<sqlite3 **>(v.data());
//        char *zErrMsg = 0;

//        if (handle != 0) { // check that it is not NULL
//            int res = sqlite3_enable_load_extension(handle,1);
//            if (res == SQLITE_OK)
//            {
//                res = sqlite3_load_extension(handle,QDir::currentPath().toUtf8()+"/libSqliteIcu.so",0,&zErrMsg);
//                if (res == SQLITE_OK)
//                   ;
//                else
//                    fprintf(stderr, "Error loading SqliteIcu: %s\n", zErrMsg);
//            }
//            else
//                   qDebug() << "Sqlite3 enable load extension fail";
//        }
//    }

    if ( !db.tables().contains("razdelz") ) create_tables();
    return true;
}

//Создает таблицы в новой базе данных
void Libqokved::create_tables()
{ 
    QSqlQuery query;
    if (!query.exec("CREATE TABLE razdelz (\"rid\" INTEGER PRIMARY KEY, \"name\" TEXT, \"caption\" TEXT, \"father\" INTEGER)"))
      qDebug() << query.lastError();
    if (!query.exec("CREATE TABLE okveds (\"oid\" INTEGER PRIMARY KEY, \"number\" TEXT, \"name\" TEXT, \"addition\" TEXT, \"razdel_id\" INTEGER )"))
      qDebug() << query.lastError();
}

QSqlTableModel* Libqokved::razdels_model()
{
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("razdelz");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();
   // model->removeColumn(0);
  //  model->insertRow(0);
   // model->removeColumn(2);
    //model->removeColumn(2);

    model->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("id"));
    model->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("Раздел"));

    model->setSort(0, Qt::AscendingOrder); // Сортировка по id

    return model;
    //model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

}

QSqlTableModel* Libqokved::okveds_model(int rid)
{
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("okveds");

    QString filter;
    if (rid != 1) {
        filter = "(razdel_id="+QString::number(rid);

        QSqlQuery query("SELECT rid FROM razdelz WHERE father="+QString::number(rid));
        while (query.next()) {
            QString rid2 = query.value(0).toString();

            filter.append(" OR razdel_id="+rid2);
        }
        filter.append(")");
    } else filter.clear();

    model->setFilter(filter);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();

    model->setSort(1, Qt::AscendingOrder); // Сортировка по номеру
    model->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("Номер"));
    model->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("Наименование"));

    //model->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("id"));
  //  model->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("Раздел"));

    return model;
    //model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

}

void Libqokved::fill_db_from_zakon(QString zakon)
{

        QString osn_text = zakon.split(QString::fromUtf8("Приложение А")).at(0);
        QString prilozhenie_a = zakon.split(QString::fromUtf8("Приложение А")).at(1);

        int i = 0;
        int last_razdel = 0;
        int last_podrazdel = 0;
    
        QString id_str;
        QString add_str;
        QString pordazdel_str;
        QString okved_number;
        bool skobki=false;
        bool add_info=false;
        bool podrazdel_info=false;

        QSqlQuery query;
        query.prepare("DELETE FROM razdelz");
        query.exec();
        query.prepare("DELETE FROM okveds");
        query.exec();

        query.prepare("INSERT INTO razdelz (name, father) "
                           "VALUES (:name, 9999)");
        query.bindValue(":name", QString::fromUtf8("Все разделы"));
        query.exec();

        QStringList lines = osn_text.split("\n");
        for (int o = 0; o < lines.size(); ++o) {
            QString line_str = lines.at(o).simplified();
            while (line_str.contains("  ")) {
                line_str = line_str.replace("  ", " ");
            };

            if (line_str.isEmpty()) continue;

            if (line_str.startsWith(QString::fromUtf8("("))){
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

                pordazdel_str.clear();
                podrazdel_info = true;
                QSqlQuery query;
                query.prepare("INSERT INTO razdelz (name, father) "
                                   "VALUES (:name, :father)");

                if (!lines[o+1].simplified().isEmpty()) line_str = line_str + " " + lines[o+1].simplified();


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

                if (id_str.isEmpty()) {
                    okved_number = line_str.left(prob_index);
                    id_str = line_str.right(line_str.count() - prob_index-1);
                    continue;
                }

                podrazdel_info = false;
                QSqlQuery query;
                query.prepare("INSERT INTO okveds (number, name, addition, razdel_id) "
                              "VALUES (:number, :name, :addition, :razdel_id)");
                query.bindValue(":number", okved_number);
                query.bindValue(":name", id_str);

                if (last_podrazdel!=0) {
                    query.bindValue(":razdel_id", last_podrazdel);
                } else query.bindValue(":razdel_id", last_razdel);

                QString ab;
                if (prilozhenie_a.contains(okved_number+" ")){
                    QString aa = prilozhenie_a.right(prilozhenie_a.count() - prilozhenie_a.indexOf(okved_number+" "));

                    ab = aa.left(aa.indexOf(QRegExp(QString::fromUtf8("(?:\n[0-9]{1,2}|РАЗДЕЛ |Подраздел )")), aa.indexOf(" ")));

                    while (ab.contains("  ")) {
                        ab = ab.replace("  ", " ");
                    };

                    query.bindValue(":addition", ab);
                } else query.bindValue(":addition", "");

                if (ab == okved_number + " " +id_str.replace("\n", "")) query.bindValue(":addition", "");

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

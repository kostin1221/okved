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
    if (!query.exec("CREATE TABLE okveds (\"oid\" INTEGER PRIMARY KEY, \"number\" TEXT, \"name\" TEXT, \"addition\" TEXT, \"razdel_id\" INTEGER )"))
      qDebug() << query.lastError();
}

QSqlTableModel* Libqokved::razdels_model()
{
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("razdelz");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
   // model->removeColumn(0);
    model->removeColumn(2);
    model->removeColumn(2);

    model->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("id"));
    model->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("Раздел"));

    return model;
    //model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

}

QSqlTableModel* Libqokved::okveds_model(int rid)
{
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("okveds");
  //  qDebug() << "razdel_id="+QString::number(rid);
    model->setFilter("razdel_id="+QString::number(rid));
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("Оквед"));

    //model->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("id"));
  //  model->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("Раздел"));

    return model;
    //model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

}

void Libqokved::fill_db_from_zakon(QString zakon)
{

        QFile file(zakon);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
    
        QString data = QString::fromUtf8(file.readAll().data());

        QString osn_text = data.split(QString::fromUtf8("Приложение А")).at(0);
        QString prilozhenie_a = data.split(QString::fromUtf8("Приложение А")).at(1);

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

        QStringList lines = osn_text.split("\n");
        for (int o = 0; o < lines.size(); ++o) {
      //  while (!file.atEnd()) {
        //    if (prilozhenie) break;
           // if ( i> 3000 ) continue;
            QString line_str = lines.at(o).simplified();
            //QString line_str = QString::fromUtf8(line.data()).simplified();
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
                query.prepare("INSERT INTO okveds (number, name, addition, razdel_id) "
                              "VALUES (:number, :name, :addition, :razdel_id)");
                query.bindValue(":number", okved_number);
                query.bindValue(":name", id_str);

                if (last_podrazdel!=0) {
                    query.bindValue(":razdel_id", last_podrazdel);
                } else query.bindValue(":razdel_id", last_razdel);

//                if (add_info){
//                 //   qDebug() << "dop razdel: " << add_str;
//                    query.bindValue(":addition", add_str);
//                    add_str.clear();
//                    add_info = false;
//                } else {query.bindValue(":addition", "");}
query.bindValue(":addition", "");

                QString aa = prilozhenie_a.right(prilozhenie_a.count() - prilozhenie_a.indexOf(okved_number));
                qDebug() << aa.indexOf(QRegExp("[0-9]{1,2}|РАЗДЕЛ|Подраздел"));
                QString ab = aa.left(aa.indexOf(QRegExp("(?:^[0-9]{1,2}|РАЗДЕЛ |Подраздел )")));
                qDebug() << ab;
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

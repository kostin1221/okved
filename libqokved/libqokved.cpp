#include "libqokved.h"

Libqokved::Libqokved(QObject* parent = 0) :
        QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    active_version = -1;

    razdels_mdl = false;
    okved_mdl = false;

}
Libqokved::~Libqokved()
{

}

void Libqokved::update_db_date()
{
     QSqlQuery query;
     query.prepare("INSERT INTO updates (key, value) "
		   "VALUES (:date, :value)");
     query.bindValue(":date", "date");
     query.bindValue(":value", QDateTime::currentDateTime().toString("yyyyMMddhhmm"));
     query.exec();
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
    if ( !db.tables().contains("versions") ) create_tables();
    return true;
}

int Libqokved::createVersion(QString name)
{
    QSqlQuery query;
    query.prepare("INSERT INTO versions (name) "
                       "VALUES (:name)");
    query.bindValue(":name", name);
    query.exec();
    int id_version = query.lastInsertId().toInt();
    query.clear();
    if (!query.exec(QString::fromUtf8("CREATE TABLE razdelz_%1 (\"rid\" INTEGER PRIMARY KEY, \"name\" TEXT, \"caption\" TEXT, \"father\" INTEGER)").arg(QString::number(id_version))))
      qDebug() << query.lastError();
    if (!query.exec(QString::fromUtf8("CREATE TABLE okveds_%1 (\"oid\" INTEGER PRIMARY KEY, \"number\" TEXT, \"name\" TEXT, \"addition\" TEXT, \"razdel_id\" INTEGER )").arg(QString::number(id_version))))
      qDebug() << query.lastError();

    return id_version;
}

bool Libqokved::setActiveVersion(int ver)
{
    active_version = ver;
    return true;
}

QMap<int, QString> Libqokved::versions()
{
    QMap<int, QString> ret;
    QSqlQuery query("SELECT * FROM versions");
    while (query.next()) {
        ret.insert(query.value(0).toInt(), query.value(1).toString());
    }
    return ret;
}

//Создает таблицы в новой базе данных
void Libqokved::create_tables()
{ 
    QSqlQuery query;
    if (!query.exec("CREATE TABLE updates (\"id\" INTEGER PRIMARY KEY, \"key\" TEXT, \"value\" TEXT)"))
      qDebug() << query.lastError();
    if (!query.exec("CREATE TABLE versions (\"id\" INTEGER PRIMARY KEY, \"name\" TEXT)"))
      qDebug() << query.lastError();
    update_db_date();
}

QSqlTableModel* Libqokved::razdels_model()
{
    if (!razdels_mdl)
    {

    razdels_mdl = new QSqlTableModel(this);
    razdels_mdl->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("id"));
    razdels_mdl->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("Раздел"));

    //razdels_mdl->setSort(0, Qt::AscendingOrder); // Сортировка по id
    razdels_mdl->setEditStrategy(QSqlTableModel::OnFieldChange);

    connect (razdels_mdl, SIGNAL(beforeDelete(int)), this, SLOT(update_db_date()));
    connect (razdels_mdl, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(update_db_date()));
    connect (razdels_mdl, SIGNAL(beforeUpdate(int,QSqlRecord&)), this, SLOT(update_db_date()));
    }

    if (active_version==-1) return razdels_mdl;
    razdels_mdl->setTable(QString("razdelz_%1").arg(QString::number(active_version)));
    razdels_mdl->select();
    return razdels_mdl;
}

QSqlTableModel* Libqokved::okveds_model(int rid)
{
    if (!okved_mdl)
    {
    okved_mdl = new QSqlTableModel();

    okved_mdl->setEditStrategy(QSqlTableModel::OnFieldChange);

    okved_mdl->setSort(1, Qt::AscendingOrder); // Сортировка по номеру
    okved_mdl->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("Номер"));
    okved_mdl->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("Наименование"));

    connect (okved_mdl, SIGNAL(beforeDelete(int)), this, SLOT(update_db_date()));
    connect (okved_mdl, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(update_db_date()));
    connect (okved_mdl, SIGNAL(beforeUpdate(int,QSqlRecord&)), this, SLOT(update_db_date()));
    }

    okved_mdl->setTable(QString("okveds_%1").arg(QString::number(active_version)));

    QString filter;
    if (rid != 1) {
	filter = "razdel_id="+QString::number(rid);

	QSqlQuery query(QString("SELECT rid FROM razdelz_%1 WHERE father="+QString::number(rid)).arg(QString::number(active_version)));
	while (query.next()) {
	    QString rid2 = query.value(0).toString();

	    filter.append(" OR razdel_id="+rid2);
	}
    } else filter.clear();

    okved_mdl->setFilter(filter);
    okved_mdl->select();


    return okved_mdl;
}


/*
    Аргумент - строка с законом. Единственный закон 2007 года, который нашел - в консультанте
    экспортируется от туда в таком форматировании, что страшно представить, поэтому парсер такой длинный,
    чтоб хоть как-то сгладить ситуацию.
*/
void Libqokved::fill_db_from_zakon(QString zakon)
{
    zakon = zakon.remove(0, zakon.indexOf(QString::fromUtf8("РАЗДЕЛ A")));
    QString osn_text = zakon.split(QString::fromUtf8("Приложение А")).at(0);
    QString prilozhenie_a = zakon.split(QString::fromUtf8("Приложение А")).at(1);

    int i = 0;
    int last_razdel = 0;
    int last_podrazdel = 0;
    int used_razdel = 0;

    QString id_str;
    QString add_str;
    QString pordazdel_str;
    QString okved_number;
    bool skobki=false;
    bool add_info=false;
    bool change_used_razdel=false;
    bool podrazdel_info = true;

    QSqlQuery query;
    query.prepare(QString("DELETE FROM razdelz_%1").arg(QString::number(active_version)));
    query.exec();
    query.prepare(QString("DELETE FROM okveds_%1").arg(QString::number(active_version)));
    query.exec();

    query.prepare(QString("INSERT INTO razdelz_%1 (name, father) "
		       "VALUES (:name, 9999)").arg(QString::number(active_version)));
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
	    query.prepare(QString("INSERT INTO razdelz_%1 (name, father) "
			       "VALUES (:name, 0)").arg(QString::number(active_version)));
	    QString razdel_name = line_str.toLower();
	    razdel_name[0] = line_str.toUpper()[0];
	    query.bindValue(":name", razdel_name);
	    query.exec();
	    last_razdel = query.lastInsertId().toInt();
	    last_podrazdel = 0;
	    change_used_razdel = true;

	} else if (line_str.startsWith(QString::fromUtf8("Подраздел "))){

	    pordazdel_str.clear();
	    podrazdel_info = true;
	    QSqlQuery query;
	    query.prepare(QString("INSERT INTO razdelz_%1 (name, father) "
			       "VALUES (:name, :father)").arg(QString::number(active_version)));

	    if (!lines[o+1].simplified().isEmpty()) line_str = line_str + " " + lines[o+1].simplified();


	    QString razdel_name = line_str.toLower();
	    razdel_name[0] = line_str.toUpper()[0];
	    query.bindValue(":name", razdel_name);
	    query.bindValue(":father", last_razdel);
	    query.exec();

	    last_podrazdel = query.lastInsertId().toInt();
	    change_used_razdel = true;

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
	    query.prepare(QString("INSERT INTO okveds_%1 (number, name, addition, razdel_id) "
			  "VALUES (:number, :name, :addition, :razdel_id)").arg(QString::number(active_version)));
	    query.bindValue(":number", okved_number);
	    query.bindValue(":name", id_str);


	    query.bindValue(":razdel_id", used_razdel);
//                if (last_podrazdel!=0) {
//                    query.bindValue(":razdel_id", last_podrazdel);
//                } else query.bindValue(":razdel_id", last_razdel);

	    QString ab;
	    if (prilozhenie_a.contains(okved_number+" ")){
		QString aa = prilozhenie_a.right(prilozhenie_a.count() - prilozhenie_a.indexOf(okved_number+" "));

		ab = aa.left(aa.indexOf(QRegExp(QString::fromUtf8("(?:\n\\s*[0-9]{1,2}|РАЗДЕЛ |Подраздел )")), aa.indexOf(" ")));

		while (ab.contains("  ")) {
		    ab = ab.replace("  ", " ");
		};

		query.bindValue(":addition", ab);
	    } else query.bindValue(":addition", "");

	    if (ab == okved_number + " " +id_str.replace("\n", "")) query.bindValue(":addition", "");

	    query.exec();

	    if (change_used_razdel)
	    {
		if (last_podrazdel!=0) {
		    used_razdel = last_podrazdel;
		} else used_razdel = last_razdel;
		change_used_razdel = false;
	    }

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
    update_db_date();
}

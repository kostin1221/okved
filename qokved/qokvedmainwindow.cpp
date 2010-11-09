#include "qokvedmainwindow.h"
#include "ui_qokvedmainwindow.h"

#include "odswriter.h"
#include "addokveddialog.h"

#include <QDebug>
#include <QFile>
#include <QClipboard>
#include <QTextTable>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

QOkvedMainWindow::QOkvedMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QOkvedMainWindow)
{
    ui->setupUi(this);
    ui->additionView->installEventFilter(this);

    qokved = new Libqokved(this);

    #ifdef Q_WS_WIN
        appDir = QDir::convertSeparators(QDir::homePath() + "/Application Data/qokved");
    #elif defined(Q_WS_X11)
        appDir = QDir::homePath() + "/.config/qokved/";
    #endif

    QDir::root().mkpath(appDir);

    QString db_path=QDir::convertSeparators(appDir + "qokved.db");
    if (!QFile(db_path).exists())
    {
        QString def_db_path = findExistPath(QStringList() << QDir::convertSeparators(QCoreApplication::applicationDirPath () + "/../share/qokved/templates/qokved.db.default")  << QDir::convertSeparators(QCoreApplication::applicationDirPath() + "/templates/qokved.db.default") );
        if (def_db_path.isNull())
        {
            QFile(def_db_path).copy(db_path);
        } else errorMessage(QString::fromUtf8("Не найдена база данных, будет создана новая!"));
    }

    qokved->setDbPath(db_path);

    razdels_update();

    QSettings settings("qokved", "qokved");
    ui->filterEdit->setText(settings.value("last_filter").toString());

    ui->razdelsView->setAcceptDrops(true);
    ui->razdelsView->viewport()->setAcceptDrops(true);
}

void QOkvedMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "event";
    event->accept();
}

void QOkvedMainWindow::createDbFromTxt()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                            QString::fromUtf8("Укажите путь к закону в txt"), QDir::homePath(), "Text Files (*.txt)");

    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString data = QString::fromUtf8(file.readAll().data());
        qokved->fill_db_from_zakon(data);
        razdels_update();
    }
}

void QOkvedMainWindow::errorMessage(QString message)
{
    QMessageBox::critical(this, "QOkved", message, QMessageBox::Ok);

}

QString QOkvedMainWindow::findExistPath(QStringList path_list)
{
    for (int i = 0; i < path_list.size(); ++i)
    {
        if ( QFile(path_list.at(i)).exists() ) return path_list.at(i);
    }

    return QString::null;
}

void QOkvedMainWindow::action_oocalc()
{
    if (ui->okvedsView->model()->rowCount()==0) return;
    OdsWriter *ods_writer = new OdsWriter(this);
    connect(ods_writer, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));

    QString template_path = findExistPath(QStringList() << QDir::convertSeparators(QCoreApplication::applicationDirPath () + "/templates/soffice.ods") << QDir::convertSeparators(QCoreApplication::applicationDirPath () + "/../share/qokved/templates/soffice.ods"));
    if (template_path.isEmpty() || template_path.isNull())
    {
        errorMessage(QString::fromUtf8("Невозможно найти путь к soffice.ods"));
        return;
    }
    ods_writer->open(template_path);

    QMap<QString, QString> table;

    QSqlTableModel *model =  static_cast<QSqlTableModel*>(ui->okvedsView->model());
    for(int i = 0; i < model->rowCount(); i++)
    {
        table.insert(model->data(model->index(i, 1)).toString(), model->data(model->index(i, 2)).toString());
    }

    ods_writer->writeTable(table);
    QString ods_temp = QString(QDir::tempPath()+"/%1").arg(QDateTime::currentDateTime().toTime_t()) + ".ods";
    ods_writer->save(ods_temp);
    ods_writer->startOO(ods_temp);
}

void QOkvedMainWindow::row_filter_update()
{
     QString filter = ui->filterEdit->text();
     QSqlTableModel *model =  static_cast<QSqlTableModel*>(ui->okvedsView->model());

     if (filter.contains(QRegExp(QString::fromUtf8("(?:[a-z]|[A-Z]|[а-я]|[А-Я])")))) {
         for(int i = 0; i < model->rowCount(); i++)
         {
             if (!model->data(model->index(i, 2)).toString().contains(filter, Qt::CaseInsensitive))
             {
                 ui->okvedsView->hideRow(i);
             } else  ui->okvedsView->showRow(i);
         }
     } else {
         for(int i = 0; i < model->rowCount(); i++)
         {
             if (!model->data(model->index(i, 1)).toString().startsWith(filter, Qt::CaseInsensitive))
             {
                 ui->okvedsView->hideRow(i);
             } else  ui->okvedsView->showRow(i);
         }
     }

}

void QOkvedMainWindow::razdels_update()
{
    ui->razdelsView->setModel(qokved->razdels_model());
    ui->razdelsView->hideColumn(0);

    ui->okvedsView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

  //  ui->okvedsView->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);

    QHeaderView* hw = ui->razdelsView->horizontalHeader();
    hw->setResizeMode(QHeaderView::Stretch );

    //currentRowChanged ( const QModelIndex & current, const QModelIndex & previous )
    connect(ui->razdelsView->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex & )),this,SLOT(razdels_row_changed()));
    ui->razdelsView->hideColumn(2);
    ui->razdelsView->hideColumn(3);

    ui->razdelsView->selectRow(0);
}

void QOkvedMainWindow::razdels_row_changed()
{
    int row = ui->razdelsView->selectionModel()->currentIndex().row();
    if (row > -1){
        QAbstractItemModel *model = ui->razdelsView->model();

        ui->okvedsView->setModel(qokved->okveds_model(model->data(model->index(row, 0)).toInt()));

        ui->okvedsView->hideColumn(0);
        ui->okvedsView->hideColumn(3);
        ui->okvedsView->hideColumn(4);

        ui->okvedsView->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);
        //  qDebug() << ui->razdelsView->model()->itemData(index).values()[0].toString();

        connect(ui->okvedsView->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex & )),this,SLOT(additionUpdate( )));
    }
}

void QOkvedMainWindow::additionUpdate()
{
    QAbstractItemModel *model = ui->okvedsView->model();
    int row = ui->okvedsView->selectionModel()->currentIndex().row();
    if (row > -1) {
        //model->data()
      QString text = model->data(model->index(row, 3)).toString();
      if (text.isEmpty()) text = QString::fromUtf8("Без описания");
      ui->additionView->setPlainText (text);
    }
}

bool QOkvedMainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        if (object == ui->additionView)
        {
            QString text = ui->additionView->toPlainText();
            if (text != m_addition_text) {

                QSqlTableModel *model =  static_cast<QSqlTableModel*>(ui->okvedsView->model());
                QModelIndex sel_mod = ui->okvedsView->selectionModel()->currentIndex();
                int row = sel_mod.row();

                model->setData(model->index(row, 3), text);
                ui->okvedsView->selectRow(row);
            }
        }
    }
    if (event->type() == QEvent::FocusIn)
    {
        if (object == ui->additionView)
        {
           m_addition_text = ui->additionView->toPlainText();

        }
    }
    return false;
}

void QOkvedMainWindow::action_copy_text()
{
    if (ui->okvedsView->model()->rowCount()==0) return;
    QSqlTableModel *model =  static_cast<QSqlTableModel*>(ui->okvedsView->model());
    QString buffer;
    for(int i = 0; i < model->rowCount(); i++)
    {
        buffer.append(model->data(model->index(i, 1)).toString() + " " + model->data(model->index(i, 2)).toString() + "\n");
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(buffer);
}

void QOkvedMainWindow::action_copy_table()
{
    if (ui->okvedsView->model()->rowCount()==0) return;
    QClipboard *clipboard = QApplication::clipboard();
    QString buffer;
    QMimeData *mime = new QMimeData();
    buffer.append("<table  cellpadding=\"4\" cellspacing=\"0\">");

    QSqlTableModel *model =  static_cast<QSqlTableModel*>(ui->okvedsView->model());
    buffer.append(QString::fromUtf8("<tr><td><b>Код по ОКВЭД</b></td><td>Наименование</td></tr>"));
    for(int i = 0; i < model->rowCount(); i++)
    {
        buffer.append("<tr><td>"+model->data(model->index(i, 1)).toString()+"</td>" + " " + "<td WIDTH=400>"+model->data(model->index(i, 2)).toString()+"</td></tr>");
    }
    buffer.append("</table>");

    mime->setHtml(buffer);
    clipboard->setMimeData(mime);

}

//Сдвигает позиции начиная прибавляет к rid ко всем позициям, начиная с row
void QOkvedMainWindow::freeRid(int row)
{
    QSqlTableModel *model = static_cast<QSqlTableModel*>(ui->razdelsView->model());
    for(int i = model->rowCount(); i >= row; i--)  //Сдвиг всех разделов для вставки подраздела
    {
        QSqlRecord rec = model->record(i);
        int rid = rec.value("rid").toInt();
        rec.setValue("rid", rid+1);
        model->setRecord(i, rec);
    }
}

void QOkvedMainWindow::razdelzTablePopup(const QPoint & pos)
{
    QMenu myMenu;
    QPoint globalPos = ui->razdelsView->mapToGlobal(pos);
    QModelIndex sel_mod = ui->razdelsView->selectionModel()->currentIndex();
    int row = sel_mod.row();
    QSqlTableModel *model =  static_cast<QSqlTableModel*>(ui->razdelsView->model());
    if (model->data(model->index(row, 3)).toString() == "0" )
        myMenu.addAction(QString::fromUtf8("Создать подраздел"));

    myMenu.addAction(QString::fromUtf8("Создать раздел"));
    myMenu.addAction(QString::fromUtf8("Удалить раздел"));

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->text() == QString::fromUtf8("Удалить раздел"))
        {
            if (row > -1)
            {
                //удаление всех оквэдов, относящихся к разделу
                if (row == 0) return; //Если "все разделы"
                QAbstractItemModel *model = ui->razdelsView->model();
                QSqlTableModel *model_okved = qokved->okveds_model(model->data(model->index(row, 0)).toInt());
                model_okved->removeRows(0, model_okved->rowCount());
                ui->razdelsView->selectRow(row-1);
                model->removeRow(row);//Удаление самого раздела

            }
        }
        if (selectedItem->text() == QString::fromUtf8("Создать раздел") || selectedItem->text() == QString::fromUtf8("Создать подраздел"))
        {
            bool ok;
            QString text = QInputDialog::getText(this, QString::fromUtf8("Создать (под)раздел"),
                                                 QString::fromUtf8("Название нового (под)раздела:"), QLineEdit::Normal, "", &ok);
            if (ok && !text.isEmpty())
            {
                QSqlRecord record;
                int dist_row = -1;
                QSqlField field("name", QVariant::String);
                field.setValue(text);
                record.append(field);
                QSqlField fieldf("father", QVariant::Int);
                fieldf.setValue(0);
                if (selectedItem->text() == QString::fromUtf8("Создать подраздел"))
                {
                    QSqlTableModel *model = static_cast<QSqlTableModel*>(ui->razdelsView->model());
                    freeRid(row+1);
                    QSqlField rid_field("rid", QVariant::Int);
                    rid_field.setValue(model->record(row).value("rid").toInt()+1);
                    record.append(rid_field);

                    fieldf.setValue(model->data(model->index(row, 0)).toInt());
                    dist_row=row+1;
                }

                record.append(fieldf);
                model->insertRecord(dist_row, record);
                int index_row=model->rowCount()-1;
                if (dist_row!=-1) index_row = dist_row;
                ui->razdelsView->selectRow(index_row);
            }
        }

    }
}

void QOkvedMainWindow::tablePopup(const QPoint & pos)
{

    QPoint globalPos = ui->okvedsView->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);
    QModelIndex sel_mod = ui->okvedsView->selectionModel()->currentIndex();
    int row = sel_mod.row();

    QMenu myMenu;
    myMenu.addAction(QString::fromUtf8("Добавить ОКВЭД"));
    myMenu.addAction(QString::fromUtf8("Удалить ОКВЭД"));
    // ...

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->text() == QString::fromUtf8("Удалить ОКВЭД"))
        {
            if (row > -1)
            {

                ui->okvedsView->model()->removeRow(row);
                int new_row;
                if (row == 0) {
                    new_row = 1;
                } else {
                    new_row = row -1;
                }
                ui->okvedsView->selectRow(new_row);
             }
        }
        if (selectedItem->text() == QString::fromUtf8("Добавить ОКВЭД"))
        {
                int row = ui->razdelsView->selectionModel()->currentIndex().row();
                AddOkvedDialog *dialog = new AddOkvedDialog(this);
                QSqlTableModel *model = static_cast<QSqlTableModel*>(ui->razdelsView->model());
                for(int i = 1; i < model->rowCount()-1; i++)
                {
                    dialog->addRazdel(model->data(model->index(i, 1)).toString(), model->data(model->index(i, 0)).toString());
                }
                if (row > 0)
                    dialog->setActiveRazdel(model->data(model->index(row, 0)).toString());
                connect(dialog, SIGNAL(addNewOkved(QString, QString, QString, QString)), this, SLOT(addNewOkved(QString, QString, QString, QString)));
                dialog->exec();
        }
    }
}


void QOkvedMainWindow::addNewOkved(QString rid, QString number, QString name, QString caption)
{
    QSqlTableModel *model =  static_cast<QSqlTableModel*>(ui->okvedsView->model());
    //(\"oid\" INTEGER PRIMARY KEY, \"number\" TEXT, \"name\" TEXT, \"addition\" TEXT, \"razdel_id\" INTEGER )"

    QSqlRecord record;
    QSqlField field_name("name", QVariant::String);
    field_name.setValue(name);
    record.append(field_name);

    QSqlField field_num("number", QVariant::String);
    field_num.setValue(number);
    record.append(field_num);

    QSqlField field_cap("addition", QVariant::String);
    field_cap.setValue(caption);
    record.append(field_cap);

    QSqlField field_rid("razdel_id", QVariant::Int);
    field_rid.setValue(rid.toInt());
    record.append(field_rid);

    model->insertRecord(-1, record);

}

QOkvedMainWindow::~QOkvedMainWindow()
{
    QSettings settings("qokved", "qokved");
    settings.setValue("last_filter", ui->filterEdit->text());

    delete ui;
    delete qokved;
}

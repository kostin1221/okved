#include "qokvedmainwindow.h"
#include "ui_qokvedmainwindow.h"

#include "odtwriter.h"

#include <QDebug>
#include <QFile>
#include <QClipboard>
#include <QTextTable>

QOkvedMainWindow::QOkvedMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QOkvedMainWindow)
{
    ui->setupUi(this);
    ui->additionView->installEventFilter(this);

    qokved = new Libqokved();

    qokved->setDbPath(QDir::currentPath() + "/okved.db");


    QFile file(QDir::homePath () + "/okved.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    #ifdef Q_WS_WIN
        appDir = QDir::homePath() + "/Application Data/qokved";
    #elif defined(Q_WS_X11)
        appDir = QDir::homePath() + "/.config/qokved/";
    #endif

    //QString data = QString::fromUtf8(file.readAll().data());
 //   qokved->fill_db_from_zakon(data);


/*    QList<Razdel> razdel_list = qokved->razdels_list();

    for (int i = 0; i < razdel_list.size(); ++i) {
        Razdel razdel = razdel_list.at(i);
        ui->razdelsView->addItem(razdel.name);

    }
*/
    razdels_update();

}

void QOkvedMainWindow::action_oocalc()
{
    OdtWriter *odt_writer = new OdtWriter(this);
    odt_writer->open(QDir::currentPath() + "/123.ods");

    QMap<QString, QString> table;

    QSqlTableModel *model =  static_cast<QSqlTableModel*>(ui->okvedsView->model());
    QString buffer;
    for(int i = 0; i < model->rowCount(); i++)
    {
        table.insert(model->data(model->index(i, 1)).toString(), model->data(model->index(i, 2)).toString());
    }


    odt_writer->writeTable(table);
    odt_writer->save(QDir::homePath() + "/test.ods");
    odt_writer->startOO(QDir::homePath() + "/test.ods");

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
//    connect(ui->okvedsView->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex & )),this,SLOT(okved_row_changed(const QModelIndex &, const QModelIndex & )));

   // ui->razdelsView->setColumnWidth ( 0,  ui->razdelsView->width() );
   // ui->razdelsView->setColumnWidth ( 1,  ui->razdelsView->width() );
}


void QOkvedMainWindow::razdels_row_changed()
{
    int row = ui->razdelsView->selectionModel()->currentIndex().row();
    if (row > -1){
        QAbstractItemModel *model = ui->razdelsView->model();

        ui->okvedsView->setModel(qokved->okveds_model(model->data(model->index(row, 0)).toInt(), ui->filterEdit->text()));

        ui->okvedsView->hideColumn(0);
       // ui->okvedsView->hideColumn(2);
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
                ui->okvedsView->selectionModel()->setCurrentIndex(sel_mod, QItemSelectionModel::SelectCurrent);
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

void QOkvedMainWindow::tablePopup(const QPoint & pos)
{
    // for most widgets
    QTableView* table_view = static_cast<QTableView*>(sender());
    QPoint globalPos = table_view->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction(QString::fromUtf8("Добавить позицию"));
    myMenu.addAction(QString::fromUtf8("Удалить позицию"));
    // ...

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->text() == QString::fromUtf8("Удалить позицию"))
        {
            if (table_view->objectName() == "razdelsView")
            {
                int row = ui->razdelsView->selectionModel()->currentIndex().row();
                if (row > -1){
                    QAbstractItemModel *model = ui->razdelsView->model();
                    QSqlTableModel *model_okved = qokved->okveds_model(model->data(model->index(row, 0)).toInt(), "");
                    qDebug() <<  model_okved->rowCount();
                 //   for(int i = 0; i < model_okved->rowCount(); i++)
                  //  {
                        model_okved->removeRows(0, model_okved->rowCount());
                  //  }
                    model_okved->submitAll();
                    qDebug() <<  model_okved->rowCount();
                qDebug() << "123";
                }
            }
//            QModelIndex sel_mod = table_view->selectionModel()->currentIndex();
//            int row = sel_mod.row();
//            table_view->model()->removeRow(row);
        }

    }
    else
    {
        // nothing was chosen
    }
//    qDebug() << "show popup " << pos;
}


QOkvedMainWindow::~QOkvedMainWindow()
{
    delete ui;
    delete qokved;
}

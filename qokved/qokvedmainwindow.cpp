#include "qokvedmainwindow.h"
#include "ui_qokvedmainwindow.h"

#include <QDebug>
#include <QFile>

QOkvedMainWindow::QOkvedMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QOkvedMainWindow)
{
    ui->setupUi(this);

    qokved = new Libqokved();
qDebug() <<QDir::homePath ();
    qokved->setDbPath(QDir::homePath () + "/okved.db");

    qokved->fill_db_from_zakon(QDir::homePath () + "/okved2.txt");

/*    QList<Razdel> razdel_list = qokved->razdels_list();

    for (int i = 0; i < razdel_list.size(); ++i) {
        Razdel razdel = razdel_list.at(i);
        ui->razdelsView->addItem(razdel.name);

    }
*/

    ui->razdelsView->setModel(qokved->razdels_model());
    ui->razdelsView->hideColumn(0);

    ui->okvedsView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QHeaderView* hw = ui->razdelsView->horizontalHeader();
    hw->setResizeMode(QHeaderView::Stretch );

    //currentRowChanged ( const QModelIndex & current, const QModelIndex & previous )
    connect(ui->razdelsView->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex & )),this,SLOT(okved_list_update(const QModelIndex &)));

   // ui->razdelsView->setColumnWidth ( 0,  ui->razdelsView->width() );
   // ui->razdelsView->setColumnWidth ( 1,  ui->razdelsView->width() );

}

void QOkvedMainWindow::okved_list_update(const QModelIndex index)
{
    QAbstractItemModel *model = ui->razdelsView->model();

    ui->okvedsView->setModel(qokved->okveds_model(model->itemData(model->index(index.row(), 0)).values()[0].toInt()));

    ui->okvedsView->hideColumn(0);
   // ui->okvedsView->hideColumn(2);
   // ui->okvedsView->hideColumn(3);
    //ui->okvedsView->hideColumn(4);
    //  qDebug() << ui->razdelsView->model()->itemData(index).values()[0].toString();

}

QOkvedMainWindow::~QOkvedMainWindow()
{
    delete ui;
    delete qokved;
}

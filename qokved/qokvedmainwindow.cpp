#include "qokvedmainwindow.h"
#include "ui_qokvedmainwindow.h"

#include <QDebug>
#include <QFile>

QOkvedMainWindow::QOkvedMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QOkvedMainWindow)
{
    ui->setupUi(this);

    //ui->razdelsView->setColumnWidth ( 0,  ui->razdelsView->width() );

    qokved = new Libqokved();

    qokved->setDbPath("/home/BALTECH.LTD/admin/okved.db");

   // qokved->fill_db_from_zakon("/home/BALTECH.LTD/admin/okved2.txt");

    QList<Razdel> razdel_list = qokved->razdels_list();

    for (int i = 0; i < razdel_list.size(); ++i) {
        Razdel razdel = razdel_list.at(i);
        ui->razdelsView->addItem(razdel.name);


    }

 /*
    //qDebug() << QString::fromUtf8("05f").toInt();*/

}

QOkvedMainWindow::~QOkvedMainWindow()
{
    delete ui;
    delete qokved;
}

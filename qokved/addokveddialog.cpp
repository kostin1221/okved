#include "addokveddialog.h"
#include "ui_addokveddialog.h"
#include <QDebug>

AddOkvedDialog::AddOkvedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOkvedDialog)
{
    ui->setupUi(this);
}

AddOkvedDialog::~AddOkvedDialog()
{
    delete ui;
}

void AddOkvedDialog::accept()
{

    emit addNewOkved(ui->comboRazdels->itemData(ui->comboRazdels->currentIndex()).toString(), ui->lineNumber->text(), ui->lineName->text(), ui->plainCaption->toPlainText ());
    close();
}

void AddOkvedDialog::addRazdel(QString name, QString rid)
{
    ui->comboRazdels->addItem(name, rid);
}

void AddOkvedDialog::setActiveRazdel(QString rid)
{
    ui->comboRazdels->setCurrentIndex(ui->comboRazdels->findData(rid));
}

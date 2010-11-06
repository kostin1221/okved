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
    qDebug() << "accept";

}

void AddOkvedDialog::addRazdel(QString name, QString rid)
{
    ui->comboRazdels->addItem(name, rid);
}

void AddOkvedDialog::setActiveRazdel(QString rid)
{
    ui->comboRazdels->setCurrentIndex(ui->comboRazdels->findData(rid));
}

void AddOkvedDialog::changeRazdelButtonsClicked()
{
    ui->comboRazdels->setEnabled(ui->oldRazdelButton->isChecked ());
    ui->newRazdelEdit->setEnabled(ui->newRazdelButton->isChecked ());
}

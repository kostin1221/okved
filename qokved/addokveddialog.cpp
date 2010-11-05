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


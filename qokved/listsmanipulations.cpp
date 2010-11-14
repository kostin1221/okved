#include "listsmanipulations.h"
#include "ui_listsmanipulations.h"

listsManipulations::listsManipulations(Libqokved* libokved, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::listsManipulations)
{
    ui->setupUi(this);

    qokved = libokved;
}

listsManipulations::~listsManipulations()
{
    delete ui;
}

void listsManipulations::cancel()
{
    close();
}

void listsManipulations::save_list()
{
    //qokved->save_global_list();
    close();
}

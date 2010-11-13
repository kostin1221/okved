#include "listsmanipulations.h"
#include "ui_listsmanipulations.h"

listsManipulations::listsManipulations(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::listsManipulations)
{
    ui->setupUi(this);
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
    close();
}

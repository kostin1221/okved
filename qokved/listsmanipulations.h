#ifndef LISTSMANIPULATIONS_H
#define LISTSMANIPULATIONS_H

#include <QDialog>
#include "libqokved.h"


namespace Ui {
    class listsManipulations;
}

class listsManipulations : public QDialog
{
    Q_OBJECT

public:
    explicit listsManipulations(Libqokved* libokved, QWidget *parent = 0);
    ~listsManipulations();

private:
    Ui::listsManipulations *ui;
    Libqokved* qokved;

public slots:
    void cancel();
    void save_list();
};

#endif // LISTSMANIPULATIONS_H

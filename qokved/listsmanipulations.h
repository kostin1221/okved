#ifndef LISTSMANIPULATIONS_H
#define LISTSMANIPULATIONS_H

#include <QDialog>

namespace Ui {
    class listsManipulations;
}

class listsManipulations : public QDialog
{
    Q_OBJECT

public:
    explicit listsManipulations(QWidget *parent = 0);
    ~listsManipulations();

private:
    Ui::listsManipulations *ui;

public slots:
    void cancel();
    void save_list();
};

#endif // LISTSMANIPULATIONS_H

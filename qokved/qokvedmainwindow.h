#ifndef QOKVEDMAINWINDOW_H
#define QOKVEDMAINWINDOW_H

#include <QMainWindow>
#include "libqokved.h"

namespace Ui {
    class QOkvedMainWindow;
}

class QOkvedMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QOkvedMainWindow(QWidget *parent = 0);
    ~QOkvedMainWindow();

private:
    Ui::QOkvedMainWindow *ui;
    Libqokved* qokved;

public slots:
    void okved_list_update(QModelIndex index);
};

#endif // QOKVEDMAINWINDOW_H

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
    void okved_list_update(const QModelIndex index);
    void okved_row_changed(const QModelIndex index, const QModelIndex old);
};

#endif // QOKVEDMAINWINDOW_H

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
    bool eventFilter(QObject *object, QEvent *event);

    QString m_addition_text;
    QString appDir;

public slots:
    void razdels_row_changed();
    void additionUpdate();
    void razdels_update();

    void action_copy_text();
    void action_copy_table();
    void action_oocalc();
    void tablePopup(const QPoint & pos);
};

#endif // QOKVEDMAINWINDOW_H

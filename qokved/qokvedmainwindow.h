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

    void freeRid(int rid);

public slots:
    void razdels_row_changed();
    void additionUpdate();
    void razdels_update();
    void row_filter_update();
    void createDbFromTxt();

    void action_copy_text();
    void action_copy_table();
    void action_oocalc();
    void tablePopup(const QPoint & pos);
    void razdelzTablePopup(const QPoint & pos);
    void errorMessage(QString message);

    void addNewOkved(QString rid, QString number, QString name, QString caption);

    void dragEnterEvent(QDragEnterEvent *event);

};

#endif // QOKVEDMAINWINDOW_H

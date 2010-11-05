#ifndef ADDOKVEDDIALOG_H
#define ADDOKVEDDIALOG_H

#include <QDialog>

namespace Ui {
    class AddOkvedDialog;
}

class AddOkvedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOkvedDialog(QWidget *parent = 0);
    ~AddOkvedDialog();

private:
    Ui::AddOkvedDialog *ui;

public slots:
    void accept();
};

#endif // ADDOKVEDDIALOG_H

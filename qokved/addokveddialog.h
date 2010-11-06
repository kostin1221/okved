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

    void addRazdel(QString name, QString rid);
    void setActiveRazdel(QString rid);

private:
    Ui::AddOkvedDialog *ui;

public slots:
    void accept();
    void changeRazdelButtonsClicked();
};

#endif // ADDOKVEDDIALOG_H

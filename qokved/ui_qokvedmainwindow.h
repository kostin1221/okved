/********************************************************************************
** Form generated from reading UI file 'qokvedmainwindow.ui'
**
** Created
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QOKVEDMAINWINDOW_H
#define UI_QOKVEDMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QOkvedMainWindow
{
public:
    QWidget *centralWidget;
    QListWidget *razdelsView;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QOkvedMainWindow)
    {
        if (QOkvedMainWindow->objectName().isEmpty())
            QOkvedMainWindow->setObjectName(QString::fromUtf8("QOkvedMainWindow"));
        QOkvedMainWindow->resize(681, 486);
        centralWidget = new QWidget(QOkvedMainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        razdelsView = new QListWidget(centralWidget);
        razdelsView->setObjectName(QString::fromUtf8("razdelsView"));
        razdelsView->setGeometry(QRect(0, 0, 301, 421));
        QOkvedMainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QOkvedMainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 681, 22));
        QOkvedMainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QOkvedMainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QOkvedMainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QOkvedMainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QOkvedMainWindow->setStatusBar(statusBar);

        retranslateUi(QOkvedMainWindow);

        QMetaObject::connectSlotsByName(QOkvedMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *QOkvedMainWindow)
    {
        QOkvedMainWindow->setWindowTitle(QApplication::translate("QOkvedMainWindow", "QOkved", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QOkvedMainWindow: public Ui_QOkvedMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QOKVEDMAINWINDOW_H

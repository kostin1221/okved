#include "odtwriter.h"
#include <QDir>
#include <QProcess>
#include <QDateTime>
#include <QDebug>
#include <QTextOStream>
#include <QMessageBox>


OdtWriter::OdtWriter(QObject *parent) :
    QObject(parent)
{
}

bool OdtWriter::open( const QString &fname )
{
        QDir dir;
        QString temp;

        QString templateDir = QDir::currentPath();
        temp = QDir::tempPath ();

        copyName = QString(temp+"/%1").arg(QDateTime::currentDateTime().toTime_t());
        copyName = QDir::convertSeparators(copyName);

        if(!dir.mkdir(copyName))
        {
            int ret = QMessageBox::error(this, "QOkved",
                                           QString::fromUtf8("Невозможно создать временный каталог"),
                                           QMessageBox::Ok);
                return false;
        }

        QProcess *process = new QProcess(this);
        process->setWorkingDirectory ( templateDir);

#ifndef Q_OS_WIN32
        process->start(QString("unzip"), QStringList() << fname << "-d" << copyName);

#else
        process->start(QString("7z"), QStringList() << "x" << "-y" << QString("-o%1").arg(copyName) << fname);
#endif
        if (!process->waitForFinished())
        {
                qDebug() <<  tr("unzip dead");
                return false;
        }

        if( process->exitStatus() != QProcess::NormalExit )
        {
                qDebug() <<  tr("unzip dead");
                return false;
        }
        else
        {
                qDebug() <<  tr("unzip normal");
        }

return true;
}

bool OdtWriter::writeTable(QMap<QString, QString> table)
{
    QString end_of_row = "</table:table-row>";
    QString end_of_style = "</style:style>";


    QFile file (QDir::convertSeparators( copyName+"/content.xml") );
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = QString::fromUtf8(file.readAll().data());
    file.close();

    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);

    QMapIterator<QString, QString> i(table);
    QString row;
    while (i.hasNext()) {
        i.next();
        row.append(QString("<table:table-row table:style-name=\"ro1\"><table:table-cell table:style-name=\"ce2\" office:value-type=\"string\"><text:p>%1</text:p></table:table-cell><table:table-cell table:style-name=\"ce2\" office:value-type=\"string\"><text:p>%2</text:p></table:table-cell></table:table-row>").arg(i.key()).arg(i.value()));
        qDebug() << i.key() << ": " << i.value() << endl;
    }

    data.insert(data.indexOf(end_of_style) + end_of_style.count(), "<style:style style:name=\"ce2\" style:family=\"table-cell\" style:parent-style-name=\"Default\"><style:text-properties fo:font-weight=\"normal\" style:font-weight-asian=\"normal\" style:font-weight-complex=\"normal\"/></style:style>");
    data.insert(data.indexOf(end_of_row) + end_of_row.count(), row);

    QTextStream out(&file);
    out << data;
    return true;

}

bool OdtWriter::save( const QString & fname )
{
        QProcess *process = new QProcess(this);
        process->setWorkingDirectory(copyName);

#ifndef Q_OS_WIN32
        process->start(QString("zip"), QStringList() << "-r" << fname << ".");

#else
        process->start(QString("7z"), QStringList() << "a" << "-tzip" << fname << "-r" << .);
#endif
        if (!process->waitForFinished())
        {
                qDebug() <<  tr("zip dead");
                return false;
        }

        return true;
}

bool OdtWriter::startOO( const QString & fname )
{

    QProcess *process = new QProcess(this);

    process->start(QString("ooffice"), QStringList() << fname);
    return true;
}


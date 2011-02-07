#ifndef ODTWRITER_H
#define ODTWRITER_H

#include <QObject>
#include <QMap>
#include <QStringList>

class OdsWriter : public QObject
{
    Q_OBJECT
public:
    explicit OdsWriter(QObject *parent = 0);
    ~OdsWriter();
    bool open( const QString &fname );
    bool writeTable(QMap<QString, QString> table);
    bool save( const QString & fname );
    bool startOO( const QString & fname );

private:
    QString copyName;
    bool removeDir(const QString &dirName);
    QStringList ods_list;

signals:
    void errorMessage(QString error);

private slots:

};

#endif // ODTWRITER_H

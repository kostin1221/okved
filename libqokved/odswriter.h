#ifndef ODTWRITER_H
#define ODTWRITER_H

#include <QObject>
#include <QMap>

class OdsWriter : public QObject
{
    Q_OBJECT
public:
    explicit OdsWriter(QObject *parent = 0);
    bool open( const QString &fname );
    bool writeTable(QMap<QString, QString> table);
    bool save( const QString & fname );
    bool startOO( const QString & fname );

private:
    QString copyName;
    bool removeDir(const QString &dirName);

signals:
    void errorMessage(QString error);

private slots:
    void soffice_finished( int exitCode );

};

#endif // ODTWRITER_H

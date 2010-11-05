#ifndef ODTWRITER_H
#define ODTWRITER_H

#include <QObject>
#include <QMap>

class OdtWriter : public QObject
{
    Q_OBJECT
public:
    explicit OdtWriter(QObject *parent = 0);
    bool open( const QString &fname );
    bool writeTable(QMap<QString, QString> table);
    bool save( const QString & fname );
    bool startOO( const QString & fname );

private:
    QString copyName;
signals:

public slots:

};

#endif // ODTWRITER_H

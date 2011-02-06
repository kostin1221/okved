#include "odswriter.h"
#include <QDir>
#include <QDirIterator>
#include <QProcess>
#include <QDateTime>
#include <QDebug>
#include <QTextOStream>
#include <QMessageBox>

#include "qzip/qzipreader_p.h"
#include "qzip/qzipwriter_p.h"

/*
The Bitter Bitty flag

The first versions of the UnZip class used to first parse the local header records to retrieve the archive contents. However this introduced some problems with entries that have the third general purpose bit flag set. The original purpose of this flag is to allow to write the CRC32, compressed and uncompressed size after the compressed data in a so-called "data descriptor" record. This was necessary because some devices (maybe some ice-age tape drive) might not be able to seek back and set the three fields in the local header record. Locating the data descriptor is however a big issue since we need to skip the compressed data without knowing its length. And btw.: the data descriptor has an OPTIONAL signature.

One day one of you guys out there realized that the class failed in parsing OpenOffice archives (plain PKZIP 2.0 zip files!). After a short coffeine powered debug session, I realized that the problem was related to my beloved 3rd general purpose bit flag (friends call him Bitty, but his full name is Bitter Bitty - ok, I will quit it with these poor geek jokes). OpenOffice sets the Bitty flag for empty files (or under some other circumstance.. I can't remember now) but the routine I wrote to locate the data descriptor was slow and buggy (very slow; and very buggy).

This was a good reason for me to write the whole archive parsing routines back from scratch. The latest versions start parsing the central directory records (at the end of the zip file) instead of parsing the local header entries. This allows me to always retrieve CRC32, compressed and uncompressed size, besides of the offset of the local header entry for each file. I suppose most zip routines use this second approach (at least some open source ones do) as it is quite faster. However, this means that incomplete zip files (files with a corrupted or missing end) won't be parsed, because I didn't add any routines to handle these files by attempting to parse the local header.

Btw. the local headers are still read to do some basic redundancy checks and detect if the values in the central directory differ from the ones in the local header.

Самый простой вариант это пофиксить - распаковал ods файл и запаковал обратно без этого флага. Благо темплейт всегда поставляется вместе с программой.
Можно юзать фнешнюю консольную программу, но это ад для поддержки на разных ОС.
  */

OdsWriter::OdsWriter(QObject *parent) :
    QObject(parent)
{
}

bool OdsWriter::open( const QString &fname )
{
        QDir dir;
        QString temp;

        QString templateDir = QDir::currentPath();
        temp = QDir::tempPath ();

	copyName = QDir::convertSeparators(QString(temp+"/%1").arg(QDateTime::currentDateTime().toTime_t())+'/');

        if(!dir.mkdir(copyName))
        {
            errorMessage(QString::fromUtf8("Невозможно создать временный каталог"));
            return false;
        }

	//Вариант с использованием консольной версии unzip. Абсолютно неудобно мейнтерить.

      /*  QProcess *process = new QProcess(this);
	process->setWorkingDirectory ( templateDir);

	process->start(QString("unzip"), QStringList() << fname << "-d" << copyName);

     (!process->waitForFinished())
        {
            qDebug() <<  tr("unzip dead");
            return false;
        }

        if( process->exitStatus() != QProcess::NormalExit )
        {
            errorMessage(QString::fromUtf8("Ошибка запуска unzip, проверьте что он установлен и присутствует в PATH"));
            return false;
	}*/
	///////////////////////////////////////////////////////////////////////////////////////////

	QZipReader zip_reader(fname);
	if (zip_reader.exists()) {
	    // распаковка архива по указанному пути
	    zip_reader.extractAll(copyName);

	}

return true;
}

bool OdsWriter::writeTable(QMap<QString, QString> table)
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
    }

    data.insert(data.indexOf(end_of_style) + end_of_style.count(), "<style:style style:name=\"ce2\" style:family=\"table-cell\" style:parent-style-name=\"Default\"><style:text-properties fo:font-weight=\"normal\" style:font-weight-asian=\"normal\" style:font-weight-complex=\"normal\"/></style:style>");
    data.insert(data.indexOf(end_of_row) + end_of_row.count(), row);

    QTextStream out(&file);
    out << data;
    return true;
}

bool OdsWriter::save( const QString & fname )
{
    //Вариант с использованием консольной версии zip. Абсолютно неудобно мейнтерить.
    /*    QProcess *process = new QProcess(this);
        process->setWorkingDirectory(copyName);

//#ifndef Q_OS_WIN32
        process->start(QString("zip"), QStringList() << "-r" << fname << ".");

//#else
 //       process->start(QString("7z"), QStringList() << "a" << "-tzip" << fname << "-r" << .);
//#endif
        if (!process->waitForFinished())
        {
                qDebug() <<  tr("zip dead");
                return false;
        }

        if( process->exitStatus() != QProcess::NormalExit )
        {
            errorMessage(QString::fromUtf8("Ошибка запуска zip, проверьте что он установлен и присутствует в PATH"));
            return false;
	}*/
	////////////////////////////////////////////////////////////////////////////////////////////////

    QZipWriter zip(fname);
    if (zip.status() != QZipWriter::NoError)
	return false;

    zip.setCompressionPolicy(QZipWriter::AutoCompress);

    QDirIterator it(copyName, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
		    QDirIterator::Subdirectories);
    while (it.hasNext()) {
	QString file_path = it.next();
	if (it.fileInfo().isDir()) {
	    zip.setCreationPermissions(QFile::permissions(file_path));
	    zip.addDirectory(file_path.remove(copyName));
	} else if (it.fileInfo().isFile()) {
	    QFile file(file_path);
	    if (!file.open(QIODevice::ReadOnly))
		continue;

	    zip.setCreationPermissions(QFile::permissions(file_path));
	    QByteArray ba = file.readAll();
	    zip.addFile(file_path.remove(copyName), ba);
	}
    }
    zip.close();

    QDir dir(copyName);
    qDebug() << "rm: " << copyName<< removeDir(copyName);

    return true;
}

bool OdsWriter::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
	Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
	    if (info.isDir()) {
		result = removeDir(info.absoluteFilePath());
	    }
	    else {
		result = QFile::remove(info.absoluteFilePath());
	    }

	    if (!result) {
		return result;
	    }
	}
	result = dir.rmdir(dirName);
    }

    return result;
}

void OdsWriter::soffice_finished( int exitCode ){

    qDebug() << "exit";
}

bool OdsWriter::startOO( const QString & fname )
{

    QProcess *process = new QProcess(this);

    process->start(QString("soffice"), QStringList() << fname);
    if( process->exitStatus() != QProcess::NormalExit )
    {
        errorMessage(QString::fromUtf8("Ошибка запуска OpenOffice, проверьте что он установлен и ooffice присутствует в PATH"));
        return false;
    }
    connect(process,SIGNAL(finished(int)),this,SLOT(soffice_finished(int)));
    return true;
}



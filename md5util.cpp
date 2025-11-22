#include "md5util.h"
#include <QCryptographicHash>

QByteArray GetMD5(const QString& str)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(str.toUtf8());
    return hash.result();
}


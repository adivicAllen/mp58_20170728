#ifndef PARSING_H
#define PARSING_H

#include "parsing_global.h"
#include <QSettings>

class PARSINGSHARED_EXPORT Parsing
{

public:
    Parsing();
    QSettings *qsettings;
    QString    filePathName;
    QVariant   getIniValue(QString SectionName, QString FieldName);


};

extern "C" PARSINGSHARED_EXPORT QVariant *parseSettings(QString filePathName, QString sectionName, QString fieldName);

#endif // PARSING_H

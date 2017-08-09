#include "libtest.h"
#include <iostream>
#include <QSettings>
#include <QVariant>
#include <QString>

Libtest::Libtest()
{
     std::cout << "test show lib test dll" << std::endl;
}

void showqt()
{
     std::cout<< "test show qt" << std::endl;
}

void Libtest::t1()
{
     std::cout<< "t1 :: test show qt" << std::endl;
}

QVariant parseSetting(QString filePathName, QString sectionName, QString fieldName)
{
    QSettings settings (filePathName, QSettings::IniFormat)  ;
    QString settingKey =QString("%1/%2").arg(sectionName).arg(fieldName);
    //QVariant qret =settings.value(settingKey );
    //return qret;
    return settings.value(settingKey );
}


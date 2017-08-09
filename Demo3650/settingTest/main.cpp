#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
 //   QSettings settings("/home/allen/mp58_20170728/Demo3650/InitConfigFile.ini",  QSettings::IniFormat);
   /*
    QSettings settings("../InitConfigFile.ini",  QSettings::IniFormat);
    QSettings settings("/home/allen/mp58_20170728/Demo3650/InitConfigFile.ini",  QSettings::IniFormat);
    QString ret = settings.value("Server Config Setting/Server_1_IP", "").toString();
    qDebug()<< ret << endl;
    */

    QSettings *settings = 0;
    settings = new QSettings("InitConfigFile.ini",  QSettings::IniFormat);
     QString ret = settings -> value("Server Config Setting/Server_1_IP", "").toString();

    /*
     QSettings settings;
     settings.setPath( QSettings::IniFormat, QSettings::UserScope ,"/home/allen/mp58_20170728/Demo3650/InitConfigFile.ini" );
     settings.status();
     QString ret = settings . value("Server Config Setting/Server_1_IP", "").toString();
     */

    return a.exec();
}


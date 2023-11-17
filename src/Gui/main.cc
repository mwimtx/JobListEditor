#include "mainwindow.h"
#include <QApplication>
#include <QFile>
//#include <QCleanlooksStyle>
//#include <QMotifStyle>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QString>

int main(int argc, char *argv[])
{
    QString qstrTemp;

    qstrTemp = SVN_REVISION_STRING;
    qstrTemp = qstrTemp.right   (qstrTemp.size () - qstrTemp.indexOf (' '));
    qstrTemp = qstrTemp.left    (qstrTemp.indexOf ('$'));
    qstrTemp = qstrTemp.trimmed ();

    BuildManifest::VersionString += qstrTemp.toStdString();

    qDebug () << qstrTemp.toStdString().c_str ();
    qDebug () << BuildManifest::VersionString.c_str ();

    QApplication application(argc, argv);
    application.setApplicationName( QString( BuildManifest::ApplicationName.c_str() ) );
    application.setApplicationVersion( "Version : " + QString( BuildManifest::VersionString.c_str() ) +
                                       " build date : " + QString( BuildManifest::BuildDate.c_str() ) +
                                       " build time : " + QString( BuildManifest::BuildTime.c_str() ) );

    qDebug()  << "********************************************************************************";
    qDebug()  << "* Application name : " << application.applicationName();
    qDebug()  << "* Version          : " << application.applicationVersion();
    qDebug()  << "* Compilerversion  : " << BuildManifest::CompilerVersionString.c_str();
    qDebug()  << "********************************************************************************";

    QCoreApplication::setOrganizationName("Metronix");
    QCoreApplication::setOrganizationDomain("metronix.de");
    QCoreApplication::setApplicationName("JobListEditor");

    Q_INIT_RESOURCE(jle_resources);

    QStringList args = application.arguments();

    QString jobFileName;
    bool bigstyle = false;

    if (args.contains("-bigstyle")) {
        bigstyle = true;
        args.removeAll("-bigstyle");
    }
    if ( args.length() == 2 )
        jobFileName = args[1];

    //application.setStyle( new QMotifStyle() ); // Ownership of the style object is transferred to QApplication
    // application.setStyle( new QCleanlooksStyle() ); // Ownership of the style object is transferred to QApplication

    QFile style;
    if (bigstyle) {
        style.setFileName(":/styles/Resources/bigstyle.style");
    }
    else {
        style.setFileName(":/styles/Resources/main.style");
    }
    style.open(QIODevice::ReadOnly);
    QString styleSheetName = style.readAll();
    if ( styleSheetName.isEmpty() )
        qDebug() << "stylesheet " << styleSheetName << " not found, using defaults.";
    application.setStyleSheet(styleSheetName);
    style.close();


    MainWindow* pclMainWindow;
    pclMainWindow = new MainWindow ();
    pclMainWindow->show();
    if (not jobFileName.isEmpty())
    {
        pclMainWindow->setAndLoadJobListFile (jobFileName);
    }
    
    return application.exec();
}

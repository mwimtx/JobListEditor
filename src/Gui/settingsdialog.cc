#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>

SettingsDialog::SettingsDialog( DataModel& dataModel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    mDataModel( dataModel )
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setup()
{
    updateGui();
}

void SettingsDialog::updateGui()
{

    bool haveAllData = 
        ( not mHwConfigFileName.isEmpty() ) and 
        ( not mJobFileSubdirectoryName.isEmpty() ) and
        ( not mJoblistDirectoryName.isEmpty() );
    
    ui -> pbAccept -> setEnabled( haveAllData );

}


void SettingsDialog::accept()
{
    qDebug() << __PRETTY_FUNCTION__ ;
    QDialog::accept();
}

void SettingsDialog::handlePbJobListDir()
{
    QFileDialog fd;
    fd.setFileMode( QFileDialog::Directory );
    fd.setOptions( QFileDialog::ShowDirsOnly );
    if (not (fd.exec() == QDialog::Accepted))
    {
        return;
    }
    QStringList result = fd.selectedFiles();
    if ( result.size() != 1 )
        return;
    mJoblistDirectoryName = result[0];
    ui -> lbJoblistDirectoryName -> setText( mJoblistDirectoryName );
    updateGui();
}

void SettingsDialog::handlePbHWConfigFile()
{
    QFileDialog fd;
    fd.setNameFilter(tr("Hardware configuration templates (*.xml)"));
    fd.setFileMode( QFileDialog::ExistingFile );
    if (not (fd.exec() == QDialog::Accepted))
        return;
    QStringList result = fd.selectedFiles();
    if ( result.size() != 1 )
        return;
    mHwConfigFileName = result[0];
    ui -> lbHwConfigFileName -> setText( mHwConfigFileName );
    updateGui();

}
    
void SettingsDialog::handlePbJobFileSubDir()
{
    QFileDialog fd;
    fd.setFileMode( QFileDialog::Directory );
    fd.setOptions ( QFileDialog::ShowDirsOnly );
    if (not (fd.exec() == QDialog::Accepted))
        return;
    QStringList result = fd.selectedFiles();
    if ( result.size() != 1 )
        return;
    mJobFileSubdirectoryName = result[0];
    ui -> lbJobFileSubdirectoryName -> setText( mJobFileSubdirectoryName );
    updateGui();

}


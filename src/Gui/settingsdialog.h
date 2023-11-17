#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H



#include <QDialog>
#include "datamodel.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
        Q_OBJECT
        
    public:
    explicit SettingsDialog( DataModel& dataModel , QWidget *parent = 0);
    ~SettingsDialog();
        
    void setup();

public slots:

    virtual void accept();
        
    void handlePbJobListDir(); 
    void handlePbHWConfigFile();
    void handlePbJobFileSubDir();

private:

    void updateGui();

    Ui::SettingsDialog *ui;
    DataModel& mDataModel;

    QString mHwConfigFileName;
    QString mJobFileSubdirectoryName;
    QString mJoblistDirectoryName;

};

#endif // SETTINGSDIALOG_H

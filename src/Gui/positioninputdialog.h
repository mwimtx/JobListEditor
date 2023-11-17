#ifndef POSITIONINPUTDIALOG_H
#define POSITIONINPUTDIALOG_H

#include <QDialog>

namespace Ui {
    class PositionInputDialog;
}

class PositionInputDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit PositionInputDialog(QWidget *parent = 0);
        ~PositionInputDialog();

    void   setValue( float );
    double getValue() const;
    bool   valueIsValid() const;
                
public slots:
    
    void handlePbToggleSign(); 
    virtual void accept();

protected:

    void keyPress(QChar key);

private:
    
    void updateGui();

    Ui::PositionInputDialog *ui;

    bool mPlusSign;
    bool mValueValid;
    double mValue;

};

#endif // POSITIONINPUTDIALOG_H

#include "positioninputdialog.h"
#include "ui_positioninputdialog.h"
#include <QDoubleValidator>

PositionInputDialog::PositionInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PositionInputDialog),
    mPlusSign( true ),
    mValueValid( false ),
    mValue( 0 )
{
    ui->setupUi(this);
    QObject::connect( ui -> pbToggleSign , SIGNAL( clicked() ), this , SLOT( handlePbToggleSign() ) , Qt::UniqueConnection );
}

PositionInputDialog::~PositionInputDialog()
{
    delete ui;
}

void PositionInputDialog::setValue( float value ) 
{ 
    mValue = value; 
    mPlusSign = value >= 0.0;
    ui -> leInput -> setText( QString::number( value ) );
    ui -> leInput -> setValidator( new QDoubleValidator( this ) );

}

double PositionInputDialog::getValue() const
{
    return mValue;
}

bool PositionInputDialog::valueIsValid() const
{
    return mValueValid;
}

void PositionInputDialog::handlePbToggleSign()
{
    mPlusSign = not mPlusSign;
    ui -> pbToggleSign -> setText( mPlusSign ? "+" : "-" );
}

void PositionInputDialog::accept()
{
    QString value = ui -> leInput -> text();
    value.replace( ',' , '.' );        
    mValue = value.toDouble( &mValueValid );
    if ( ( mPlusSign and ( mValue < 0 ) ) or
         ( ( not mPlusSign ) and ( mValue >= 0 ) )
        )
        mValue *= -1;

    QDialog::accept();
}


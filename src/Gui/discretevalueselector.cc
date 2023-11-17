#include "discretevalueselector.h"
#include "ui_discretevalueselector.h"

DiscreteValueSelector::DiscreteValueSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiscreteValueSelector)
{
    ui->setupUi(this);
}

DiscreteValueSelector::~DiscreteValueSelector()
{
    delete ui;
}

void DiscreteValueSelector::setAllowedValues( const QStringList& allowedValues )
{
    mAllowedValues = allowedValues;
    
    for (int i = 0; i < mAllowedValues.size(); ++i)
    {
        QRadioButton* radioButton = new QRadioButton( this );

        radioButton->setObjectName( QString( mAllowedValues[ i ] ).append( "_").append( QString::number( i )));
        radioButton->setText( mAllowedValues[ i ] );
        ui -> verticalLayout -> addWidget(radioButton);
        mButtonVector.push_back( radioButton );
    }
}

QString DiscreteValueSelector::getValue()
{

    for ( unsigned i = 0; i < mButtonVector.size(); i++ )
        if ( mButtonVector[ i ] -> isChecked() ) 
            return mButtonVector[ i ] -> text();

    return QString();
          
}

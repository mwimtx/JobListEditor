#ifndef DISCRETEVALUESELECTOR_H
#define DISCRETEVALUESELECTOR_H

#include <QDialog>
#include <QStringList>
#include <QRadioButton>
#include <vector>

namespace Ui {
    class DiscreteValueSelector;
}

class DiscreteValueSelector : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DiscreteValueSelector(QWidget *parent = 0);
        ~DiscreteValueSelector();

    void    setAllowedValues( const QStringList& );
    QString getValue();

    private:
        Ui::DiscreteValueSelector *ui;

    QStringList mAllowedValues;

    typedef std::vector<QRadioButton*> ButtonVector;
    ButtonVector mButtonVector;

};

#endif // DISCRETEVALUESELECTOR_H

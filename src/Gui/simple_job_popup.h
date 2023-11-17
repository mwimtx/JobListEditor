#ifndef SIMPLE_JOB_POPUP_H
#define SIMPLE_JOB_POPUP_H

#include <QDialog>
#include "tools_types.h"

namespace Ui {
    class SimpleJobPopup;
}

class SimpleJobPopup : public QDialog
{
    Q_OBJECT
        
public:
    explicit SimpleJobPopup(QStringList qstrlValidSampleFreqs, QWidget *parent = 0);
    ~SimpleJobPopup();

    SimpleJobPopup& setDurationAsInterval( const Types::TimeInterval& );
    
    Types::TimeInterval getDuration() const;

    unsigned        getHour () const { return mHour; }
    SimpleJobPopup& setHour ( const unsigned& value ) { mHour = value; return *this; }

    unsigned        getMinute () const { return mMinute; }
    SimpleJobPopup& setMinute ( const unsigned& value ) { mMinute = value; return *this; }
    
    unsigned        getSecond () const { return mSecond; }
    SimpleJobPopup& setSecond ( const unsigned& value ) { mSecond = value; return *this; }
    
    unsigned        getFrequency () const
    {
        return mFrequency;
    }

    SimpleJobPopup& setFrequency ( const unsigned& value ) { mFrequency = value; return *this; }

    void updateDurationDisplay();
    void updateFrequencyDisplay();

    void updateDisplay();

public slots:
    
    void setFrequency();
    
    void setDurationHour   ();
    void setDurationMinute ();
    void setDurationSecond ();

private:
    
    Ui::SimpleJobPopup *ui;

    unsigned mFrequency;

    unsigned mHour;
    unsigned mMinute;
    unsigned mSecond;

    QStringList qstrlSampleFreqs;

};

#endif // SIMPLE_JOB_POPUP_H

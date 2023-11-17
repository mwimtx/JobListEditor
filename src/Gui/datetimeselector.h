#ifndef DATETIMESELECTOR_H
#define DATETIMESELECTOR_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
    class DateTimeSelector;
}


class DateTimeSelector : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DateTimeSelector(QWidget *parent = 0);
        ~DateTimeSelector();

        DateTimeSelector& setDateTime( const QDateTime&  );

        void setStartMode(const QString& qstrStartMode);

        const QString getStartMode(void);

        void setCyclicJobParameters(const QString& qstrCyclicEvent, const QString& qstrCyclicDuration, const QString& qstrCyclicUnit);

        const QString getCyclicEvent    (void);
        const QString getCyclicDuration (void);
        const QString getCyclicUnit     (void);

        bool getCleanJobTables (void);

        bool setCleanJobTable (const bool bFlag);

        QDateTime getDateTime() const;


    public slots:

        void setHourFromKeypad   ();
        void setMinuteFromKeypad ();
        void setSecondFromKeypad ();

        void setTodayFromKeypad    ();
        void setTomorrowFromKeypad ();

        void setDayFromKeypad();
        void setMonthFromKeypad();
        void setYearFromKeypad();

        void setDate( const QDate & );


    private slots:

        void on_pbStartTimeExpert_clicked ();

        void on_cbStartMode_currentIndexChanged(int index);



    protected:

        void setupSignals();
        void updateTimeDisplay();
        void updateDateDisplay();


    private:
        Ui::DateTimeSelector *ui;

        int mHour;
        int mMinute;
        int mSecond;

        QDate mDate;

};

#endif // DATETIMESELECTOR_H

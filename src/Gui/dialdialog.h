#ifndef DIALDIALOG_H
#define DIALDIALOG_H

#include <QDialog>

namespace Ui {
class DialDialog;
}

/**
 * \class DialDialog
 *
 * \brief This class implements the DialDialog input class.
 *
 * This dialog can be used to enter a new value by the use of "up / down" buttons.
 *
 * \author MWI
 * \date 2014-07-23
 */
class DialDialog : public QDialog
{
    Q_OBJECT

    public:

        /**
         * This is the return value of the dialog.
         */
        double dValue;

        /**
         * This flag is set to true if the user closed the dialog by clicking on
         * "OK" button, hence the return value is valid.
         */
        bool bRetValueValid;

        /**
         * \brief This is the class constructor
         *
         * This is the constructor. It sets up the GUI element. Especially it sets up the
         * QDoubleSpinBox to fit to the min / max values and stepping.
         *
         * @param[in] const double dMax = maximum value
         * @param[in] const double dMin = minimum value
         * @param[in] const double dStep = single step, if one of the button is clicked once
         * @param[in] const double dInitValue = initial value for Spinbox
         * @param[in] const QString qstrHeading = heading string
         * @param[in] const QString qstrSuffix = suffix for Spinbox number
         * @param[in] QWidget *parent = 0 = pointer to parent widget class
         *
         * \author MWI
         * \date 2014-07-23
         */
        explicit DialDialog(const double dMax, const double dMin, const double dStep, const double dInitValue, const QString qstrHeading, const QString qstrSuffix, QWidget *parent = 0);

        /**
         * \brief This is the class destructor
         *
         * frees up all allocated resources
         *
         * \author MWI
         * \date 2014-07-23
         */
        ~DialDialog();


    private slots:

        /**
         * \brief This function is to handle clicks on the "up" button.
         *
         * It will increment the value of the Spinbox.
         *
         * \author MWI
         * \date 2014-07-23
         */
        void on_pbUp_pressed (void);

        /**
         * \brief This function is to handle clicks on the "down" button.
         *
         * It will decrement the value of the Spinbox.
         *
         * \author MWI
         * \date 2014-07-23
         */
        void on_pbDown_pressed (void);

        /**
         * \brief This function is to handle the released signal for the "up" button.
         *
         * It will reset the "steps inc/decremented" counter
         *
         * \author MWI
         * \date 2014-07-23
         */
        void on_pbUp_released (void);

        /**
         * \brief This function is to handle the released signal for the "down" button.
         *
         * It will reset the "steps inc/decremented" counter
         *
         * \author MWI
         * \date 2014-07-23
         */
        void on_pbDown_released (void);

        /**
         * \brief This slot is called if the "Cancel" button is clicked.
         *
         * It will simply close the dialog without any item beeing selected.
         *
         * \author MWI
         * \date 2014-07-23
         */
        void on_pbCancel_clicked (void);

        /**
         * \brief This slot is called if the "OK" button is clicked.
         *
         * This will store the actual value of the double spin box
         * into the return value and afterwards close the dialog.
         *
         * \author MWI
         * \date 2014-07-23
         */
        void on_pbOK_clicked (void);


    private:

        /**
         * Link to GUI class (QDesigner form)
         */
        Ui::DialDialog *ui;

        /**
         * This counter is used to switch between increment / decrement speeds if
         * the up / down button is clicked.
         */
        unsigned int uiNumStepsIncDec;
        unsigned int uiNumStepsLastIncDec;

        /**
         * \brief This function will increment / decrement thi spinbox value.
         *
         * It will increment / decrement the spinbox value according to the value of the
         * bIncrement parameter. It will use a pause of 125ms between two steps and increment
         * the value as long as the button is pressed.
         * After the first 4 increments it will decrease the pause time between 2 increments
         * to 62ms to have faster scrolling.
         *
         * @param[in] const bool bIncrement = true: increment value / false: decrement value
         *
         * \author MWI
         * \date 2014-07-23
         */
        void scrollSpinBoxValue (const bool bIncrement);

};

#endif // DIALDIALOG_H

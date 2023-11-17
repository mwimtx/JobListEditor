#ifndef FASTBUTTONSELECTPAD_H
#define FASTBUTTONSELECTPAD_H

#include <QDialog>
#include <QSignalMapper>

namespace Ui {
    class FastButtonSelectPad;
}


/**
 * \class FastButtonSelectPad
 *
 * \brief This class implements the "Fast Button Select Keypad".
 *
 * The "Fast Button Select Keypad" will show a dialog that shows buttons
 * for all items, that have been handed over to the constructor as a
 * QStringList (e.g. a list of valid sample frequencies).
 * The user may now click on one of the buttons to select the item. This
 * will automatically close the dialog so that program execution is returned
 * to the calling process.
 * The return value will be stored inside the global attribute "qstrRetValue".
 * The additional value "bValueSelected" will be set to true, if the dialog was
 * closed because the use selected one of the buttons and will stay false,
 * if the dialog was closed for any other reason and the return value is
 * undefined.
 *
 * \date 2014-01-31
 * \author MWI
 */
class FastButtonSelectPad : public QDialog
{
    Q_OBJECT

    public:

        /**
         * This is the public attribute that will contain the selected item.
         */
        QString qstrRetValue;

        /**
         * This public attribute can be evaluated to check, if one of the buttons
         * was clicked (qstrRetValue is valid) or the dialog closed for any other
         * reason.
         */
        bool bValueSelected;

        /**
         * show cancel button.
         */
        bool bShowCancelButton;

        /**
         * \brief This is the class constructor.
         *
         * This is the classes constructor that will receive the QStringList with the
         * items that shall be shown as buttons for selection and the heading as string.
         *
         * @param[in] QStringList& qstrlButtonTexts = list with all items buttons shall be shown for
         * @param[in] const QString& qstrHeading = string with heading for window
         * @param[in] QWidget *parent = 0 = pointer to parent widget class
         * @param[in] const bool bShowCancelButton = true = show the cancel button - not in all cases this makes sense, e.g. if choosing joblist type you MUST select 1 option
         */
        explicit FastButtonSelectPad(QStringList& qstrlButtonTexts, const QString& qstrHeading, QWidget *parent = 0, const bool bShowCancelButton = true);

        /**
         * \brief This is the class destructor.
         *
         * -
         */
        ~FastButtonSelectPad();


    private slots:

        /**
         * \brief This slot is called whenever a button is clicked.
         *
         * This slot is called by the signal mapper clButtonSigMapper whenever one of the
         * buttons is clicked.
         * It will set the return value qstrRetValue to the selected item and additionally
         * set bValueSelected to true.
         * Afterwards it will close the dialog.
         *
         * @param[in] int iIndex = index of selected item inside the qstrlButtons list
         */
        void buttonClicked (int iIndex);

        /**
         * \brief This slot is called if the "Cancel" button is clicked.
         *
         * It will simply close the dialog without any item beeing selected.
         */
        void on_pbCancel_clicked (void);


    private:

        /**
         * This attribute contains the list with items that are shown as buttons.
         */
        QStringList qstrlButtons;

        /**
         * This signal mapper handles the signals of the single buttons.
         */
        QSignalMapper clButtonSigMapper;

        /**
         * link to GUI (form)
         */
        Ui::FastButtonSelectPad *ui;

        /**
         * \brief This function will configure the Gui.
         *
         * This function will add a button for all items inside the qstrlButtons list
         * to the Gui and connect the "clicked (...)" slot of the single buttons to the
         * buttonClicked (...) slot by the use of the signal mapper.
         */
        void buildGui (void);
};

#endif // FASTBUTTONSELECTPAD_H

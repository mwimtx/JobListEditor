/* 
 *  TXM22 -- Graphical User Interface for Metronix TXM22 devices
 *  Copyright (C) 2010 Metronix Meßgeräte und Elektronik GmbH  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/** 
 * \file gui/freekeypadwindow.h
 * 
 * \brief Class for free input keypad.
 *
 * \author Clive.Thomsen@yasc.de
 * \date 2010/05/27
 * \version 1.0
 *
 * - changed at: -
 * - changed by: -
 * - changes:    -
 *  
 */
#ifndef KEYPAD_FREE_TXM_H
#define KEYPAD_FREE_TXM_H

#include <QDialog>
#include <QMap>
#include <ui_keypad_free_txm.h>


namespace Ui {
    class KeypadFreeTxm;
}


/** \brief Controller for "Free Keypad" Dialog.
  * A free keypad has an accpet Button and allows input of arbitrary length
  */
class KeypadFreeTxmClass : public QDialog
{
        Q_OBJECT
public:
        /** \brief Configure keypad.
          * The keypad position is fetched from global settings
          * \param integer Disallow input of floating values
          * \param sign Allow input of signed values
          * \param title Lable for the value to enter
          * \param parent Widget parent
          */
        explicit KeypadFreeTxmClass(bool integer, bool sign, QString title, QWidget *parent = 0);

        /** \brief Handler for keyboard input.
          * Allows to use keyboard instead of buttons
          * \param event Qt Keyboard event
          */
        void keyPressEvent(QKeyEvent *event);

        /** \brief Access input value as int.
          * \return Converted value
          */
        int         toInt() { return qstrInput.toInt(); }

        /** \brief Access input value as double.
          * \return Converted value
          */
        double      toDouble() { return qstrInput.toDouble(); }

        /** \brief Access input value.
          * \return Current input string
          */
        QString     text()  { return qstrInput; }

        ~KeypadFreeTxmClass();

private slots:
        /** \brief Handler for button clicks.
          * The calling button is decode in the handler
          */
        void buttonClicked();

protected:

        /** \brief Enables the given set of buttons.
          * \param buttons String with chars to enable
          */
        void enableButtons(QString buttons);

        /** \brief Process all input  (Buttons and keys).
          * \param key Key triggered
          */
        void keyPress(QChar key);

        /** \brief Internal helper: map a button to a certain key.
          */
        void mapButton(QPushButton* button, QChar c);

        /** \brief Check which buttons to enable after new input.
          */        void checkState();

        QMap<QPushButton*,QChar>    qmButtons;
        QString             qstrInput;
        QString             qstrLegalKeys;
        bool                bInteger;
        bool                bSign;
        Ui::KeypadFreeTxm   ui;
};

#endif // KEYPAD_FREE_TXM_H

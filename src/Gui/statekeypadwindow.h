/* 
 *  TXM22 -- Graphical User Interface for Metronix TXM22 devices
 *  Copyright (C) 2010 Metronix Me√üger√§te und Elektronik GmbH  
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
 * \file gui/statekeypadwindow.h
 * 
 * \brief Classes providing stateful Keypads.
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
#ifndef STATEKEYPADWINDOW_H
#define STATEKEYPADWINDOW_H

#include <QDialog>
#include <QMap>

#include "ui_keypad_new.h"

#define TXM_FORMAT_GPS          "X XXX∞XX'XX.XXX\""


/** \brief Generic stateful Keypad.
  * These keypads read input from the user against a prototype string,
  * auto accepting if no wildcards ('X') in the protottype are left
  */
class StateKeypadWindow : public QDialog
{
        Q_OBJECT
public:
        /** \brief Prepare keypad.
          * \param prototype The protoype for input (something like "XXX-XX.XX")
          * \param title Heading for value to input
          * \param parent QWidget parent
          */
        StateKeypadWindow(QString prototype, QString title, QWidget *parent = 0);

        /** \brief Handler for Keypress events.
          * Allows to trigger buttons by keyboard
          * \param event Qt Key event
          */
        void keyPressEvent(QKeyEvent *event);

        /** \brief Get user input.
          * \return String read by keypad
          */
        QString text() { return qstrInput; }

        virtual ~StateKeypadWindow();
signals:

public slots:

private slots:
        /** \brief Handels cliaks to dialog buttons.
          */
        void buttonClicked();

protected:
        /** \brief Enable a set of Ui buttons.
          * \param buttons String of keys to enable
          */
        void enableButtons(QString buttons);

        /** \brief Handle key or button click.
          * \param key User input
          */
        virtual void keyPress(QChar key);

        /** \brief Check state of inpuot.
          * May change enabled buttons or auto accept dialog
          */
        virtual void enterState();

        /** \brief Add to user input.
          * \param key Valur to add
          */
        void addKey(QChar key);

        /** \brief Delete last key from user input.
          */
        void delKey();

        /** \brief Delete all user input.
          */
        void clearAll();
private:
        /** \brief Connecvt Button with key value.
          * \param button Button to map
          * \parac c Key value
          */
        void mapButton(QPushButton* button, QChar c);

        QMap<QPushButton*,QChar>    qmButtons;
        QString                     qstrLegalKeys;
        QString                     qstrPrototype;
        QString                     qstrInput;

        Ui::KeypadNew               ui;
};

/** \brief Class specialized for GPS input.
  * Input will have the format [+/-] XXX∞XX'XX.XXX"
  */
class GpsKeypadWindow : public StateKeypadWindow
{
        Q_OBJECT
public:
        explicit GpsKeypadWindow(QString title, QWidget *parent = 0);

        /** \brief Access Gps coordinate.
          * \return Input in msecs
          */
        long gpsValue();

protected:
        /// \brief Ensures input will be valid.
        virtual void enterState();

};

/** \brief Class specialized for input of minutes or seconds.
  * Input will be in [0..59]
  */
class MinSecKeypadWindow : public StateKeypadWindow
{
        Q_OBJECT
public:
        /** \brief Setup.
          * \param title Heading for value to input
          * \param parent Qt Widget parent
          */
        explicit MinSecKeypadWindow(QString title, QWidget* parent = 0);

        /** Access input
          * \return time value
          */
        int timeValue();
protected:
        /// \brief Ensures input will be valid.
        virtual void enterState();
};

/** \brief Class specialized for input of hours.
  * Input will be in [0..23]
  */
class HourKeypadWindow : public StateKeypadWindow
{
        Q_OBJECT
public:
        /** \brief Setup.
          * \param title Heading for value to input
          * \param parent Qt widget parent
          */
        explicit HourKeypadWindow(QString title, QWidget* parent = 0);

        /** Access input
          * \return time value
          */
        int timeValue();
protected:
        /// \brief Ensures input will be valid.
        virtual void enterState();
};

/** \brief Class specialized for input of time offsets.
  * Input will be in [-12..+12]
  */
class OffsetKeypadWindow : public StateKeypadWindow
{
        Q_OBJECT
public:
        explicit OffsetKeypadWindow(QWidget* parent = 0);

protected:
        /// \brief Ensures input will be valid.
        virtual void enterState();
};


/** \brief Class specialized for input of years.
  * Input will be in [0000..7999]
  */
class YearKeypadWindow : public StateKeypadWindow
{
        Q_OBJECT
public:
        /** \brief Setup.
          * \param title Heading for value to input
          * \param parent Qt widget parent
          */
        explicit YearKeypadWindow(QString title, QWidget* parent = 0);

        /** Access input
          * \return time value
          */
        int yearValue();
protected:
        /// \brief Ensures input will be valid.
        virtual void enterState();
};


/** \brief Class specialized for input of months.
  * Input will be in [00..12]
  */
class MonthKeypadWindow : public StateKeypadWindow
{
        Q_OBJECT
public:
        /** \brief Setup.
          * \param title Heading for value to input
          * \param parent Qt widget parent
          */
        explicit MonthKeypadWindow(QString title, QWidget* parent = 0);

        /** Access input
          * \return time value
          */
        int monthValue();
protected:
        /// \brief Ensures input will be valid.
        virtual void enterState();
};


/** \brief Class specialized for input of days.
  * Input will be in [00..12]
  */
class DayKeypadWindow : public StateKeypadWindow
{
        Q_OBJECT
public:
        /** \brief Setup.
          * \param title Heading for value to input
          * \param parent Qt widget parent
          */
        explicit DayKeypadWindow(QString title, QWidget* parent = 0);

        /** Access input
          * \return time value
          */
        int dayValue();
protected:
        /// \brief Ensures input will be valid.
        virtual void enterState();
};


#endif // STATEKEYPADWINDOW_H

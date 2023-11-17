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
 * \file gui/statekeypadwindow.cc
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
#include "statekeypadwindow.h"
#include "mainwindow.h"

//#include "settings.h"
//#include "main.h"

#include <QKeyEvent>
#include <QSettings>
#include <QDebug>

StateKeypadWindow::StateKeypadWindow(QString prototype, QString title, QWidget *parent) :
    QDialog(parent), qstrPrototype(prototype)
{
        ui.setupUi(this);
        ui.titleLabel->setText(title);

        pclSettings->beginGroup(objectName());
        restoreGeometry(pclSettings->value("geometry").toByteArray());
        pclSettings->endGroup();

        mapButton(ui.backButton, 'B');
        mapButton(ui.clearButton, 'C');
        mapButton(ui.plusButton,'+');
        mapButton(ui.minusButton, '-');
        mapButton(ui._0Button, '0');
        mapButton(ui._1Button, '1');
        mapButton(ui._2Button, '2');
        mapButton(ui._3Button, '3');
        mapButton(ui._4Button, '4');
        mapButton(ui._5Button, '5');
        mapButton(ui._6Button, '6');
        mapButton(ui._7Button, '7');
        mapButton(ui._8Button, '8');
        mapButton(ui._9Button, '9');

        enableButtons("1234567890+-BC");

        clearAll();

}

StateKeypadWindow::~StateKeypadWindow() {
        pclSettings->beginGroup(objectName());
        pclSettings->setValue("geometry",saveGeometry());
        pclSettings->endGroup();
}
void StateKeypadWindow::enterState() {
        if (qstrInput.isEmpty())
                enableButtons("1234567890+-");
        else
                enableButtons("1234567890+-BC");
}


void StateKeypadWindow::mapButton(QPushButton *button, QChar c)
{
        qmButtons[button] = c;
        connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void StateKeypadWindow::enableButtons(QString buttons)
{
        qstrLegalKeys = buttons;
        foreach(QPushButton* button, qmButtons.keys()){
                button->setEnabled(buttons.contains(qmButtons[button]));
        }
}

void StateKeypadWindow::buttonClicked()
{
        QChar key = qmButtons[static_cast<QPushButton*>(sender())];
        keyPress(key);
}

void StateKeypadWindow::keyPressEvent(QKeyEvent *event)
{
        QString text(event->text());
        QChar key;

        if (event->key() == Qt::Key_Delete)
                key = 'C';
        else if (event->key() == Qt::Key_Backspace)
                key = 'B';
        else if (text.length() == 1)
                key = text[0];
        else {
                QDialog::keyPressEvent(event);
                return;
        }

        if (qstrLegalKeys.contains(key)) {
                event->accept();
                keyPress(key);
        } else
                QDialog::keyPressEvent(event);
}

void StateKeypadWindow::addKey(QChar key)
{
        qstrInput+=key;

        while (qstrInput.length() < qstrPrototype.length()) {
                if (qstrPrototype[qstrInput.length()] != 'X')
                        qstrInput += qstrPrototype[qstrInput.length()];
                else
                        break;
        }

        ui.valueLine->setText(qstrInput + qstrPrototype.mid(qstrInput.length()));
}

void StateKeypadWindow::delKey()
{
        do {
                qstrInput.resize(qstrInput.length()-1);
        } while (qstrInput.length() && qstrPrototype[qstrInput.length()] != 'X');

        ui.valueLine->setText(qstrInput + qstrPrototype.mid(qstrInput.length()));
}

void StateKeypadWindow::clearAll()
{
        qstrInput.clear();
        ui.valueLine->setText(qstrPrototype);
}

void StateKeypadWindow::keyPress(QChar key)
{
        //qDebug() << __func__ << key;
        switch(key.toLatin1()) {
        case 'C':
                clearAll();
                break;
        case 'B':
                delKey();
                break;
        default:
                addKey(key);
        }

        enterState();

        if (qstrPrototype.length() == qstrInput.length())
                accept();
}



GpsKeypadWindow::GpsKeypadWindow(QString title, QWidget *parent)
        : StateKeypadWindow(TXM_FORMAT_GPS, title, parent)
{
        enterState();
}

void GpsKeypadWindow::enterState()
{
        const char *buttons = 0;

        switch (text().length()) {
        case 0:
                buttons = "+-";
                break;
        case 2:
                buttons = "01BC";
                break;
        case 6:
        case 9:
                buttons = "012345BC";
                break;
        default:
                buttons = "1234567890BC";
        }

        if (buttons) enableButtons(buttons);
}

long GpsKeypadWindow::gpsValue()
{
        long value = text().mid(2,3).toLong();
        value = value * 60 + text().mid(6,2).toLong();
        value = value * 60 + text().mid(9,2).toLong();
        value = value * 1000 + text().mid(12,3).toLong();

        if (text()[0] == '-')
                value *= -1;

        return value;
}


MinSecKeypadWindow::MinSecKeypadWindow(QString title, QWidget* parent)
        : StateKeypadWindow("XX", title ,parent)
{
        enterState();
}

int MinSecKeypadWindow::timeValue()
{
        return text().toInt();
}

void MinSecKeypadWindow::enterState()
{
        switch (text().length()) {
        case 0:
                enableButtons("012345");
                break;
        default:
                enableButtons("0123456789BC");
        }
}

HourKeypadWindow::HourKeypadWindow(QString title, QWidget* parent)
        : StateKeypadWindow("XX", title ,parent)
{
        enterState();
}

int HourKeypadWindow::timeValue()
{
        return text().toInt();
}

void HourKeypadWindow::enterState()
{
        switch (text().length()) {
        case 0:
                enableButtons("012");
                break;
        case 1:
                if (text()[0] == '2') {
                        enableButtons("0123BC");
                        break;
                }
        default:
                enableButtons("0123456789BC");
        }
}

OffsetKeypadWindow::OffsetKeypadWindow(QWidget* parent)
        : StateKeypadWindow("XXX", tr("Local Time offset [hours]"),parent)
{
        enterState();
}


void OffsetKeypadWindow::enterState()
{
        switch (text().length()) {
        case 0:
                enableButtons("+-");
                break;
        case 1:
                enableButtons("01BC");
                break;
        case 2:
                if (text()[1] == '1') {
                        enableButtons("012BC");
                        break;
                }
        default:
                enableButtons("0123456789BC");
        }
}


YearKeypadWindow::YearKeypadWindow(QString title, QWidget* parent)
        : StateKeypadWindow("XXXX", title ,parent)
{
        enterState();
}

int YearKeypadWindow::yearValue()
{
        return text().toInt();
}

void YearKeypadWindow::enterState()
{
        switch (text().length()) {
        case 0:
                enableButtons("0123457689");
                break;
        default:
                enableButtons("0123456789BC");
        }
}


MonthKeypadWindow::MonthKeypadWindow(QString title, QWidget* parent)
        : StateKeypadWindow("XX", title ,parent)
{
        enterState();
}

int MonthKeypadWindow::monthValue()
{
        return text().toInt();
}

void MonthKeypadWindow::enterState()
{
        switch (text().length()) {
        case 0:
                enableButtons("01");
                break;
        case 1:
                if (text()[0] == '0')
                {
                    enableButtons("123456789BC");
                }
                else
                {
                    enableButtons("012BC");
                }
                break;
        default:
                enableButtons("0123456789BC");
        }
}


DayKeypadWindow::DayKeypadWindow(QString title, QWidget* parent)
        : StateKeypadWindow("XX", title ,parent)
{
        enterState();
}

int DayKeypadWindow::dayValue()
{
        return text().toInt();
}

void DayKeypadWindow::enterState()
{
        switch (text().length()) {
        case 0:
                enableButtons("0123");
                break;
        case 1:
                if (text()[0] == '0')
                {
                    enableButtons("123456789BC");
                }
                else if (text()[0] == '3')
                {
                    enableButtons("01BC");
                }
                else
                {
                    enableButtons("0123456789BC");
                }
                break;
        default:
                enableButtons("0123456789BC");
        }
}


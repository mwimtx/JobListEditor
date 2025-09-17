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
 * \file gui/freekeypadwindow.cc
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
#include "freekeypadwindow.h"
#include "mainwindow.h"

#include <QSettings>
#include <QVariant>
#include <QKeyEvent>
#include <QDebug>

#include "ui_keypad_free_txm.h"

KeypadFreeTxmClass::KeypadFreeTxmClass(bool integer, bool sign, QString title, QWidget *parent) :
    QDialog(parent)
{
        ui.setupUi(this);
        ui.titleLabel->setText(title);

        bInteger = integer;
        bSign    = sign;

        pclSettings->beginGroup(objectName());
        restoreGeometry(pclSettings->value("geometry").toByteArray());
        pclSettings->endGroup();

        mapButton(ui.backButton,  'B');
        mapButton(ui.clearButton, 'C');
        mapButton(ui.pointButton, '.');
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

        // set focus to OK button to make backspace, delete and enter buttons
        // on keyboard make work properly
        ui.okButton->setFocus();

        checkState();
}

KeypadFreeTxmClass::~KeypadFreeTxmClass()
{
        pclSettings->beginGroup(objectName());
        pclSettings->setValue("geometry",saveGeometry());
        pclSettings->endGroup();
}

void KeypadFreeTxmClass::enableButtons(QString buttons)
{
        qstrLegalKeys = buttons;
        foreach(QPushButton* button, qmButtons.keys())
        {
                button->setEnabled(buttons.contains(qmButtons[button]));
        }
}

void KeypadFreeTxmClass::mapButton(QPushButton *button, QChar c)
{
        qmButtons[button] = c;
        connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void KeypadFreeTxmClass::buttonClicked()
{
        QChar key = qmButtons[static_cast<QPushButton*>(sender())];
        keyPress(key);
}

void KeypadFreeTxmClass::keyPressEvent(QKeyEvent *event)
{
        QString text(event->text());
        QChar key;

        qDebug() << __func__ << QString::number(event->key(),16);

        if (event->key() == Qt::Key_Delete)
                key = 'C';
        else if (event->key() == Qt::Key_Backspace)
                key = 'B';
        else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
                accept();
        else if (text.length() == 1)
                key = text[0];
        else {
                QDialog::keyPressEvent(event);
                return;
        }
        if (key==',') key = '.';

        if (qstrLegalKeys.contains(key)) {
                event->accept();
                keyPress(key);
        } else
                QDialog::keyPressEvent(event);
}

void KeypadFreeTxmClass::checkState()
{
        QString buttons("0123456789");

        if (!bInteger && !qstrInput.contains('.'))
                buttons += '.';
        if (bSign && qstrInput.isEmpty())
                buttons += '-';
        if (!qstrInput.isEmpty())
                buttons += "BC";

        enableButtons(buttons);
}

void KeypadFreeTxmClass::keyPress(QChar key)
{
    switch (key.toLatin1()) {
        case 'B':
                qstrInput.resize(qstrInput.length()-1);
                break;
        case 'C':
                qstrInput.clear();
                break;
        case '.':
                if (qstrInput.isEmpty())
                        qstrInput += '0';
        default:
                qstrInput += key;
        }
        ui.valueLine->setText(qstrInput);
        checkState();
}

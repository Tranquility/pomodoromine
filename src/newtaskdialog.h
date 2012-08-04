/****************************************************************************
**
** Copyright (C) 2012  Ole Reifschneider

** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


#ifndef NEWTASKDIALOG_H
#define NEWTASKDIALOG_H

#include "ui_newtaskdialog.h"

#include <QDialog>
#include <QSettings>
#include <QXmlQuery>
#include <QDebug>



namespace Ui {
class newTaskDialog;
}

class newTaskDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit newTaskDialog(QWidget *parent = 0);
    QString subject;
    QString estimation;
    QString projectId;
    QString userId;

    ~newTaskDialog();
    
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::newTaskDialog *ui;
    QSettings settings;
    QStringList projectIdList;

    void fillProjectbox();

};

#endif // NEWTASKDIALOG_H

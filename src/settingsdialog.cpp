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


#include "settingsdialog.h"

settingsDialog::settingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDialog)
{
    ui->setupUi(this);

    ui->keyEdit->setText(settings.value("login/key","none").toString());
    ui->hostdomainEdit->setText(settings.value("login/hostdomain","none").toString());

}

settingsDialog::~settingsDialog()
{
    delete ui;
}

void settingsDialog::on_buttonBox_accepted()
{
    settings.setValue("login/key",ui->keyEdit->text());
    settings.setValue("login/hostdomain",ui->hostdomainEdit->text());
    accept();
}

void settingsDialog::on_buttonBox_rejected()
{
    reject();
}

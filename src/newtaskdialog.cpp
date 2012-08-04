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


#include "newtaskdialog.h"

newTaskDialog::newTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newTaskDialog)
{
    ui->setupUi(this);

    fillProjectbox();
}

newTaskDialog::~newTaskDialog()
{
    delete ui;
}

void newTaskDialog::fillProjectbox()
{
    QStringList projectNameList;
    QXmlQuery query;

    const QString projectsNameQueryUrl = "doc('"+settings.value("login/hostdomain").toString()+"/projects.xml?key="+settings.value("login/key").toString()+"')/projects/project/name/string()";
    query.setQuery(projectsNameQueryUrl);
    query.evaluateTo(&projectNameList);

    const QString projectsIdQueryUrl = "doc('"+settings.value("login/hostdomain").toString()+"/projects.xml?key="+settings.value("login/key").toString()+"')/projects/project/id/string()";
    query.setQuery(projectsIdQueryUrl);
    query.evaluateTo(&projectIdList);

    const QString userIdQueryUrl = "doc('"+settings.value("login/hostdomain").toString()+"/users/current.xml?key="+settings.value("login/key").toString()+"')/user/id/string()";
    query.setQuery(userIdQueryUrl);
    query.evaluateTo(&userId);

    ui->projectComboBox->addItems(projectNameList);
}

void newTaskDialog::on_buttonBox_accepted()
{
    subject = ui->nameEdit->text();
    estimation = QString("%1").arg(ui->estimationSpinBox->text().toFloat()/2);
    projectId = projectIdList.at(ui->projectComboBox->currentIndex());
    accept();
}

void newTaskDialog::on_buttonBox_rejected()
{
    reject();
}

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


#ifndef TASKQUERY_H
#define TASKQUERY_H

#include <QList>
#include <QStringList>
#include <QSettings>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QXmlQuery>
#include <QXmlStreamWriter>

class TaskInformation
{
    public:
        typedef QList<TaskInformation> List;

        TaskInformation(const QString &tid, const QString &name, const QString &projectName, const QString &estimated, const QString &done);
        QString getId() const;
        QString getName() const;
        QString getProjectName() const;
        QString getEstimated() const;
        QString getDone() const;

    private:
        QString tid;
        QString name;
        QString projectName;
        QString estimated;
        QString done;
};


class TaskQuery
{
    public:
        static TaskInformation::List query();
        void markDone(QString taskId);
        void createTask(QString userId, QString subject, QString projectId, QString estimation);
        void savePomodoro(QString taskId);
        QNetworkAccessManager manager;

    private:
        static QString pomodorosDone(const QString &pomodoroId);
        void sendRequest(QString xml);
        void sendIssueUpdate(QString xml, QString id);

        QSettings settings;
};


#endif // TASKQUERY_H

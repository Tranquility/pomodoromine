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


#include "taskquery.h"

TaskInformation::TaskInformation(const QString &tid, const QString &name, const QString &projectName, const QString &estimated, const QString &done)
    : tid(tid), name(name), projectName(projectName), estimated(estimated), done(done)
{
}

QString TaskInformation::getId() const
{
    return tid;
}

QString TaskInformation::getName() const
{
    return name;
}

QString TaskInformation::getProjectName() const
{
    return projectName;
}

QString TaskInformation::getEstimated() const
{
    return QString::number(estimated.toFloat()*2);
}

QString TaskInformation::getDone() const
{
    return QString::number(done.toFloat()*2);
}

TaskInformation::List TaskQuery::query()
{
    QSettings settings;
    QString url(settings.value("login/hostdomain").toString()+"/issues.xml?key="+settings.value("login/key").toString()+"");
    url = (QString("%1").arg(url));

    QMap<QString, QVariant> params;
    params["assigned_to_id"] = "me";
    params["status_id"] = "2";

    foreach(QString param, params.keys()) {
        url += QString("&amp;"+param+"="+params[param].toString());
    }

    const QString idsQueryUrl = QString("doc('%1')/issues/issue/id/string()").arg(url);
    const QString namesQueryUrl = QString("doc('%1')/issues/issue/subject/string()").arg(url);
    const QString projectsNameQueryUrl = QString("doc('%1')/issues/issue/project/string(@name)").arg(url);
    const QString estimatedPomodorosQueryUrl = QString("doc('%1')/issues/issue/estimated_hours/string()").arg(url);

    QStringList idList;
    QStringList nameList;
    QStringList projectNameList;
    QStringList estimatedList;

    QXmlQuery query;
    query.setQuery(idsQueryUrl);
    query.evaluateTo(&idList);

    query.setQuery(namesQueryUrl);
    query.evaluateTo(&nameList);

    query.setQuery(projectsNameQueryUrl);
    query.evaluateTo(&projectNameList);

    query.setQuery(estimatedPomodorosQueryUrl);
    query.evaluateTo(&estimatedList);

    TaskInformation::List information;

    if (idList.count()*3 == (nameList.count()+projectNameList.count()+estimatedList.count()) )
    {
        for (int i = 0; i < idList.count(); ++i)
            information.append(TaskInformation(idList.at(i).simplified(), nameList.at(i).simplified(), projectNameList.at(i).simplified(),
                                               estimatedList.at(i).simplified(), pomodorosDone(idList.at(i))));

    }
    return information;

}

QString TaskQuery::pomodorosDone(const QString &pomodoroId)
{
    QSettings settings;
    const QString donePomodorosQueryUrl = "doc('"+settings.value("login/hostdomain").toString()+"/issues/"+pomodoroId+".xml?key="+settings.value("login/key").toString()+"')/issue/spent_hours/string()";

    QString done;
    QXmlQuery query;

    query.setQuery(donePomodorosQueryUrl);
    query.evaluateTo(&done);

    return done;
}

void TaskQuery::markDone(QString taskId)
{
    QString xmlstring;
    QXmlStreamWriter stream(&xmlstring);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("issue");
    stream.writeTextElement("status_id", "5");
    stream.writeEndElement(); // time_entry
    stream.writeEndDocument();

    QByteArray myBArray(xmlstring.toUtf8());
    myBArray.resize(myBArray.size() - 1);
    QNetworkRequest request;
    QUrl url(settings.value("login/hostdomain").toString()+"/issues/"+taskId+".xml?key="+settings.value("login/key").toString()+"");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");
    manager.put(request, myBArray);
}

void TaskQuery::createTask(QString userId, QString subject, QString projectId, QString estimation)
{
    QString xmlstring;
    QXmlStreamWriter stream(&xmlstring);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("issue");
    stream.writeTextElement("subject", subject);
    stream.writeTextElement("project_id", projectId);
    stream.writeTextElement("estimated_hours", estimation);
    stream.writeTextElement("assigned_to_id", userId);
    stream.writeEndElement();
    stream.writeEndDocument();

    QByteArray myBArray(xmlstring.toUtf8());
    myBArray.resize(myBArray.size() - 1);
    QNetworkRequest request;
    QUrl url(settings.value("login/hostdomain").toString()+"/issues.xml?key="+settings.value("login/key").toString()+"");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");
    manager.post(request, myBArray);
}

void TaskQuery::savePomodoro(QString taskId)
{
    QString xmlstring;
    QXmlStreamWriter stream(&xmlstring);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("time_entry");
    stream.writeTextElement("issue_id", taskId);
    stream.writeTextElement("hours", "0.5");
    stream.writeEndElement(); // time_entry
    stream.writeEndDocument();

    QByteArray myBArray(xmlstring.toUtf8());
    myBArray.resize(myBArray.size() - 1);
    QNetworkRequest request;
    QUrl url(settings.value("login/hostdomain").toString()+"/time_entries.xml?key="+settings.value("login/key").toString()+"");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");
    manager.post(request, myBArray);
}

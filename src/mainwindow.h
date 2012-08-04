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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "taskquery.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "newtaskdialog.h"

#include <QMainWindow>
#include <QNetworkReply>
#include <QStateMachine>
#include <QSystemTrayIcon>
#include <QTreeWidget>
#include <QStateMachine>
#include <QTimer>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <QSettings>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QDialog>
#include <QDesktopServices>
#include <QMessageBox>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void createTask(QString userId, QString subject, QString projectId, QString estimation);
    ~MainWindow();

private slots:
    void on_pushButton_released();
    void timerTimeout();
    void showTime();
    void resetUI();
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current);
    void tickingAboutToFinish();
    void startPomodoro();
    void startBreak();
    void savePomodoro();

    void on_actionSettings_triggered();
    void on_actionOpenInBrowser_triggered();
    void on_actionMark_Done_triggered();
    void on_actionNew_Task_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    TaskQuery taskquery;
    TaskInformation::List tasks;
    QStateMachine stateMaschine;
    QState idleState;
    QState pomodoroState;
    QState breakState;
    QTimer timer;
    QNetworkAccessManager issueManager;
    QNetworkAccessManager projectManager;
    QStringList ids;
    QStringList names;
    QStringList projectNames;
    QStringList estimatesPomodoros;
    QXmlQuery query;
    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    QSystemTrayIcon trayIcon;
    int timeLeftInMSecs;
    int currentSet;
    float leftAlltime;
    QString done;
    QString donePomodorosQueryUrl;
    float pomodoroMinutes;
    QString currentItemId;
    QSettings settings;
    QString key;
    QSet<int> dataLists;

    void evaluateQuery(const QString url, QStringList &list);
    void startTimer();
    void showTasks();
    void addProject(QString name, QString description);
    void addTask(QTreeWidgetItem *parent, QString name, QString description, QString id);

signals:
    void pomodoroStarted();
    void setFinished();
    void pomodoroFinished();
    void breakFinished();
    void cancelSet();
    void cancelBreak();
};

#endif // MAINWINDOW_H

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


#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(mediaObject, audioOutput);

    pomodoroMinutes = 25;

    // set state transitions
    idleState.addTransition(this, SIGNAL(pomodoroStarted()), &pomodoroState);
    pomodoroState.addTransition(this, SIGNAL(cancelSet()), &idleState);
    breakState.addTransition(this, SIGNAL(setFinished()), &idleState);
    pomodoroState.addTransition(this, SIGNAL(pomodoroFinished()), &breakState);
    breakState.addTransition(this, SIGNAL(breakFinished()), &pomodoroState);
    breakState.addTransition(this, SIGNAL(cancelBreak()), &pomodoroState);


    // connections for states
    connect(&idleState, SIGNAL(entered()), this, SLOT(resetUI()));
    connect(&pomodoroState, SIGNAL(entered()), this, SLOT(startPomodoro()));
    connect(&breakState, SIGNAL(entered()), this, SLOT(startBreak()));
    connect(&breakState, SIGNAL(entered()), this, SLOT(savePomodoro()));


    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(tickingAboutToFinish()));
    connect(ui->actionRefresh, SIGNAL(triggered()), this, SLOT(resetUI()));
    connect(&taskquery.manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(resetUI()));


    // add states to statemaschine and start it
    stateMaschine.addState(&idleState);
    stateMaschine.addState(&pomodoroState);
    stateMaschine.addState(&breakState);
    stateMaschine.setInitialState(&idleState);
    stateMaschine.start();

    // state properties
    idleState.assignProperty(ui->pushButton,"text",tr("Start Pomodoro"));
    pomodoroState.assignProperty(ui->pushButton, "text", tr("Cancel Pomodoro"));
    breakState.assignProperty(ui->pushButton, "text", tr("Cancel Break"));
    idleState.assignProperty(ui->comboBox, "visible", true);
    pomodoroState.assignProperty(ui->comboBox, "visible", false);
    idleState.assignProperty(ui->currentSetSizeLabel, "visible", false);
    pomodoroState.assignProperty(ui->currentSetSizeLabel, "visible", true);
    pomodoroState.assignProperty(ui->timeEdit, "styleSheet", "background-color:white");
    breakState.assignProperty(ui->timeEdit, "styleSheet", "background-color:green");


    ui->comboBox->addItem("1");
    ui->comboBox->addItem("2");
    ui->comboBox->addItem("3");
    ui->comboBox->addItem("4");
    ui->comboBox->setCurrentIndex(3);

    ui->treeWidget->setColumnCount(4);
    ui->treeWidget->setColumnWidth(0, 250);
    ui->pushButton->setEnabled(false);

    // session timer setup
    timeLeftInMSecs = pomodoroMinutes * 60 * 1000;
    timer.setSingleShot(false);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));

    // systray setup
    trayIcon.setIcon(QIcon(":/images/pomodoromine.png"));
    trayIcon.setVisible(true);
}

MainWindow::~MainWindow()
{
    delete mediaObject;
}


void MainWindow::evaluateQuery(const QString url, QStringList &list)
{
    query.setQuery(url);
    query.evaluateTo(&list);
}

void MainWindow::on_pushButton_released()
{
    if (stateMaschine.configuration().contains(&pomodoroState))
    {
        timer.stop();
        mediaObject->clearQueue();
        mediaObject->stop();
        emit cancelSet();
    } else if (stateMaschine.configuration().contains(&breakState)) {
        if (currentSet != 0)
        {
            emit cancelBreak();
        } else {
            timer.stop();
            mediaObject->clearQueue();
            mediaObject->stop();
            emit setFinished();
        }
    } else  {
        currentSet = ui->comboBox->currentIndex()+1;
        emit pomodoroStarted();
    }
}

void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current)
{
    if (current) {
        ui->pushButton->setEnabled(true);
        ui->label->setText(current->text(0));
        currentItemId = current->data(0,Qt::UserRole).toString();

        const QString taskDescriptionQueryUrl = "doc('"+settings.value("login/hostdomain").toString()+"/issues/"+currentItemId+".xml?key="+settings.value("login/key").toString()+"')/issue/description/string()";

        QString description;

        query.setQuery(taskDescriptionQueryUrl);
        query.evaluateTo(&description);
        ui->taskDescriptionBrowser->setText(description);
    }
}

void MainWindow::tickingAboutToFinish()
{
    if (stateMaschine.configuration().contains(&pomodoroState))
    {
        mediaObject->enqueue(Phonon::MediaSource(QString(":/sound/tick.wav")));
    }
}

void MainWindow::startPomodoro()
{
    timeLeftInMSecs = pomodoroMinutes * 60 * 1000;
    ui->currentSetSizeLabel->setText(tr("Pomodoros left: " )+QString::number(currentSet));
    startTimer();
}

void MainWindow::startBreak()
{
    --currentSet;
    timeLeftInMSecs = 5 * 60 * 1000;
    startTimer();
}

void MainWindow::startTimer()
{
    timer.start(1000);
    if (stateMaschine.configuration().contains(&pomodoroState)) {
        mediaObject->enqueue(QString(":/sound/tick.wav"));
        mediaObject->play();
    }
}

void MainWindow::showTasks()
{
    tasks = TaskQuery::query();

    for (int i = 0; i < tasks.count(); ++i)
    {

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setData(0,Qt::UserRole,tasks.at(i).getId());
        item->setText(0, tasks.at(i).getName());
        item->setText(1, tasks.at(i).getProjectName());
        item->setText(2, tasks.at(i).getEstimated());
        item->setText(3, tasks.at(i).getDone());
        ui->treeWidget->insertTopLevelItem(i, item);
    }

    leftAlltime = 0;
    for (int i = 0; i < tasks.count(); ++i)
    {

        // count how many have been done for the whole time
        const QString doneAlltimePomodorosQueryUrl = "doc('"+settings.value("login/hostdomain").toString()+"/issues/"+tasks.at(i).getId()+
                ".xml?key="+settings.value("login/key").toString()+"')/issue/spent_hours/string()";
        QString doneA;
        QXmlQuery query;
        query.setQuery(doneAlltimePomodorosQueryUrl);
        query.evaluateTo(&doneA);
        float leftForTask = tasks.at(i).getEstimated().toFloat()-doneA.toFloat()*2;

        if (leftForTask > 0)
            leftAlltime += leftForTask;
    }
}

void MainWindow::timerTimeout()
{
    timeLeftInMSecs -= 1000;
    showTime();

    // if session/break expired
    if (0 >= timeLeftInMSecs) {
        timer.stop();
        if (stateMaschine.configuration().contains(&breakState))
        {
            if (currentSet > 0)
            {
                emit breakFinished();
            } else
            {
                emit setFinished();
            }
        } else {
            emit pomodoroFinished();
        }
        mediaObject->setCurrentSource(QString(":/sound/rang.mp3"));
        mediaObject->play();
    }
}

void MainWindow::showTime()
{
    // update systray
    if (stateMaschine.configuration().contains(&pomodoroState)) {
        int percent = (((pomodoroMinutes * 60 * 1000 - timeLeftInMSecs) / (pomodoroMinutes * 60 * 1000)) * 100);
        trayIcon.setIcon(QIcon(QString(":/images/progress-%10.png").arg(percent/10*10)));
    } else if (stateMaschine.configuration().contains(&breakState)) {
        trayIcon.setIcon(QIcon(QString(":/images/break.png")));
    } else {
        trayIcon.setIcon(QIcon(QString(":/images/pomodoromine.png")));
    }

    QTime time = QTime().addMSecs(timeLeftInMSecs);
    ui->timeEdit->setTime(time);

    trayIcon.setToolTip(QString(tr("Time Left: %1").arg(time.toString("mm:ss"))));
}

void MainWindow::resetUI()
{
    ui->treeWidget->clear();
    showTime();
    showTasks();

    const QString donePomodorosQueryUrl = "sum(doc('"+settings.value("login/hostdomain").toString()+"/time_entries.xml?key="+settings.value("login/key").toString()+"&amp;period_type=1&amp;period=today&amp;limit=100')/time_entries/time_entry/hours/text())";
    QString done;
    QXmlQuery query;
    query.setQuery(donePomodorosQueryUrl);
    query.evaluateTo(&done);

    ui->pomodorosLeftLabel->setText(QString::number(leftAlltime));
    ui->pomodorosDoneLabel->setText(QString::number(done.toFloat()*2));
}

void MainWindow::savePomodoro()
{
    taskquery.savePomodoro(currentItemId);
}

void MainWindow::on_actionSettings_triggered()
{
    settingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionOpenInBrowser_triggered()
{
    QDesktopServices::openUrl(QUrl(settings.value("login/hostdomain").toString()+"/issues/"+currentItemId));
}

void MainWindow::on_actionMark_Done_triggered()
{
    taskquery.markDone(QString(currentItemId));
}

void MainWindow::on_actionNew_Task_triggered()
{
    newTaskDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
        taskquery.createTask(dialog.userId,dialog.subject,dialog.projectId,dialog.estimation);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "about this program",QString("<h1>Pomodoromine %1 </h1>").arg(APP_VERSION));
}

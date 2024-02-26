#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QModelIndex>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->dateEdit->hide();

    // ui->to_do_Desc->setEnabled(false);

    ui->timeEdit->setTime(currentTime);
    ui->dateEdit->setDate(currentDate);

    ui->tabWidget->setCurrentIndex(0);

    viewMyDay();
    viewDaily();
    viewPlanned();
    on_pushButton_light_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBox_day_currentIndexChanged(int index)
{
    int selection;
    selection = index;
    if (selection == 0) {
        ui->dateEdit->hide();
    } else if (selection == 1) {
        ui->dateEdit->hide();
        ui->dateEdit->setDate(QDate::currentDate().addDays(1));
    } else if (selection == 2) {
        ui->dateEdit->show();
    }
}

void MainWindow::viewMyDay()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");

    if (!db.open()) {
        QMessageBox::critical(this,
                              "Error",
                              "Unable to open the database: " + db.lastError().text());
        return;
    }

    // Check if the table "tasks" exists in the database
    QStringList tables = db.tables();
    if (!tables.contains("TodaysTasks", Qt::CaseInsensitive)) {
        QMessageBox::critical(this, "Error", "Table 'Tasks' not found in the database");
        db.close();
        return;
    }

    QSqlTableModel *model = new QSqlTableModel(this, db);
    model->setTable("TodaysTasks");
    model->setSort(model->fieldIndex("Time"), Qt::AscendingOrder);

    // You can set additional properties for the model, such as sorting and filtering

    if (model->select()) {
        // Set the model to the TableView
        ui->tableView_myday->setModel(model);

    } else {
        QMessageBox::critical(this, "Error", "Failed to retrieve data from the 'tasks' table");
    }

    db.close();
}

void MainWindow::viewDaily()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");

    if (!db.open()) {
        QMessageBox::critical(this,
                              "Error",
                              "Unable to open the database: " + db.lastError().text());
        return;
    }

    // Check if the table "tasks" exists in the database
    QStringList tables = db.tables();
    if (!tables.contains("Daily", Qt::CaseInsensitive)) {
        QMessageBox::critical(this, "Error", "Table 'Tasks' not found in the database");
        db.close();
        return;
    }

    QSqlTableModel *model = new QSqlTableModel(this, db);
    model->setTable("Daily");
    model->setSort(model->fieldIndex("Time"), Qt::AscendingOrder);

    if (model->select()) {
        ui->tableView_daily->setModel(model);

    } else {
        QMessageBox::critical(this, "Error", "Failed to retrieve data from the 'tasks' table");
    }

    db.close();
}

void MainWindow::viewPlanned()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");

    if (!db.open()) {
        QMessageBox::critical(this,
                              "Error",
                              "Unable to open the database: " + db.lastError().text());
        return;
    }

    // Check if the table "tasks" exists in the database
    QStringList tables = db.tables();
    if (!tables.contains("Tasks", Qt::CaseInsensitive)) {
        QMessageBox::critical(this, "Error", "Table 'Tasks' not found in the database");
        db.close();
        return;
    }

    QSqlTableModel *model = new QSqlTableModel(this, db);
    model->setTable("Tasks");

    model->setSort(model->fieldIndex("Due_Date"), Qt::AscendingOrder);

    if (model->select()) {
        // Set the model to the TableView
        ui->tableView_planned->setModel(model);
    } else {
        QMessageBox::critical(this, "Error", "Failed to retrieve data from the 'tasks' table");
    }

    db.close();
}

void MainWindow::addNewToDo(const QString &TO_DO,
                            int urgency,
                            const QString &dateString,
                            const QString &timeString,
                            const QString &description)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");

    if (!db.open()) {
        QMessageBox::critical(this,
                              "Error",
                              "Unable to open the database: " + db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Tasks (To_Do, Urgency, Due_Date, Time, Description, Status) VALUES "
                  "(:to_do, :urgency, "
                  ":date, :time, :description, :status)");
    query.bindValue(":to_do", TO_DO);
    query.bindValue(":urgency", urgency);
    query.bindValue(":date", dateString);
    query.bindValue(":time", timeString);
    query.bindValue(":description", description);
    query.bindValue(":status", "Pending");

    if (query.exec()) {
        ui->Result->setText("Successfully added.");
    } else {
        QMessageBox::critical(this, "Error", "Query execution failed: " + query.lastError().text());
    }

    db.close();
}

void MainWindow::addNewTodaysTask(const QString &todays_task)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");

    if (!db.open()) {
        QMessageBox::critical(this,
                              "Error",
                              "Unable to open the database: " + db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO TodaysTasks (Today, Urgency, Time, Description, Status) VALUES "
                  "(:today, :urgency, :time, :description, :status)");
    query.bindValue(":today", todays_task);

    if (query.exec()) {
        ui->Result->setText("Successfully added.");
    } else {
        QMessageBox::critical(this, "Error", "Query execution failed: " + query.lastError().text());
    }

    db.close();
}

void MainWindow::addNewDaily(const QString &TO_DO,
                             int urgency,
                             const QString &timeString,
                             const QString &description)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");

    if (!db.open()) {
        QMessageBox::critical(this,
                              "Error",
                              "Unable to open the database: " + db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Daily (To_Do, Urgency, Time, Description, Status) VALUES "
                  "(:to_do, :urgency, "
                  ":time, :description, :status)");
    query.bindValue(":to_do", TO_DO);
    query.bindValue(":urgency", urgency);
    query.bindValue(":time", timeString);
    query.bindValue(":description", description);
    query.bindValue(":status", "Pending");

    if (query.exec()) {
        ui->Result->setText("Successfully added.");
    } else {
        QMessageBox::critical(this, "Error", "Query execution failed: " + query.lastError().text());
    }

    db.close();
}

void MainWindow::on_pushButton_OK_clicked()
{
    comboBox_day_current_index = ui->comboBox_day->currentIndex();

    TO_DO = ui->to_do->text(); //Retreives To_Do from the TO_DO text box

    urgency = ui->spinBox_urgency->value(); //Retreives urgency from the urgency box or the dial

    time = ui->timeEdit->time(); //Retreives Time from the time box

    timeString = time.toString("hh:mm");

    date = ui->dateEdit->date(); //Retreives Date from the date box
    dateString = date.toString("yyyy:MM:dd dddd");

    description = ui->to_do_Desc
                      ->toPlainText(); //Retreives To_Do description from the to_do_desc text box

    if (!TO_DO.isEmpty()) {
        if (comboBox_day_current_index == 3) {
            addNewDaily(TO_DO, urgency, timeString, description);
            ui->Result->setText("Successfully added.");
            ui->to_do->clear();
            ui->to_do_Desc->clear();
            ui->spinBox_urgency->setValue(1);
            ui->tabWidget->setCurrentIndex(1);
        } else {
            addNewToDo(TO_DO, urgency, dateString, timeString, description);
            ui->Result->setText("Successfully added.");
            ui->to_do->clear();
            ui->to_do_Desc->clear();
            ui->spinBox_urgency->setValue(1);
            ui->tabWidget->setCurrentIndex(2);
        }
    }

    ui->to_do_Desc->setEnabled(false);

    viewDaily();
    viewPlanned();
}

void MainWindow::on_radioButton_Dial_toggled(bool checked)
{
    ui->dial_urgency->show();
    ui->spinBox_urgency->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->spinBox_urgency->setReadOnly(checked);
    ui->spinBox_urgency->setToolTip("Urgency");
}

void MainWindow::on_radioButton_SpinBox_toggled()
{
    ui->dial_urgency->hide();
    ui->spinBox_urgency->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    ui->spinBox_urgency->setReadOnly(false);
    ui->spinBox_urgency->setToolTip("Select the urgency");
}

void MainWindow::on_pushButton_addDesc_clicked()
{
    ui->to_do_Desc->setEnabled(true);
    ui->to_do_Desc->setFocus();
    // QTextCursor cursor = ui->to_do_Desc->textCursor();
    // cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    // ui->to_do_Desc->setTextCursor(cursor);
}

void MainWindow::on_pushButton_light_clicked()
{
    ui->tabWidget->setStyleSheet("background-color: rgb(199, 199, 199); color: rgb(0, 0, 0);");
    ui->pushButton_light->hide();
    ui->pushButton_dark->show();
}

void MainWindow::on_pushButton_dark_clicked()
{
    ui->tabWidget->setStyleSheet("background-color: rgb(55, 55, 55); color: rgb(250,250,250);");
    ui->pushButton_dark->hide();
    ui->pushButton_light->show();
}

void MainWindow::on_pushButton_add_clicked()
{
    ui->tabWidget->setCurrentIndex(3);
    ui->to_do->setFocus();
}

void MainWindow::on_pushButton_ok_clicked()
{
    todays_task = ui->lineEdit_todaysTask->text();
    if (!todays_task.isEmpty()) {
        addNewTodaysTask(todays_task);
        ui->lineEdit_todaysTask->clear();
    }
    viewMyDay();
}

void MainWindow::toggleTabBar()
{
    ui->tabWidget->tabBar()->setVisible(!ui->tabWidget->tabBar()->isVisible());
}

void MainWindow::on_toolButton_clicked()
{
    toggleTabBar();
}

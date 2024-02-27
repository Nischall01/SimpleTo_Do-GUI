#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "json.hpp"
#include <fstream>

#include <QMessageBox>
#include <QModelIndex>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QTimer>

using json = nlohmann::json;

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

    setviewTables();

    loadSettings();
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

void MainWindow::setviewTables()
{
    // Create database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");

    // Open the database
    if (!db.open()) {
        QMessageBox::critical(this,
                              "Error",
                              "Unable to open the database: " + db.lastError().text());
        return;
    }

    // Create models for different tables
    QSqlTableModel *myday = new QSqlTableModel(this, db);
    myday->setTable("MyDay");
    myday->setSort(myday->fieldIndex("Time"), Qt::AscendingOrder);
    myday->setEditStrategy(QSqlTableModel::OnFieldChange);
    myday->select();

    QSqlTableModel *daily = new QSqlTableModel(this, db);
    daily->setTable("Daily");
    daily->setSort(daily->fieldIndex("Time"), Qt::AscendingOrder);
    daily->setEditStrategy(QSqlTableModel::OnFieldChange);
    daily->select();

    QSqlTableModel *planned = new QSqlTableModel(this, db);
    planned->setTable("Planned");
    planned->setSort(planned->fieldIndex("Date"), Qt::AscendingOrder);
    planned->setEditStrategy(QSqlTableModel::OnFieldChange);
    planned->select();

    //Set models to respective QTablesetviewTables

    ui->tableView_myday->setModel(myday);
    ui->tableView_daily->setModel(daily);
    ui->tableView_planned->setModel(planned);
}

void MainWindow::addNewPlannedTask(const QString &TO_DO,
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
    query.prepare(
        "INSERT INTO Planned (To_Do, Urgency, Due_Date, Time, Description, Status) VALUES "
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

void MainWindow::addNewMyDayTask(const QString &todays_task)
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
    query.prepare("INSERT INTO MyDay (To_Do, Urgency, Time, Description, Status) VALUES "
                  "(:today, :urgency, :time, :description, :status)");
    query.bindValue(":today", todays_task);

    if (query.exec()) {
        ui->Result->setText("Successfully added.");
    } else {
        QMessageBox::critical(this, "Error", "Query execution failed: " + query.lastError().text());
    }

    db.close();
}

void MainWindow::addNewDailyTask(const QString &TO_DO,
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
            addNewDailyTask(TO_DO, urgency, timeString, description);
            ui->Result->setText("Successfully added.");
            ui->to_do->clear();
            ui->to_do_Desc->clear();
            ui->spinBox_urgency->setValue(1);
            ui->tabWidget->setCurrentIndex(1);

        } else {
            addNewPlannedTask(TO_DO, urgency, dateString, timeString, description);
            ui->Result->setText("Successfully added.");
            ui->to_do->clear();
            ui->to_do_Desc->clear();
            ui->spinBox_urgency->setValue(1);
            ui->tabWidget->setCurrentIndex(2);
        }

        // Clear the result message after 3 seconds
        QTimer::singleShot(3000, this, [this]() { ui->Result->clear(); });
    }

    setviewTables();
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
    ui->radioButton_Light->setChecked(true);
    ui->tabWidget->setStyleSheet("background-color: #f0f0f0; color: rgb(0, 0, 0);");
    ui->sideBar->setStyleSheet("background-color: rgb(191, 191, 191); color: rgb(0, 0, 0);");
    ui->pushButton_light->hide();
    ui->pushButton_dark->show();
}

void MainWindow::on_pushButton_dark_clicked()
{
    ui->radioButton_Dark->setChecked(true);
    ui->tabWidget->setStyleSheet("background-color: rgb(55, 55, 55); color: rgb(250,250,250);");
    ui->sideBar->setStyleSheet("background-color: rgb(55, 55, 55); color: rgb(250,250,250);");
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
        addNewMyDayTask(todays_task);
        ui->lineEdit_todaysTask->clear();
    }
    setviewTables();
}

void MainWindow::toggleTabBar()
{
    ui->tabWidget->tabBar()->setVisible(!ui->tabWidget->tabBar()->isVisible());
}

void MainWindow::on_toolButton_clicked()
{
    toggleTabBar();
}

// Settings Section

void MainWindow::loadSettings()
{
    json data;
    std::ifstream file("settings.json");
    if (!file.is_open()) {
        QMessageBox::critical(this, "Error", "Unable to open settings.json: ");
        return;
    }
    file >> data;
    file.close();

    int theme_mode = data["DefaultThemeMode"]; // 0 is Dark mode and 1 is light mode
    int urgency_mode = data["DefaultUrgencyMode"];

    if (theme_mode == 0) {
        on_pushButton_dark_clicked();
        ui->radioButton_Dark->setChecked(true);
    } else {
        on_pushButton_light_clicked();
        ui->radioButton_Light->setChecked(true);
    }
    if (urgency_mode == 0) {
        ui->radioButton_Dial->setChecked(true);
    } else {
        ui->radioButton_SpinBox->setChecked(true);
    }
}

void MainWindow::on_radioButton_Dial_toggled(bool checked)
{
    if (checked) {
        ui->dial_urgency->show();
        ui->spinBox_urgency->setButtonSymbols(QAbstractSpinBox::NoButtons);
        ui->spinBox_urgency->setReadOnly(checked);
        ui->spinBox_urgency->setToolTip("Urgency");
    }
}

void MainWindow::on_radioButton_SpinBox_toggled(bool checked)
{
    if (checked) {
        ui->dial_urgency->hide();
        ui->spinBox_urgency->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        ui->spinBox_urgency->setReadOnly(false);
        ui->spinBox_urgency->setToolTip("Select the urgency");
    }
}

void MainWindow::on_radioButton_Dark_toggled(bool checked)
{
    if (checked) {
        on_pushButton_dark_clicked();
    }
}

void MainWindow::on_radioButton_Light_toggled(bool checked)
{
    if (checked) {
        on_pushButton_light_clicked();
    }
}

void MainWindow::on_pushButton_settings_Save_clicked()
{
    json data;
    data["DefaultThemeMode"] = ui->radioButton_Dark->isChecked()
                                   ? 0
                                   : 1; // Save 0 for Dark mode, 1 for Light mode

    data["DefaultUrgencyMode"] = ui->radioButton_Dial->isChecked()
                                     ? 0
                                     : 1; // Save 0 for Dark mode, 1 for Light mode

    std::ofstream file("settings.json");
    if (!file.is_open()) {
        QMessageBox::critical(this, "Error", "Unable to open settings.json: ");
        return;
    }
    file << data;
    file.close();
}

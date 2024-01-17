#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->dateEdit->hide();

    ui->to_do_Desc->setEnabled(false);

    ui->timeEdit->setTime(currentTime);
    ui->dateEdit->setDate(currentDate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::viewDatabase()
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
    query.prepare("SELECT * FROM tasks");

    if (query.exec()) {
        ui->textEdit->clear();

        while (query.next()) {
            int id = query.value("id").toInt();
            QString task = query.value("task").toString();
            int urgency = query.value("urgency").toInt();
            QString date = query.value("date").toString();
            QString description = query.value("description").toString();

            QString result = QString("ID: %1\nTask: %2\nUrgency: %3\nDate: %4\nDescription: %5\n\n")
                                 .arg(id)
                                 .arg(task)
                                 .arg(urgency)
                                 .arg(date)
                                 .arg(description);

            ui->textEdit->append(result);
        }
    } else {
        QMessageBox::critical(this, "Error", "Query execution failed: " + query.lastError().text());
    }

    db.close();
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

void MainWindow::on_pushButton_OK_clicked()
{
    TO_DO = ui->to_do->text(); //Retreives To_Do from the TO_DO text box

    urgency = ui->spinBox_urgency->value(); //Retreives urgency from the urgency box or the dial

    time = ui->timeEdit->time(); //Retreives Time from the time box

    timeString = time.toString();

    date = ui->dateEdit->date(); //Retreives Date from the date box
    dateString = date.toString();

    if (!TO_DO.isEmpty()) {
        // ui->Result->setText(TO_DO);
        ui->Result->setText(QString::number(urgency));
        // ui->Result->setText(timeString);
        // ui->Result->setText(dateString);
        ui->to_do->clear();
        ui->to_do_Desc->clear();
        ;
    }

    ui->to_do_Desc->setEnabled(false);

    viewDatabase();
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
    ui->tabWidget->setStyleSheet("background-color: rgb(240, 240, 240);color: rgb(0,0,0;");
    ui->pushButton_light->hide();
    ui->pushButton_dark->show();
}

void MainWindow::on_pushButton_dark_clicked()
{
    ui->tabWidget->setStyleSheet("background-color: rgb(55, 55, 55);color: rgb(250,250,250);");
    ui->pushButton_dark->hide();
    ui->pushButton_light->show();
}

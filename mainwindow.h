#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QDateEdit>
#include <QMainWindow>
#include <QTimeEdit>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_OK_clicked();

    void on_comboBox_day_currentIndexChanged(int index);

    void on_radioButton_Dial_toggled(bool checked);

    void on_radioButton_SpinBox_toggled();

    void on_pushButton_addDesc_clicked();

    void on_pushButton_light_clicked();

    void on_pushButton_dark_clicked();

    void viewDatabase();

private:
    QTime time;
    QDate date;
    QString timeString;
    QString dateString;
    QString TO_DO;

    unsigned int urgency;

    QTime currentTime = QTime::currentTime();
    QDate currentDate = QDate::currentDate();

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

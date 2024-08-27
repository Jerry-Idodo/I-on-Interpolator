#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include "interpolateddata.h"
#include "hhtable.h"

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
    void on_SelectInputButton_clicked();

    void on_GenTableButton_clicked();

    void on_SaveInterpolatedDataButton_clicked();

    void on_SaveDailyDataButton_clicked();

private:
    Ui::MainWindow *ui;
    QString csvfile_path;
    QFileInfo fileinfo;
    InterpolatedData csvData;
    HHTable hhData;
};
#endif // MAINWINDOW_H

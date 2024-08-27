#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "interpolateddata.h"
#include "hhtable.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void show_message(const QString text)
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}


QString pick_file()
{
    QString filename = QFileDialog::getOpenFileName(nullptr, "Open File",
                                                    QDir::homePath(),
                                                    "csv (*.csv)");
    return filename;
}

void MainWindow::on_SelectInputButton_clicked()
{
    csvfile_path = pick_file();
    fileinfo.setFile(csvfile_path);

    ui->csvLabel->clear();
    ui->csvLabel->setText(fileinfo.fileName());

    // Load Data
    if(csvData.csvData.import_csv_data(csvfile_path)) {
        show_message("File Loaded!");
    } else {
        show_message("File Load Error!");
    }
}


void MainWindow::on_GenTableButton_clicked()
{   
    // Interpolate Data
    csvData.interpolate_csv_data();
    show_message("Data Interpolated!");

    // Create HH Table
    hhData.create_hh_table(csvData);
    show_message("Table Created!");

    // Save HH Table
    hhData.save_hh_table(csvfile_path);  
    ui->ImportLabel->setText(fileinfo.completeBaseName() + "_import.csv");
    ui->ExportLabel->setText(fileinfo.completeBaseName() + "_export.csv");
    show_message("File Saved");
}


void MainWindow::on_SaveInterpolatedDataButton_clicked()
{
    QString interpolated_file{fileinfo.completeBaseName() + "_interpolated.csv"};
    csvData.save_data(fileinfo.absolutePath() + "/" + interpolated_file);
    ui->InterpolatedLabel->setText(interpolated_file);
    show_message("Data Interpolated Saved!");
}


void MainWindow::on_SaveDailyDataButton_clicked()
{
    QString dailyfile{fileinfo.completeBaseName() + "_daily.csv"};
    csvData.save_daily_data(fileinfo.absolutePath() + "/" + dailyfile);
    ui->DailyDataLabel->setText(dailyfile);
    show_message("Daily Data Saved!");
}


#ifndef INTERPOLATEDDATA_H
#define INTERPOLATEDDATA_H

#include <QString>
#include <QDateTime>
#include <vector>

constexpr qint64 half_hour()
{
    return 30 * 60;
}

class RawData {

public:
    RawData() {}
    QString csvSerialNo;
    std::vector<QDateTime> csvTime;
    std::vector<double> csvImport;
    std::vector<double> csvExport;

    bool import_csv_data(const QString& csvfile_path);

};

class InterpolatedData
{

public:
    InterpolatedData();

    QString SerialNo;
    std::vector<QDateTime> TimeData;
    std::vector<double> ImportData;
    std::vector<double> ExportData;

    RawData csvData;
    bool interpolate_csv_data();
    void save_data(const QString& filename);
    void save_daily_data(const QString& filename);
};

#endif // INTERPOLATEDDATA_H

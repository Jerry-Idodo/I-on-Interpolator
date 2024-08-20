#ifndef HHTABLE_H
#define HHTABLE_H

#include "interpolateddata.h"

class DailyData {

public:
    QDate date;
    std::vector<double> imports;
    std::vector<double> exports;

    void clear_data();
};

class HHTable
{
    std::vector<DailyData> DataTable;
    QString SerialNo;
    bool loaded = false;

    QString remove_file_extension(const QString& filePath);
    QString print_header_row();
    QString print_body_row(int index, const QString& energy_type);

public:
    HHTable();

    bool create_hh_table (const InterpolatedData& HHData);
    bool save_hh_table (const QString& csvpath);
};

#endif // HHTABLE_H

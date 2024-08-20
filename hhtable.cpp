#include "hhtable.h"
#include <QFileInfo>

HHTable::HHTable() {}

void DailyData::clear_data()
{
    imports.clear();
    exports.clear();
}

bool HHTable::create_hh_table (const InterpolatedData& HHData)
{
    int i;
    QDate date;
    std::vector<double> import_data, export_data;
    DailyData Data;

    SerialNo = HHData.SerialNo;

    // First Row
    date = HHData.TimeData[0].date();
    Data.date = date;

    for (i = 1; i < HHData.TimeData.size(); i++) {
        import_data.push_back(HHData.ImportData[i] - HHData.ImportData[i - 1]);
        export_data.push_back(HHData.ExportData[i] - HHData.ExportData[i - 1]);

        if (HHData.TimeData[i].date() != date) {
            for (int j = i; j < 49; j++) {
                Data.imports.push_back(0.0);
                Data.exports.push_back(0.0);
            }
            Data.imports.insert(Data.imports.end(), import_data.begin(), import_data.end());
            Data.exports.insert(Data.exports.end(), export_data.begin(), export_data.end());
            import_data.clear();
            export_data.clear();
            break;
        }
    }

    DataTable.push_back(Data);
    Data.clear_data();

    //Other Rows
    date = HHData.TimeData[i].date();
    i -= 1;

    for (i += 1; i < HHData.TimeData.size(); i++) {
        if (HHData.TimeData[i].date() !=date) {
            import_data.push_back(HHData.ImportData[i] - HHData.ImportData[i - 1]);
            export_data.push_back(HHData.ExportData[i] - HHData.ExportData[i - 1]);

            Data.date = date;
            Data.imports = import_data;
            Data.exports = export_data;
            DataTable.push_back(Data);

            import_data.clear();
            export_data.clear();
            Data.clear_data();
            date = HHData.TimeData[i].date();
        }
        import_data.push_back(HHData.ImportData[i] - HHData.ImportData[i - 1]);
        export_data.push_back(HHData.ExportData[i] - HHData.ExportData[i - 1]);
    }

    //Last Row
    Data.date = date;
    Data.imports = import_data;
    Data.exports = export_data;
    DataTable.push_back(Data);

    qDebug() << "i is : " << i;
    loaded = true;
    return true;
}

QString HHTable::remove_file_extension(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.completeBaseName();
    QString path = fileInfo.absolutePath();
    return path + "/" + baseName;
}

QString HHTable::print_header_row()
{
    QString out_string = "Date, Serial No, ";
    QTime hh(0,0,0);

    for (int i = 0; i < 47; i++) {
        out_string += hh.addSecs(i * half_hour()).toString("HH:mm") + " ,";
    }
    out_string += hh.addSecs(47 * half_hour()).toString("HH:mm");

    return out_string;
}

QString HHTable::print_body_row(int index, const QString& energy_type)
{
    QString out_string = DataTable[index].date.toString("yyyy-MM-dd") + " ," + SerialNo + ", ";

    if (energy_type == "import") {
        for (int i = 0; i < DataTable[index].imports.size() - 2; i++) {
            out_string += QString::number(DataTable[index].imports[i], 'f', 2) + ", ";
        }
        out_string += QString::number(*(DataTable[index].imports.end() - 1), 'f', 2);
    } else if (energy_type == "export") {
        for (int i = 0; i < DataTable[index].exports.size() - 2; i++) {
            out_string += QString::number(DataTable[index].exports[i], 'f', 2) + ", ";
        }
        out_string += QString::number(*(DataTable[index].exports.end() - 1), 'f', 2);
    } else {
        return "error";
    }

    return out_string;
}

bool HHTable::save_hh_table (const QString& csvpath)
{
    QFileInfo csvfile_info{csvpath};
    QString path = remove_file_extension(csvpath);
    QString importfile_path = path + "_import.csv";
    QString exportfile_path = path + "_export.csv";

    if (!loaded)
        return false;

    QFile importfile{importfile_path};
    if (!importfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out_import(&importfile);

    out_import << print_header_row() << '\n';

    for (int i = 0; i < DataTable.size(); i++) {
        out_import << print_body_row(i, "import") << '\n';
    }

    QFile exportfile{exportfile_path};
    if (!exportfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out_export(&exportfile);

    out_export << print_header_row() << '\n';

    for (int i = 0; i < DataTable.size(); i++) {
        out_export << print_body_row(i, "export") << '\n';
    }

    return true;
}

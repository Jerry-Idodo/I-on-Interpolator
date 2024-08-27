#include "interpolateddata.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <sstream>

InterpolatedData::InterpolatedData() {}


static std::vector<std::string> split_csv_line(const QByteArray& line)
{
    std::vector<std::string> columns;
    std::stringstream ss{line.toStdString()};
    std::string buff;

    while (std::getline(ss, buff, ',')) {
        columns.push_back(buff);
    }

    return columns;
}

static std::string strip_leading_space(const std::string str)
{
    std::string striped_str;
    if (str[0] == ' ') {
        for (int i = 1; i < str.length(); i++) {
            striped_str += str[i];
        }
        return striped_str;
    }

    return str;
}

static std::string strip_char(const std::string& str, const char ch)
{
    std::string striped_str = str;
    if (str.find(ch) != std::string::npos)
        striped_str.erase(std::remove(striped_str.begin(), striped_str.end(), ch), striped_str.end());
    return striped_str;
}

static std::vector<int> get_column_indices(const std::vector<std::string> headings)
{
    int serial_no_index;
    int date_index;
    int import_index;
    int export_index;

    for (int i = 0; i < headings.size(); i++) {
        if (strip_leading_space(headings[i]) == "Serial Number") {
            serial_no_index = i;
        } else if (strip_leading_space(headings[i]) == "Date") {
            date_index = i;
        } else if (strip_leading_space(headings[i]) == "Import") {
            import_index = i;
        } else if (strip_leading_space(headings[i]) == "Export") {
            export_index = i;
        }
    }
    std::vector<int> indices = {serial_no_index, date_index, import_index, export_index};
    return indices;
}

QDateTime get_date_time(const std::string& str)
{
    std::string dateTimeStr = strip_char(str, '\"');
    QString qDateTimeStr = QString::fromStdString(dateTimeStr);
    QDateTime DateTime = QDateTime::fromString(qDateTimeStr, "yyyy-MM-dd HH:mm:ss");
    return DateTime;
}

QDateTime get_start_time(const QDateTime& csv_start_time)
{
    qint64 start_time_int = csv_start_time.toSecsSinceEpoch();
    if (start_time_int % half_hour() == 0)
        return csv_start_time;

    return QDateTime::fromSecsSinceEpoch(half_hour() + half_hour() * (start_time_int / half_hour()));
}

static int get_time_point(const QDateTime& time, const std::vector<QDateTime>& csvTime, bool *perfect_time)
{
    for (auto i = 0; i < csvTime.size(); i++) {
        if (time == csvTime[i]) {
            *perfect_time = true;
            return i;
        }

        if (time < csvTime[i]) {
            *perfect_time = false;
            return i;
        }
    }

    return 0;
}

bool RawData::import_csv_data(const QString& csvfile_path)
{
    int i = 0;
    QFile csvfile(csvfile_path);
    if (!csvfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray csv_header = csvfile.readLine();
    std::vector<std::string> headings = split_csv_line(csv_header);
    std::vector<int> indices = get_column_indices(headings);

    while (!csvfile.atEnd()) {
        QByteArray line = csvfile.readLine();
        std::vector<std::string> data = split_csv_line(line);

        if (i == 0) {
            csvSerialNo = QString::fromStdString(data[indices[0]]);
            i = 1;
        }

        csvTime.push_back(get_date_time(data[indices[1]]));
        csvImport.push_back(std::stod(data[indices[2]]));
        csvExport.push_back(std::stod(data[indices[3]]));
    }

    return true;
}

bool InterpolatedData::interpolate_csv_data()
{
    QDateTime csv_start_time = *csvData.csvTime.begin();
    QDateTime csv_end_time = *(csvData.csvTime.end() - 1);
    QDateTime time = get_start_time(csv_start_time);

    while (time < csv_end_time) {
        bool perfect_time = false;
        int t_index = get_time_point(time, csvData.csvTime, &perfect_time);

        SerialNo = csvData.csvSerialNo;
        TimeData.push_back(time);

        if (perfect_time) {
            ImportData.push_back(csvData.csvImport[t_index]);
            ExportData.push_back(csvData.csvExport[t_index]);
        } else {
            double time_diff = (csvData.csvTime[t_index] - csvData.csvTime[t_index - 1]).count() / 1000.0;
            double time_to_point = (csvData.csvTime[t_index] - time).count() / 1000.0;

            double temp_import = csvData.csvImport[t_index] - (csvData.csvImport[t_index] - csvData.csvImport[t_index - 1]) * time_to_point / time_diff;
            ImportData.push_back(temp_import);

            double temp_export = csvData.csvExport[t_index] - (csvData.csvExport[t_index] - csvData.csvExport[t_index - 1]) * time_to_point / time_diff;
            ExportData.push_back(temp_export);
        }

        time = time.addSecs(half_hour());
    }

    return true;
}


void InterpolatedData::save_data(const QString& filename)
{
    QFile outfile{filename};
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: could not open file " << filename;
        return;
    }

    QTextStream outstream(&outfile);
    outstream << "Serial Number, Date, Import, Export\n";

    for (int i = 0; i < TimeData.size(); i++) {
        outstream << SerialNo << ", "
                  << TimeData[i].toString("yyyy-MM-dd HH:mm") << ", "
                  << QString::number(ImportData[i], 'f', 2) << ", "
                  << QString::number(ExportData[i], 'f', 2) << '\n';
    }
}


void InterpolatedData::save_daily_data(const QString& filename)
{
    QFile outfile{filename};
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: could not open file " << filename;
        return;
    }

    QTextStream outstream(&outfile);
    outstream << "Serial Number, Date, Import, Export\n";

    for (int i = 0; i < TimeData.size(); i++) {
        if (TimeData[i].time() == QTime::fromMSecsSinceStartOfDay(0)) {
            QDateTime datetime = TimeData[i].addMSecs(-1);
            outstream << SerialNo << ", "
                      << datetime.toString("yyyy-MM-dd HH:mm:ss") << ", "
                      << QString::number(ImportData[i], 'f', 2) << ", "
                      << QString::number(ExportData[i], 'f', 2) << '\n';
        }
    }
}

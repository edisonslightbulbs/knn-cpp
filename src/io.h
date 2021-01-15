#ifndef IO_H
#define IO_H

#include <fstream>
#include <memory>
#include <string>

#include "logger.h"
#include "point.h"
#include "point3d.h"
#include "timer.h"

/** system $pwd */
#ifdef WINDOWS
#include <direct.h>
#define getCurrentDir _getcwd
#else
#include <unistd.h>
#define getCurrentDir getcwd
#endif

using pool = std::vector<std::vector<int>>;

extern const int PASS = 0;
// extern const int FAIL = -3;

static std::string pwd()
{
    char buff[FILENAME_MAX];
    getCurrentDir(buff, FILENAME_MAX);
    std::string workingDir(buff);
    return workingDir;
}

static void read(std::shared_ptr<std::vector<Point*>>& sharedPtr_points3d,
    const char* t_file)
{
    Timer timer;
    /* file stream and string for parsing csv file */
    std::ifstream data(t_file);
    std::string line;

    int pointId = 1;
    /* get each line in csv */
    while (std::getline(data, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        /* parse each line */
        while (std::getline(ss, cell, ' ')) {
            row.push_back(cell);
        }
        /* create points */
        Point* ptr_point = new Point3d(pointId, (float)std::stof(row[0]),
            (float)std::stof(row[1]), (float)std::stof(row[2]));
        sharedPtr_points3d->push_back(ptr_point);
        pointId++;
    }
    LOG(INFO) << "parsing data took: " << timer.getDuration();
    /* todo: a missing step here is to sort the data */
}

static void write(const std::shared_ptr<pool>& sharedPtr_neighbourLists,
    const std::string& t_file)
{
    std::ofstream filestream;
    filestream.open(t_file);
    filestream << "id,nn1" << std::endl;

    std::ostringstream pointIndex;

    for (auto& neighbourList : *sharedPtr_neighbourLists) {

        for (auto& neighbour : neighbourList) {
            pointIndex << neighbour << ",";
        }
        /** malarkey of dynamic csv file delimiters */
        std::string str = pointIndex.str();
        str.pop_back();
        filestream << str << std::endl;
        pointIndex.str(std::string());
    }
    filestream.close();
}

#endif /* IO_H */

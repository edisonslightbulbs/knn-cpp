#include "io.h"

std::vector<Point> IO::read(std::vector<Point> t_points, const char* t_file)
{
    /** create input stream and string for parsing file data */
    std::ifstream data(t_file);
    std::string line;

    /** assign each parsed point and id starting from 1 */
    int id = 1;

    /** while un-parsed lines exist ... */
    while (std::getline(data, line)) {

        /** ... get next line in file */
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        /** ... parse each line using ' ' as a delimiter */
        while (std::getline(ss, cell, ' ')) {
            row.push_back(cell);
        }

        /** create Point type definitions */
        Point point((float)std::stof(row[0]), (float)std::stof(row[1]),
            (float)std::stof(row[2]));
        point.m_id = id;
        t_points.push_back(point);
        id++;
    }
    return t_points;
}

void IO::write(const int& t_K,
    std::vector<std::vector<Point>>& t_neighbours, const std::string& t_file)
{
    std::ofstream filestream;
    filestream.open(t_file);
    filestream << "id,nn1" << std::endl;

    /** graph-able index for points already sorted in ascending order */
    int count = 1;
    for(int i = 0; i < t_K; ++i) {
        for (auto &point : t_neighbours[i]) {
            filestream << count << ", " << point.m_distance.second << std::endl;
            count++;
        }
    }
    filestream.close();
}

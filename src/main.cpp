#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <set>
#include <utility>
#include <vector>

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

extern const int PASS = 0;

std::string pwd()
{
    char buff[FILENAME_MAX];
    getCurrentDir(buff, FILENAME_MAX);
    std::string workingDir(buff);
    return workingDir;
}

void readData(std::shared_ptr<std::vector<Point*>>& sharedPtr_points3d,
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
}

void write(const std::vector<Point*>& t_points, const std::string& t_file)
{
    std::ofstream filestream;
    filestream.open(t_file);
    filestream << "id,n1,n2,n3,n4" << std::endl;

    for (auto* ptr_point : t_points) {
        filestream << ptr_point->m_x << ", " << ptr_point->m_y << ", "
                   << ptr_point->m_z << std::endl;
    }
    filestream.close();
}

void knn(const int& t_numNeighbours, std::vector<int>& t_numPointsPerThread,
    const std::shared_ptr<std::vector<Point*>>& sharedPtr_points3d,
    const std::shared_ptr<std::vector<std::vector<int>>>& sharedPtr_threadPool)

{
    Timer timer;

    /** create the knn task ?? */
    auto knnTask = [sharedPtr_points3d, sharedPtr_threadPool, &t_numNeighbours](
                       int firstIndex, int lastIndex) mutable {
        int index;
        int neighbourCount;
        double distance;

        /** track the distances from each point to all other
         *  points and save the distances as a set of tuples */
        std::set<std::pair<double, int>> tuples;

        for (int pointId = firstIndex; pointId < lastIndex; ++pointId) { // ??
            tuples.clear();
            index = 0;

            for (auto* point3d : *sharedPtr_points3d) {
                index++;
                if (pointId == index) {
                    continue;
                }
                distance = (*sharedPtr_points3d)[pointId]->distance(point3d);
                tuples.insert({ distance, point3d->m_id });
            }

            /** iterate for set of tuples and save the id's */
            neighbourCount = 0;
            for (auto it = tuples.begin();
                 it != tuples.end() && neighbourCount < t_numNeighbours;
                 ++it, ++neighbourCount) {
                (*sharedPtr_threadPool)[pointId].push_back(it->second);
            }
        }
    };

    /** create  wrapper for asynchronous operations ?? */
    std::vector<std::future<void>> compute;

    /** partition the knn task using the allocated number of
     *  points per thread and run it using multi-threads */
    int startIndex = 0;
    int endIndex = 0;
    for (const auto& numPoints : t_numPointsPerThread) {
        endIndex = startIndex + numPoints;
        compute.push_back(
            std::async(std::launch::async, knnTask, startIndex, endIndex));
        startIndex += numPoints;
    }

    /** ???? */
    for (auto& future : compute) {
        future.get();
    }

    /** output data */
    const std::string OUTPUT_FILE = pwd() + "/build/output_data.txt";
    std::ofstream outFile;
    outFile.open(OUTPUT_FILE);

    /** ???? */
    for (auto& pool : *sharedPtr_threadPool) {
        for (auto& point : pool) {
            outFile << point << " ";
        }
        outFile << std::endl;
    }
    outFile.close();

    LOG(INFO) << "knn compute completed in: " << timer.getDuration();
}

std::vector<int> configThreads(const int& t_numThreads, const int& t_numPoints)
{
    /** find number of points to process per thread */
    int pointsPerThread = t_numPoints / t_numThreads;

    /** take care of any (possible) surplus points */
    int surplusPoints = pointsPerThread + t_numPoints % t_numThreads;

    /** save thread config */
    std::vector<int> numPointsPerThread(t_numThreads, pointsPerThread);
    numPointsPerThread.back() = surplusPoints;

    return numPointsPerThread;
}

using pool = std::vector<std::vector<int>>;

int main(int argc, char* argv[])
{
    /** initialize logger */
    logger(argc, argv);

    /** define resources */
    const std::string INPUT_FILE = pwd() + "/resources/input_data.txt";

    /** create a shared pointer for the 3d points */
    std::shared_ptr<std::vector<Point*>> sharedPtr_points3d(
        new std::vector<Point*>);

    readData(sharedPtr_points3d, INPUT_FILE.c_str());
    const int THREADS_COUNT = 4;
    std::vector<int> numPointsPerThread
        = configThreads(THREADS_COUNT, sharedPtr_points3d->size());

    /** create a shared pointer for ... */
    std::shared_ptr<pool> sharedPtr_threadPool(
        new pool(sharedPtr_points3d->size()));
    // make notes about share pointer initialization !!!!

    const int NEIGHBOUR_COUNT = 4;

    /** compute k nearest neighbour */
    knn(NEIGHBOUR_COUNT, numPointsPerThread, sharedPtr_points3d,
        sharedPtr_threadPool);

    return PASS;
}

/**
 * continue usage:
 * see:
 * https://www.geeksforgeeks.org/continue-statement-cpp/
 * date: 2021-01-14 13:36
 */

/**
 * set usage:
 * see:
 * https://www.geeksforgeeks.org/set-in-cpp-stl/
 * date: 2021-01-14 13:41
 */

/**
 * pair usage:
 * see:
 * https://stackoverflow.com/questions/19655629/when-is-it-good-to-use-stdpair
 * date: 2021-01-14 13:41
 */

/**
 * it->second usage:
 * see:
 * https://stackoverflow.com/questions/15451287/what-does-iterator-second-mean
 * date: 2021-01-14 14:28
 */

#include <future>
#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "logger.h"
#include "point.h"
#include "timer.h"

/**
 *  For absolute path to  working directory */
#ifdef WINDOWS
#include <direct.h>
#define getCurrentDir _getcwd
#else
#include <unistd.h>
#define getCurrentDir getcwd
#endif

// extern const int PASS = 0;
// extern const int FAIL = -3;

std::string pwd()
{
    char buff[FILENAME_MAX];
    getCurrentDir(buff, FILENAME_MAX);
    std::string workingDir(buff);
    return workingDir;
}

using threadPool = std::vector<std::vector<int>>;

int main(int argc, char* argv[])
{
    /** initialize logger */
    logger(argc, argv);

    {
        /** time operation */
        Timer timer;

        /** file resources */
        const std::string INPUT_FILE = pwd() + "/resources/input_data.txt";
        const std::string OUTPUT_FILE = pwd() + "/build/output_data.txt";

        /** open system files */
        std::ifstream inFile;
        inFile.open(INPUT_FILE);
        std::ofstream outFile;
        outFile.open(OUTPUT_FILE);

        /** knn parameters */
        const int NEIGHBORS = 4;

        /** specify worker threads */
        int numPoints;
        inFile >> numPoints;
        const int THREADS = 4;
        int work = numPoints / THREADS; // points per thread

        /* account for surplus/left over points */
        int surplusWork = work + numPoints % THREADS;

        /** read this as  number of threads (i.e., THREADS)
         *  to process some number of points (i.e., work) */
        std::vector<int> threads(THREADS, work);
        threads.back() = surplusWork;

        /** create shared pointer to a list of points */
        std::shared_ptr<std::vector<Point3d>> sharedPtr_points3d(
            new std::vector<Point3d>(numPoints));

        /** create shared pointer to a list of lists of points */
        std::shared_ptr<threadPool> sharedPtr_threadPool(
            new threadPool(numPoints));

        /** parse 3d data points  from */
        int pointIndex = 1;
        for (auto& point : *sharedPtr_points3d) {
            inFile >> point;
            point.id = pointIndex++;
        }

        /** how to use shared pointers ???? */
        auto knn = [sharedPtr_points3d, sharedPtr_threadPool, &NEIGHBORS](
                       int firstIndex, int lastIndex) mutable {
            int index;
            int neighbour;
            double distance;

            std::set<std::pair<double, int>> dataSet;

            for (int pointId = firstIndex; pointId < lastIndex; ++pointId) {
                dataSet.clear();

                index = 0;
                for (auto& point3d : *sharedPtr_points3d) {
                    index++;
                    if (pointId == index) {
                        continue;
                    }

                    distance = (*sharedPtr_points3d)[pointId].distance(point3d);
                    dataSet.insert({distance, point3d.id });
                }

                neighbour = 0;
                for (auto setIter = dataSet.begin();
                     setIter != dataSet.end() && neighbour < NEIGHBORS;
                     ++setIter, ++neighbour) {
                    (*sharedPtr_threadPool)[pointId].push_back(setIter->second);
                }
            }
        };

        std::vector<std::future<void>> compute;

        int startIndex = 0;
        for (auto& thread : threads) {
            compute.push_back(std::async(
                std::launch::async, knn, startIndex, startIndex + thread));
            startIndex += thread;
        }

        for (auto& future : compute) {
            future.get();
        }

        for (auto& neighborsList : *sharedPtr_threadPool) {
            for (auto& neighbors : neighborsList) {
                outFile << neighbors << " ";
            }
            outFile << std::endl;
        }

        inFile.close();
        outFile.close();

        LOG(INFO) << "knn operation completed in: " << timer.getDuration();
    }
    return 0;
}

#include <Eigen/Dense>
#include <future>
#include <iostream>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "io.h"
#include "logger.h"
#include "point.h"
#include "point3d.h"
#include "timer.h"

using pool = std::vector<std::vector<int>>;

void knn(const int& t_numNeighbours, std::vector<int>& t_numPointsPerThread,
    const std::shared_ptr<std::vector<Point*>>& sharedPtr_points3d,
    const std::shared_ptr<std::vector<std::vector<int>>>&
        sharedPtr_neighbourLists)
{
    Timer timer;

    /** future knn task */
    auto knnTask = [sharedPtr_points3d, sharedPtr_neighbourLists,
                       &t_numNeighbours](
                       int firstIndex, int lastIndex) mutable {
        int index;
        double distance;

        /** for each point ... */
        std::set<std::pair<double, int>> tuples;
        for (int pointId = firstIndex; pointId < lastIndex; ++pointId) {
            tuples.clear();
            index = 0;

            for (auto* point3d : *sharedPtr_points3d) {
                index++;
                if (pointId == index) {
                    continue;
                }
                /** ... collect the distance  to every other point */
                distance = (*sharedPtr_points3d)[pointId]->distance(point3d);
                tuples.insert({ distance, point3d->m_id });
            }
            int count = 0;
            for (auto it = tuples.begin();
                 it != tuples.end() && count < t_numNeighbours; ++it, ++count) {

                /** ... and create a list of [k] to-neighbour distances */
                (*sharedPtr_neighbourLists)[pointId].push_back(it->second);
            }
        }
    };

    /** create  wrapper for asynchronous operations */
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

    /** run knn task asynchronously */
    for (auto& future : compute) {
        future.get();
    }
    LOG(INFO) << "knn compute completed in: " << timer.getDuration();
}

std::vector<int> computePointsPerThread(
    const int& t_numThreads, const int& t_numPoints)
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
static void sort(std::shared_ptr<std::vector<Point*>>& t_sharedPtr_points3d)
{

    /** express vec and matrix */
    Eigen::MatrixXd points3dMat(t_sharedPtr_points3d->size(), 3);
    int row = 0;
    for (auto* ptr_point : *t_sharedPtr_points3d) {
        /** create corresponding eigen matrix */
        points3dMat(row, 0) = ptr_point->m_x;
        points3dMat(row, 1) = ptr_point->m_y;
        points3dMat(row, 2) = ptr_point->m_z;
        row++;
    }

    Point* centroid = new Point3d(-1, (float)points3dMat.col(0).mean(),
        (float)points3dMat.col(1).mean(), (float)points3dMat.col(2).mean());

    /** sort point based on magnitude */
    // float distance = __DBL_MAX__;

    // std::shared_ptr<std::vector<Point*>> points3dCopy = t_sharedPtr_points3d;

    // for (auto* ptr_point3d : *sharedPtr_points3d) {
    //     // distance = ptr_point3d->distance(&t_centroid);
    //     // if (distance < m_ann) {
    //     //     m_ann = distance;
    //     // }
    // }

    // /** allocate ids' to sorted points */
    // int id = 1;
    // for (auto* ptr_point3d : *sharedPtr_points3d) {
    //     id++;
    // }
}

int main(int argc, char* argv[])
{
    /** initialize logger */
    logger(argc, argv);

    /** file with input data */
    const std::string INPUT_FILE = pwd() + "/resources/input_data.txt";

    /** create a shared pointer for the 3d points */
    std::shared_ptr<std::vector<Point*>> sharedPtr_points3d(
        new std::vector<Point*>);

    /** parse input data file */
    read(sharedPtr_points3d, INPUT_FILE.c_str());

    sort(sharedPtr_points3d);

    /** configure threads and points per thread */
    const int THREADS_COUNT = 4;
    std::vector<int> pointsPerThread
        = computePointsPerThread(THREADS_COUNT, sharedPtr_points3d->size());

    /** create a shared pointer for list of neighbours */
    std::shared_ptr<pool> sharedPtr_neighbourLists(
        new pool(sharedPtr_points3d->size()));

    const int NEIGHBOUR_COUNT = 2;

    /** compute k nearest neighbour */
    knn(NEIGHBOUR_COUNT, pointsPerThread, sharedPtr_points3d,
        sharedPtr_neighbourLists);

    const std::string OUTPUT_FILE = pwd() + "/build/output_data.txt";
    write(sharedPtr_neighbourLists, OUTPUT_FILE);

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

#include <future>
#include <memory>
#include <vector>

#include "io.h"
#include "knn.h"
#include "logger.h"
#include "point.h"
#include "timer.h"
#include "worker.h"

extern const int PASS = 0;

int main(int argc, char* argv[])
{
    /** initialize logger */
    logger(argc, argv);

    /** resources */
    const std::string INPUT_FILE = pwd() + "/resources/input_data.txt";
    const std::string OUTPUT_FILE = pwd() + "/build/output_data.txt";

    /** parse data */
    std::vector<Point> points;
    points = IO::read(points, INPUT_FILE.c_str());

    /** distribute points on threads */
    const int T = 4;
    Worker worker(T, points);

    std::shared_ptr<std::vector<std::vector<Point>>> sptr_neighbours;
    sptr_neighbours
        = std::make_shared<std::vector<std::vector<Point>>>(points.size());

    /** asynchronous task to compute */
    auto knnTask = [points, sptr_neighbours](std::vector<Point> work) mutable {
        Knn knn(points, work, sptr_neighbours);
    };

    /** create list of asynchronous tasks */
    std::vector<std::future<void>> compute;

    /** distribute asynchronous work for each thread */
    for (auto& work : *worker.sptr_work) {
        compute.push_back(std::async(std::launch::async, knnTask, work));
    }

    /** do asynchronous work */
    {
        Timer timer;
        for (auto& future : compute) {
            future.get();
        }
        LOG(INFO) << "knn computed in: " << timer.getDuration();
    }

    /** sort neighbour lists | metric = euclidean distance */
    {
        Timer timer;
        for (auto& neighbours : *sptr_neighbours) {
            // todo sort each list on a separate thread !!
            std::sort(neighbours.begin(), neighbours.end(), Point::compare);
        }
        LOG(INFO) << "neighbours sorted in: " << timer.getDuration();
    }

    /** for each point: output its K nearest neighbours */
    const int K = 1;
    IO::write(K, *sptr_neighbours, OUTPUT_FILE);

    return PASS;
}

/* notes:
 *
 * sorting execution time:
 *   16.090868 seconds (before optimization)
 *    1.234707 seconds (after optimization) */

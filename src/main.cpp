#include <future>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "io.h"
#include "logger.h"
#include "point.h"
#include "timer.h"
#include "utility.h"

void knn(const int& K, std::vector<int>& t_issue,
    const std::shared_ptr<std::vector<Point*>>& sptr_points3d,
    const std::shared_ptr<std::vector<std::vector<int>>>& sptr_neighbourLists)
{
    Timer timer;

    /** future knn task? */
    auto knnTask = [sptr_points3d, sptr_neighbourLists, &K](
                       int firstIndex, int lastIndex) mutable {
        int index;
        double distance;

        /** for each point ... ?*/
        std::set<std::pair<double, int>> tuples;
        for (int pointId = firstIndex; pointId < lastIndex; ++pointId) {
            tuples.clear();
            index = 0;

            for (auto* point3d : *sptr_points3d) {
                index++;
                if (pointId == index) {
                    continue;
                }
                /** ... collect the distance to neighbours? */
                distance = (*sptr_points3d)[pointId]->distance(point3d);
                tuples.insert({ distance, point3d->m_id });
            }
            int count = 0;
            for (auto it = tuples.begin(); it != tuples.end() && count < K;
                 ++it, ++count) {

                /** ... and create a list of K neighbours? */
                (*sptr_neighbourLists)[pointId].push_back(it->second);
            }
        }
    };

    /** create  wrapper for asynchronous operations? */
    std::vector<std::future<void>> compute;

    /** partition the knn on threads */
    int startIndex = 0;
    int endIndex = 0;
    for (const auto& numPoints : t_issue) {
        endIndex = startIndex + numPoints;
        compute.push_back(
            std::async(std::launch::async, knnTask, startIndex, endIndex));
        startIndex += numPoints;
    }

    /** run knn task asynchronously */
    for (auto& future : compute) {
        future.get();
    }
    LOG(INFO) << "knn computed successfully in: " << timer.getDuration();
}

using pool = std::vector<std::vector<int>>;

int main(int argc, char* argv[])
{
    /** initialize logger */
    logger(argc, argv);

    /** disk resources */
    const std::string INPUT_FILE = pwd() + "/resources/test.txt";
    const std::string OUTPUT_FILE = pwd() + "/build/output_data.txt";

    /** parse data */
    std::vector<Point*> points3d;
    points3d = read(points3d, INPUT_FILE.c_str());

    /** sort points (use ascending euclidean distance from centroid) */
    points3d = order(points3d);

    /** create shared pointer to points */
    auto sptr_points3d = std::make_shared<std::vector<Point*>>(points3d);

    /** configure threads and points per thread */
    const int THREADS = 4;
    std::vector<int> issue = multithread(THREADS, sptr_points3d->size());

    /** create a shared pointer to neighbours? */
    std::shared_ptr<pool> sptr_neighbourLists(new pool(sptr_points3d->size()));

    /** compute knn */
    const int K = 2;
    knn(K, issue, sptr_points3d, sptr_neighbourLists);

    /** output knn */
    write(sptr_neighbourLists, OUTPUT_FILE);

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

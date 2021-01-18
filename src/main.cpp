#include <memory>
#include <vector>

#include "io.h"
#include "knn.h"
#include "logger.h"
#include "point.h"
#include "utility.h"

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
    const int THREADS = 6;
    std::vector<int> issue = multithread(THREADS, sptr_points3d->size());

    /** create a shared pointer to neighbours? */
    std::shared_ptr<neighbours> sptr_neighbourLists(new neighbours(sptr_points3d->size()));

    /** compute knn */
    const int K = 1;
    knn(K, issue, sptr_points3d, sptr_neighbourLists);

    /** output knn */
    write(sptr_neighbourLists, OUTPUT_FILE);

    return PASS;
}

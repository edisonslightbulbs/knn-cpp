#ifndef KNN_H
#define KNN_H

#include <future>
#include <memory>
#include <vector>

#include "io.h"
#include "knn.h"
#include "logger.h"
#include "point.h"
#include "timer.h"
#include "utility.h"

static void knn(const int& K, const std::vector<int>& t_issue,
    const std::shared_ptr<std::vector<Point*>>& sptr_points3d,
    const std::shared_ptr<std::vector<std::vector<Point*>>>&
        sptr_neighbourLists)
{
    Timer timer;

    /** future knn task? */
    auto knnTask = [sptr_points3d, sptr_neighbourLists, &K](
                       int firstIndex, int lastIndex) mutable {
        /** for each point ... ?*/
        for (int pointId = firstIndex; pointId < lastIndex; ++pointId) {
            int index = 0;

            for (auto* point3d : *sptr_points3d) {
                index++;
                if (pointId == index) {
                    continue;
                }
                /** ... keep track of distance from current point
                 *      to neighbours (in neighbours' m_distance */
                (*sptr_points3d)[pointId]->m_distance
                    = (*sptr_points3d)[pointId]->distance(point3d);
            }

            /** sort neighbour points based on distance to current point */
            std::sort((*sptr_points3d).begin(), (*sptr_points3d).end(),
                comparePoints);

            tag(*sptr_points3d);

            /** ... and create a list of K neighbours for each point */
            for (int i = 0; i <= K; i++) {
                (*sptr_neighbourLists)[pointId].push_back((*sptr_points3d)[i]);
            }
        }
    };

    /** create  wrapper for asynchronous operations? */
    std::vector<std::future<void>> compute;

    /** partition the knn on threads */
    int startIndex = 0;
    int endIndex = 0;
    for (const auto& pointsPerThread : t_issue) {
        endIndex = startIndex + pointsPerThread;
        compute.push_back(
            std::async(std::launch::async, knnTask, startIndex, endIndex));
        startIndex += pointsPerThread;
    }

    /** run knn task asynchronously */
    for (auto& future : compute) {
        future.get();
    }
    LOG(INFO) << "knn computed successfully in: " << timer.getDuration();
}

#endif /* KNN_H */

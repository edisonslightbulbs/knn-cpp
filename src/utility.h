#ifndef UTILITY_H
#define UTILITY_H

#include <Eigen/Dense>
#include <memory>
#include <vector>

#include "logger.h"
#include "point3d.h"

static void show(const std::vector<Point*>& t_points3d)
{
    for (auto* point3d : t_points3d) {
        LOG(INFO) << "ID: " << point3d->m_id << " label: " << point3d->m_cluster
                  << " distance: " << point3d->m_distance
                  << " co-ordinate: " << point3d->m_x << " " << point3d->m_y
                  << point3d->m_z;
    }
}

static void show(const std::shared_ptr<std::vector<Point*>>& sptr_points3d)
{
    for (auto* point3d : *sptr_points3d) {
        LOG(INFO) << "ID: " << point3d->m_id << " label: " << point3d->m_cluster
                  << " distance: " << point3d->m_distance
                  << " co-ordinate: " << point3d->m_x << " " << point3d->m_y
                  << point3d->m_z;
    }
}

static Point3d getCentroid(std::vector<Point*>& t_points3d)
{
    /** express vec as mat */
    const int COLUMNS = 3;
    Eigen::MatrixXd points3dMat(t_points3d.size(), COLUMNS);

    int row = 0;
    for (auto* ptr_point3d : t_points3d) {
        points3dMat(row, 0) = ptr_point3d->m_x;
        points3dMat(row, 1) = ptr_point3d->m_y;
        points3dMat(row, 2) = ptr_point3d->m_z;
        row++;
    }

    return Point3d{-1, (float)points3dMat.col(0).mean(),
        (float)points3dMat.col(1).mean(), (float)points3dMat.col(2).mean()};
}

static bool comparePoints(const Point* t_point, const Point* t_other)
{
    /** return true for smaller distance.
     *    n.b., the distance stored in each point corresponds to
     *    the distance of that point from a given centroid. */
    return t_point->m_distance < t_other->m_distance;
}

static void tag(std::shared_ptr<std::vector<Point*>>& sptr_points3d)
{
    int id = 1;
    for (auto* point3d : *sptr_points3d) {
        point3d->m_id = id;
        id++;
    }
}

static void tag(std::vector<Point*>& t_points3d)
{
    int id = 1;
    for (auto* point3d : t_points3d) {
        point3d->m_id = id;
        id++;
    }
}

static std::vector<Point*> order(std::vector<Point*> t_points3d)
{
    /** evaluate centroid */
    Point3d position = getCentroid(t_points3d);
    Point* centroid = &position;
    LOG(INFO) << "centroid = " << centroid;

    /** find distance to each point from centroid */
    for (auto* point3d : t_points3d) {
        float distance = centroid->distance(point3d);
        point3d->m_distance = distance;
    }

    /** sort points based on distance to centroid */
    std::sort(t_points3d.begin(), t_points3d.end(), comparePoints);

    tag(t_points3d);
    return t_points3d;
}

std::vector<int> multithread(const int& t_numThreads, const int& t_numPoints)
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

#endif /* UTILITY_H */

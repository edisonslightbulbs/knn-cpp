#ifndef POINT3D_H
#define POINT3D_H

#include "point.h"
#include <cmath>

class Point3d : public Point {

public:
    Point3d(int t_id, float t_x, float t_y, float t_z)
        : Point(t_id, t_x, t_y, t_z)
    {
    }

    float distance(Point* ptr_point3d) override
    {
        return sqrt((ptr_point3d->m_x - m_x) * (ptr_point3d->m_x - m_x)
            + (ptr_point3d->m_y - m_y) * (ptr_point3d->m_y - m_y)
            + (ptr_point3d->m_z - m_z) * (ptr_point3d->m_z - m_z));
    }
};
#endif /* POINT3D_H */

#ifndef POINT_H
#define POINT_H

#include <cmath>
#include <iostream>

/**
 * A point may not belong to any cluster.
 * For such cases we deem the point to be
 * Either noise, or unclassified. */
const int NOISE = -2;
const int UNASSIGNED = -1;
const int UNCLASSIFIED = -1;

class Point {

private:
    bool operator==(const Point* rhs) const
    {
        if (this->m_x == rhs->m_x && this->m_y == rhs->m_y
            && this->m_z == rhs->m_z) {
            return true;
        } else {
            return false;
        }
    }
    friend std::ostream& operator<<(
        std::ostream& t_stream, const Point* ptr_point3d);

    friend std::istream& operator>>(std::istream& t_stream, Point* ptr_point3d);

public:
    int m_id;
    float m_x;
    float m_y;
    float m_z;
    int m_cluster;
    float m_distance;

    virtual float distance(Point* ptr_other) = 0;

    bool unclassified() const
    {
        if (this->m_cluster == UNCLASSIFIED || this->m_cluster == NOISE) {
            return true;
        } else {
            return false;
        }
    }

protected:
    Point()
        : m_x(0.0)
        , m_y(0.0)
        , m_z(0.0)
        , m_id(UNASSIGNED)
        , m_cluster(UNCLASSIFIED)
        , m_distance(__DBL_MAX__)
    {
    }

    Point(int t_id, float t_x, float t_y, float t_z)
        : m_x(t_x)
        , m_y(t_y)
        , m_z(t_z)
        , m_id(t_id)
        , m_cluster(UNCLASSIFIED)
        , m_distance(__DBL_MAX__)
    {
    }
};

std::ostream& operator<<(std::ostream& t_stream, const Point* ptr_point3d)
{
    t_stream << ptr_point3d->m_x << " " << ptr_point3d->m_y << " "
             << ptr_point3d->m_z;
    return t_stream;
}

std::istream& operator>>(std::istream& t_stream, Point* ptr_point3d)
{
    t_stream >> ptr_point3d->m_x >> ptr_point3d->m_y >> ptr_point3d->m_z;
    return t_stream;
}

#endif /* POINT_H */

/**
 * virtual functions
 * see:
 * https://www.geeksforgeeks.org/virtual-function-cpp/
 * date: 2020-12-10 17:11
 */

/**
 * polymorphism
 * see:
 * https://stackoverflow.com/questions/7405740/how-can-i-initialize-base-class-member-variables-in-derived-class-constructor
 * see:
 * https://www.ibm.com/support/knowledgecenter/SSLTBW_2.2.0/com.ibm.zos.v2r2.cbclx01/cplr388.htm
 * date: 2020-12-10 17:34
 */

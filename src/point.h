#include <cmath>
#include <fstream>
#include <iostream>

class Point3d {
public:
    Point3d(int id, double x, double y, double z)
        : id(id)
        , x(x)
        , y(y)
        , z(z) {};
    Point3d() { Point3d(-1, 0, 0, 0); };

    double distance(const Point3d& point) const
    {
        double x = this->x - point.x;
        double y = this->y - point.y;
        double z = this->z - point.z;
        return sqrt(x * x + y * y + z * z);
    }

    friend std::ostream& operator<<(std::ostream& out, const Point3d& point);
    friend std::istream& operator>>(std::istream& in, Point3d& point);

    int id{};
    double x{}, y{}, z{};
};

std::istream& operator>>(std::istream& is, Point3d& point)
{
    is >> point.x >> point.y >> point.z;
    return is;
}

std::ostream& operator<<(std::ostream& out, const Point3d& point)
{
    out << point.x << " " << point.y << " " << point.z;
    return out;
}

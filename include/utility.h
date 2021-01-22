#ifndef UTILITY_H
#define UTILITY_H

#include <vector>

#include "point.h"

static void show(const std::vector<Point>& t_points)
{
    for (auto point : t_points) {
        std::cout << point << std::endl;
    }
}

#endif /* UTILITY_H */

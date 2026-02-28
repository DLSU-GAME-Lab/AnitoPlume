#pragma once
#include "vcl/math/math.hpp"

struct wind_structure
{
    int intensity;
    float angle;
    vcl::vec3 wind_vector; // horizontal

    wind_structure() : intensity(0), angle(0), wind_vector(1, 0, 0) {}
    wind_structure(int intensity, int angle) : intensity(intensity), angle(angle* (3.14159 / 180))
    {}
    void recalc_wind_vector();
};

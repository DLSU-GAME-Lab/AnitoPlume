#include "angle.hpp"

namespace vcl
{
    vec3 angle_to_vector(float angle)
    {
        double radians = (angle * 3.14159) / 180;
        return vec3(cos(radians), sin(radians), 0);
    }

    float vector_to_angle(vcl::vec3 vector)
    {
        double radians = atan2(vector.y, vector.x);
        if (radians < 0) radians += (2 * 3.14159);
        return radians * (180.0 / 3.14159);
    }
}

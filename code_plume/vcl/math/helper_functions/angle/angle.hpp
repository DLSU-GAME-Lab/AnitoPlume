#pragma once
#include "vcl/math/vec/vec.hpp"

namespace vcl
{
    vec3 angle_to_vector(float angle);
    float vector_to_angle(vcl::vec3 vector);
}
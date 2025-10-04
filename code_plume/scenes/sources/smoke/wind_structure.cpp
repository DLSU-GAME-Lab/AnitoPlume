#include "wind_structure.hpp"

void wind_structure::recalc_wind_vector()
{
    wind_vector = intensity * vcl::vec3(cos(angle), sin(angle), 0);
}

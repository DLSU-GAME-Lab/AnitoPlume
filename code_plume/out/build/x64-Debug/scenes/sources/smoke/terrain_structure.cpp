#include "terrain_structure.hpp"

float terrain_structure::field_height_at(float x, float y)
{
    float interval_size = max_xyz - min_xyz;
    int idx_x = (int)(field_size * (x - min_xyz) / interval_size);
    int idx_y = (int)(field_size * (y - min_xyz) / interval_size);
    return height_field(idx_x, idx_y);
}

vcl::vec3 terrain_structure::field_normal_at(float x, float y)
{
    float interval_size = max_xyz - min_xyz;
    int idx_x = (int)(field_size * (x - min_xyz) / interval_size);
    int idx_y = (int)(field_size * (y - min_xyz) / interval_size);
    return normal_field(idx_x, idx_y);
}

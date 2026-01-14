#include "terrain_structure.hpp"

void terrain_structure::fill_height_field(vcl::buffer<vcl::vec3>& position, vcl::buffer<vcl::vec3>& normal, vcl::mesh_drawable terrain)
{
    // prepare field with parameters
    min_xyz = -15000.0f;
    max_xyz = 15000.0f;
    float interval_size = max_xyz - min_xyz;
    cell_size = 200.f;
    field_size = (size_t)(max_xyz / cell_size);
    height_field.resize(field_size, field_size);
    normal_field.resize(field_size, field_size);

    //transform like for mesh_drawable
    vcl::mat3 rotation = terrain.uniform.transform.rotation;
    vcl::vec3 translation = terrain.uniform.transform.translation;
    float scaling = terrain.uniform.transform.scaling;
    for (unsigned int i = 0; i < position.size(); i++)
    {
        position[i] = scaling * (rotation * position[i] + translation);
        normal[i] = rotation * normal[i];
    }
    positions = position;
    normals = normal;


    //fill height field for collisions
    for (unsigned int i = 0; i < position.size(); i++)
    {
        if (position[i].x < max_xyz && position[i].x > min_xyz
            && position[i].y < max_xyz && position[i].y > min_xyz)
        {
            int idx_x = (int)(field_size * (position[i].x - min_xyz) / interval_size);
            int idx_y = (int)(field_size * (position[i].y - min_xyz) / interval_size);
            if (idx_x == field_size) idx_x = field_size - 1;
            if (idx_y == field_size) idx_y = field_size - 1;
            height_field(idx_x, idx_y) = position[i].z;
            normal_field(idx_x, idx_y) = normal[i];
        }
    }
}

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

#pragma once

#include "vcl/shape/shape.hpp"
#include "vcl/containers/containers.hpp"

// Terrain grid for acceleration collision computation
struct terrain_structure
{
    vcl::buffer<vcl::vec3> positions;
    vcl::buffer<vcl::vec3> normals;

    vcl::buffer2D< std::vector<unsigned int> > grid;
    size_t grid_size;

    vcl::buffer2D<float> height_field;
    vcl::mesh_drawable height_field_mesh;
    vcl::buffer2D<vcl::vec3> normal_field;
    float cell_size;
    size_t field_size;
    float min_xyz;
    float max_xyz;

    // Fill structures
    void fill_height_field(vcl::buffer<vcl::vec3>& position, vcl::buffer<vcl::vec3>& normal,
        vcl::mesh_drawable terrain);

    float field_height_at(float x, float y);
    vcl::vec3 field_normal_at(float x, float y);
};

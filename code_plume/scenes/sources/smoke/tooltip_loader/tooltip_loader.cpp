#include "tooltip_loader.hpp"
using namespace vcl;
void tooltip_loader::load_all_textures()
{
    const char* tooltips[] = { "Tooltip-Balantoc", "Tooltip-Malaki",
    "Tooltip-Munti", "Tooltip-Piraso", "Tooltip-Minimized" };
    for (int i = 0; i < 5; i++)
    {
        std::string name = tooltips[i];
        std::string texture_path = "../scenes/sources/smoke/tooltips/"+ name + ".png";
        texture_id[i] = create_texture_gpu(image_load_png(texture_path));
    }
    current_tex_id = texture_id[0];
}
void tooltip_loader::load_tooltip(std::string terrain_filename, std::string texture_filename, bool isTrans)
{
    std::string tooltip_path = "../scenes/sources/smoke/tooltips/" + terrain_filename;
    std::string texture_path = "../scenes/sources/smoke/tooltips/" + texture_filename;

    //if (check_file_exist(terrain_path) && check_file_exist(texture_path))
    //{
    //    load_terrain(terrain_filename, texture_filename);
    //    load_texture(texture_path);
    //}
    //else
    //{
    //    std::cout << "Incorrect filename!" << "\n";
    //    return;
    //}

    std::cout << "TOOLTIP LOADER: Loading tooltip..." << "\n";

    //mesh_load_file_obj_async(terrain_path, mesh_terrain);

    // Terrain setup
    mesh mesh_terrain = mesh_load_file_obj(tooltip_path);
    tooltip = mesh_drawable(mesh_terrain);
    tooltip.shader = mesh_shader;
    tooltip.uniform.color = { 1,1,1 };
    //tooltip.uniform.shading.specular = 1.0f;
    tooltip.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
    tooltip.uniform.transform.scaling = 1.f;
    tooltip.uniform.transform.translation = { 0.f,0.f,0.f };
    if (isTrans)
        tooltip.uniform.color_alpha = 0.f;
    else
        tooltip.uniform.color_alpha = 1.f;

    tooltip.texture_id = create_texture_gpu(image_load_png(texture_path));

    //if (current_terrain != terrain_filename)
    //{
    //    current_terrain = terrain_filename;
    //    mesh mesh_terrain = mesh_load_file_obj(terrain_path);
    //    terrain = mesh_drawable(mesh_terrain);
    //    terrain.shader = mesh_shader;
    //    terrain.uniform.color = { 1,1,1 };
    //    terrain.uniform.shading.specular = 0.0f;
    //    terrain.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
    //    terrain.uniform.transform.scaling = 1.f;
    //    terrain.uniform.transform.translation = { 0.f,0.f,0.f };

    //}

    //if (current_texture != texture_filename)
    //{
    //    current_texture = texture_filename;
    //    terrain.texture_id = create_texture_gpu(image_load_png(texture_path));
    //}
    //
    new_tooltip_loaded = true;
}

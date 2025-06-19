#include "landmark_loader.hpp"
using namespace vcl;
void landmark_loader::load_all_textures()
{

}
void landmark_loader::load_landmark(std::string terrain_filename, std::string texture_filename, bool isTrans)
{
    std::string tooltip_path = "../scenes/sources/smoke/Landmarks/" + terrain_filename;
    std::string texture_path = "../scenes/sources/smoke/Landmarks/" + texture_filename;



    std::cout << "TOOLTIP LOADER: Loading landmark..." << "\n";

   

    // landmark setup
    mesh mesh_terrain = mesh_load_file_obj(tooltip_path);
    tooltip = mesh_drawable(mesh_terrain);
    tooltip.shader = mesh_shader;
    tooltip.uniform.color = { 1,1,1 };
    tooltip.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
    tooltip.uniform.transform.scaling = 1.f;
    tooltip.uniform.transform.translation = { 0.f,0.f,0.f };
    if (isTrans)
        tooltip.uniform.color_alpha = 0.f;
    else
        tooltip.uniform.color_alpha = 1.f;

    tooltip.texture_id = create_texture_gpu(image_load_png(texture_path));

   
    new_tooltip_loaded = true;
}

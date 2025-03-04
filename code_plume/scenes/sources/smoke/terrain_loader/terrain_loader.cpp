#include "terrain_loader.hpp"

using namespace vcl;

void terrain_loader::load_all_textures()
{
    const char* years[]{ "2023", "2021", "2019", "2016", "2015" };
    for (int i = 0; i < 5; i++)
    {
        std::string year = years[i];
        std::string texture_path = "../scenes/sources/smoke/textures/Taal_Texture_" + year + ".png";
        texture_id[i] = create_texture_gpu(image_load_png(texture_path));
    }
    for (int i = 0; i < 5; i++)
    {
        std::string year = years[i];
        std::string normal_path = "../scenes/sources/smoke/textures/Taal_Normal_" + year + ".png";
        normal_id[i] = create_texture_gpu(image_load_png(normal_path));
    }

    current_tex_id = texture_id[0];
    current_norm_id = normal_id[0];
}

void terrain_loader::show_gui()
{
    ImGui::Begin("Terrain", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    static const char* labels[]{ "2023", "2021", "2019", "2016", "2015" };
    if (ImGui::Combo("Year", &current_tex, labels, IM_ARRAYSIZE(labels)))
    {
        current_tex_id = texture_id[current_tex];
        current_norm_id = normal_id[current_tex];
    }

    //ImGui::InputText("Terrain", ter_input, size, ImGuiInputTextFlags_AutoSelectAll);
    //ImGui::InputText("Texture", tex_input, size, ImGuiInputTextFlags_AutoSelectAll);
    //if (ImGui::Button("Load Terrain"))
    //{
    //    std::string terrain_filename = ter_input;
    //    std::string texture_filename = tex_input;
    //}

    ImGui::End();
}

void terrain_loader::load_terrain(std::string terrain_filename, std::string texture_filename, bool isTrans)
{
    std::string terrain_path = "../scenes/sources/smoke/terrains/" + terrain_filename;
    std::string texture_path = "../scenes/sources/smoke/textures/" + texture_filename;

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

    std::cout << "TERRAIN LOADER: Loading terrain..." << "\n";

    //mesh_load_file_obj_async(terrain_path, mesh_terrain);

    // Terrain setup
    mesh mesh_terrain = mesh_load_file_obj(terrain_path);
    terrain = mesh_drawable(mesh_terrain);
    terrain.shader = mesh_shader;
    terrain.uniform.color = { 1,1,1 };
    terrain.uniform.shading.specular = 0.0f;
    terrain.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
    terrain.uniform.transform.scaling = 1.f;
    terrain.uniform.transform.translation = { 0.f,0.f,0.f };
    if (isTrans)
        terrain.uniform.color_alpha = 0.f;
    else
        terrain.uniform.color_alpha = 1.f;

    //terrain.texture_id = create_texture_gpu(image_load_png(texture_path));

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
    new_terrain_loaded = true;
}

void terrain_loader::load_terrain_async(std::string terrain_filename, std::string texture_filename)
{
    //std::thread(&terrain_loader::load_terrain, this, terrain_filename, texture_filename).detach();
}

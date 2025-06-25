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

void terrain_loader::show_gui(bool* show)
{
    ImGui::Begin("Terrain", &enabled, ImGuiWindowFlags_AlwaysAutoResize);
    *show = enabled;

    //static const char* labels[]{ "2023", "2021", "2019", "2016", "2015" };
    //if (ImGui::Combo("Year", &current_tex, labels, IM_ARRAYSIZE(labels)))
    //{
    //    current_tex_id = texture_id[current_tex];
    //    current_norm_id = normal_id[current_tex];
    //}

    vec3 t_pos = terrain.uniform.transform.translation;
    float pos[3] = { t_pos.x, t_pos.y, t_pos.z };
    float scale = terrain.uniform.transform.scaling;

    if (ImGui::InputFloat3("Position", pos, 2))
    {
        terrain.uniform.transform.translation = vec3({ pos[0], pos[1], pos[2] });
    }

    if (ImGui::InputFloat("Scale", &scale, 0.1f, 0.5f, "%.2f"))
    {
        terrain.uniform.transform.scaling = scale;
    }

    ImGui::NewLine();

    ImGui::InputText("Model", ter_input, size);
    ImGui::InputText("Texture", tex_input, size);

    if (ImGui::Button("Reload Terrain"))
    {
        std::string terrain_filename = ter_input;
        std::string texture_filename = tex_input;
        load_terrain(terrain_filename, texture_filename);
    }

    ImGui::End();
}

void terrain_loader::load_terrain(std::string terrain_filename, std::string texture_filename)
{
    std::string terrain_path = "../scenes/sources/smoke/terrains/" + terrain_filename;
    std::string texture_path = "../scenes/sources/smoke/textures/" + texture_filename;

    if (!check_file_exist(terrain_path) || !check_file_exist(texture_path))
    {
        std::cout << "Incorrect filename!" << "\n";
        return;
    }

    std::cout << "TERRAIN LOADER: Loading terrain..." << "\n";

    // Terrain setup
    if (current_terrain != terrain_filename)
    {
        mesh mesh_terrain = mesh_load_file_obj(terrain_path);
        terrain = mesh_drawable(mesh_terrain);
        terrain.shader = mesh_shader;
        terrain.uniform.color = { 1,1,1 };
        terrain.uniform.shading.specular = 0.0f;
        terrain.uniform.shading.diffuse = 1.0f;
        terrain.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
        terrain.uniform.transform.scaling = 0.25f;
        terrain.uniform.transform.translation = { 0.f,0.f,-10.f };
        std::cout << "TERRAIN LOADER: Terrain model loaded!" << "\n";
    }
    else std::cout << "TERRAIN LOADER: Terrain model " << terrain_filename << " already loaded." << "\n";

    if (current_texture != texture_filename)
    {
        current_texture = texture_filename;
        current_tex_id = create_texture_gpu(image_load_png(texture_path));
        terrain.texture_id = current_tex_id;
        std::cout << "TERRAIN LOADER: Terrain texture loaded!" << "\n";
    }
    else std::cout << "TERRAIN LOADER: Terrain texture " << texture_filename << " already loaded." << "\n";
    
    current_terrain = terrain_filename;
    current_texture = texture_filename;

    new_terrain_loaded = true;
}

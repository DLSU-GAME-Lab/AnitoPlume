#include "terrain_loader.hpp"
#include "singleton/TextureManager.hpp"

using namespace vcl;

void terrain_loader::load_terrain(std::string terrain_filename, std::string texture_filename)
{
    std::string terrain_path = "../assets/terrains/" + terrain_filename;
    std::string texture_path = "../assets/textures/" + texture_filename;

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
    terrain.uniform.shading.diffuse = 1.0f;
    terrain.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
    terrain.uniform.transform.scaling = 1.f;
    terrain.uniform.transform.translation = { 0.f,0.f,-7.f };
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

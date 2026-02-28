#include "tooltip_loader.hpp"
#include "singleton/MeshManager.hpp"
#include "singleton/ShaderManager.hpp"
#include "singleton/CameraManager.hpp"

using namespace vcl;

void tooltip_loader::setup_tooltips()
{
    std::string tooltip_path = "../assets/tooltips/Tooltip.obj";

    std::cout << "TOOLTIP LOADER: Loading tooltip..." << "\n";
    
    MeshManager::getInstance()->load("tooltip", tooltip_path);
    tooltip = MeshManager::getInstance()->getMesh("tooltip");
    shader = ShaderManager::getInstance()->getShader("mesh");

    std::string tooltips[] = {
        "Tooltip-Balantoc",
        "Tooltip-Malaki",
        "Tooltip-Munti",
        "Tooltip-Piraso",
        "Tooltip-Minimized"
    };

    for (int i = 0; i < 5; i++)
    {
        std::string name = tooltips[i];
        std::string texture_path = "../assets/tooltips/" + name + ".png";
        texture_id[i] = create_texture_gpu(image_load_png(texture_path));
        opengl_debug();
    }

    for (int i = 0; i < 4; i++)
    {
        tooltip_display.push_back(*tooltip);
        tooltip_display[i].shader = shader;
        tooltip_display[i].uniform.transform.scaling = 4.f;
        tooltip_display[i].uniform.shading.ambiant = 1.f;
        tooltip_display[i].uniform.color = { 1,1,1 };
        tooltip_display[i].uniform.color_alpha = 1.f;
        tooltip_display[i].uniform.shading.ambiant = 1.f;
        tooltip_display[i].uniform.shading.specular = 0.0f;
        tooltip_display[i].uniform.transform.scaling = 4.f;
    }

    tooltip_display[0].uniform.transform.translation = { -55.f,55.f,-2.f };
    tooltip_display[1].uniform.transform.translation = { -53.f,57.f,-10.f };
    tooltip_display[2].uniform.transform.translation = { -42.f,-60.f,-10.f };
    tooltip_display[3].uniform.transform.translation = { 37.f,60.f,-10.f };
}

void tooltip_loader::draw()
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    vcl::mat4 cam_mat = camera->camera_matrix();
    vcl::vec3 cam_pos = { cam_mat.xw, cam_mat.yw, cam_mat.zw };
    if (cam_pos.x != last_cam_pos.x || cam_pos.y != last_cam_pos.y || cam_pos.z != last_cam_pos.z)
    {
        last_cam_pos = cam_pos;
        std::sort(tooltip_display.begin(), tooltip_display.end(),
            [cam_pos](mesh_drawable a, mesh_drawable b)
            {
                float distA = vcl::sqr_mag(a.uniform.transform.translation - cam_pos);
                float distB = vcl::sqr_mag(a.uniform.transform.translation - cam_pos);
                return distA > distB;
            });
    }

    glDepthMask(false);
    for (int i = 0; i < 4; i++)
    {
        vec3 tt_vec = tooltip_display[i].uniform.transform.translation + camera->translation;
        float sqr_mag = (tt_vec.x * tt_vec.x) + (tt_vec.y * tt_vec.y) + (tt_vec.z * tt_vec.z);
        tooltip_display[i].uniform.transform.rotation = camera->orientation;

        if (sqr_mag <= tooltip_dist * tooltip_dist)
            tooltip_display[i].texture_id = texture_id[i];
        else
            tooltip_display[i].texture_id = texture_id[4];

        tooltip_display[i].draw(*camera);

    }


    glDepthMask(true);
}

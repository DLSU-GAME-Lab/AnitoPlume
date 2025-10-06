#include "landmark_loader.hpp"
#include "singleton/MeshManager.hpp"
#include "singleton/ShaderManager.hpp"
#include "singleton/CameraManager.hpp"

using namespace vcl;

void landmark_loader::setup_landmarks()
{
    std::string landmark_names[] = {
        "Landmark_Lipa",
        "Landmark_SantaTeresita",
        "Landmark_Tagaytay",
        "Landmark_Tanauan",
        "Landmark-Talisay",
        "Landmark-Agoncillo",
        "Landmark-Alitagtag",
        "Landmark-Balete",
        "Landmark-Cuenca",
        "Landmark-Laurel",
        "Landmark-Mataasnakahoy"
    };

    vec3 landmark_pos[] = {
    { 285.f,-130.f,5.f }, // lipa
    { -35,-215.f,5.f }, // sta terisita
    { -100,255.f,5.f }, //tagaytay
    { 255,130.f,5.f }, // tanauan
    { 0,150,5.f }, //talisay
    { -100,0,5.f }, //Agoncillo
    { 0,-300,5.f }, //Alitagtag
    { 255,50,5.f }, //Balete
    { 100,-250,5.f }, //Cuenca
    { -125,100.f,5.f }, //Laurel
    { 225, -50.f,5.f } //Mataas na Kahoy
    };

    std::cout << "LANDMARK LOADER: Loading landmarks..." << "\n";

    MeshManager::getInstance()->load("Landmark", "../scenes/sources/smoke/Landmarks/Landmark.obj");
    landmark = MeshManager::getInstance()->getMesh("Landmark");
    shader = ShaderManager::getInstance()->getShader("mesh");
    
    for (int i = 0; i < landmark_count; i++)
    {
        std::string texture_path = "../scenes/sources/smoke/Landmarks/" + landmark_names[i] + ".png";
        texture_id[i] = create_texture_gpu(image_load_png(texture_path));
        opengl_debug();
    }

    for (int i = 0; i < landmark_count; i++)
    {
        landmark_display[i].texture_id = texture_id[i];
        landmark_display[i] = *landmark;
        landmark_display[i].shader = shader;
        landmark_display[i].uniform.color = { 1,1,1 };
        landmark_display[i].uniform.color_alpha = 1.f;
        landmark_display[i].uniform.transform.scaling = 10.f;
        landmark_display[i].uniform.shading.ambiant = 1.f;
        landmark_display[i].uniform.shading.specular = 0.0f;
        landmark_display[i].uniform.transform.translation = landmark_pos[i];
    }
}

void landmark_loader::draw()
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    glDepthMask(false);
    for (int i = 0; i < landmark_count; i++)
    {
        vec3 lm_vec = landmark_display[i].uniform.transform.translation + camera->translation;
        float sqr_mag = (lm_vec.x * lm_vec.x) + (lm_vec.y * lm_vec.y) + (lm_vec.z * lm_vec.z);
        float alpha = 1.0f;

        if (sqr_mag < landmark_min_dist * landmark_min_dist)
        {
            alpha = (sqr_mag - (landmark_max_dist * landmark_max_dist)) / (landmark_min_dist * landmark_min_dist);
            if (alpha < 0.0f) alpha = 0.0f;
        }
        landmark_display[i].uniform.color_alpha = alpha;
        landmark_display[i].uniform.transform.rotation = camera->orientation;
        landmark_display[i].texture_id = texture_id[i];

        landmark_display[i].draw(*camera);
    }
    glDepthMask(true);
}

#include "camera_control_glfw.hpp"


namespace vcl
{


enum class camera_action_state {none, translation, rotation, scale, translation_depth};

void camera_control_glfw::update_mouse_move(camera_scene& camera, GLFWwindow* window, float x1, float y1)
{
    assert(window!=nullptr);

    // Get type of click (left, right), is CTRL pressed
    const bool mouse_click_left  = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT )==GLFW_PRESS);
    const bool mouse_click_right = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS);
    const bool key_ctrl = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL));
    const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

    if(update==false || key_shift)
    {
        x0 = x1;
        y0 = y1;
        return;
    }




    // Get window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);

    // Set action state accordingly
    camera_action_state state = camera_action_state::none;

    if(!mouse_click_left && !mouse_click_right)
        state = camera_action_state::none;
    else if( mouse_click_left && !key_ctrl )
        state = camera_action_state::rotation;
    else if( mouse_click_left && key_ctrl )
        state = camera_action_state::translation;
    else if( mouse_click_right && !key_ctrl )
        state = camera_action_state::scale;
    else if( mouse_click_right && key_ctrl )
        state = camera_action_state::translation_depth;


    // ************************************************* //
    // Compute transformation to apply on the camera
    // ************************************************* //
    if( state == camera_action_state::translation ) {

        const float tx =  (x1-x0)/w;
        const float ty = -(y1-y0)/h;
        camera.apply_translation_in_screen_plane( tx, ty );

    }

    else if( state == camera_action_state::rotation ) {

        // relative position on screen
        const float ux0 = 2*x0/float(w)-1;
        const float uy0 = 1-2*y0/float(h);

        const float ux1 = 2*x1/float(w)-1;
        const float uy1 = 1-2*y1/float(h);

        // apply rotation
        camera.apply_rotation(ux0, uy0, ux1, uy1);
    }

    else if ( state == camera_action_state::scale ) {

        const float scale_magnitude = (y0-y1)/h;

        camera.apply_scaling( scale_magnitude );
    }

    else if ( state == camera_action_state::translation_depth ) {
        const float translation_magnitude = (y1-y0)/h;

        camera.apply_translation_orthogonal_to_screen_plane( translation_magnitude );
    }


    // Update previous click position
    x0 = x1;
    y0 = y1;

}


void camera_control_glfw::update_mouse_click(camera_scene& , GLFWwindow* , int , int , int )
{

}

void camera_control_glfw::update_mouse_scroll(camera_scene& camera, GLFWwindow* window, float xoffset, float yoffset)
{
    if (yoffset == 0.0f || camera.mode != view_mode::orbital)
    {
        return;
    }

    float new_distance = camera.scale0 - (yoffset * scroll_speed * dt);

    if (new_distance < orbit_min) new_distance = orbit_min;
    else if (new_distance > orbit_max) new_distance = orbit_max;
    else orbit_distance = new_distance;

    camera.set_scale(orbit_distance);
}

void camera_control_glfw::update_timer()
{
    dt = timer.update();
}

void camera_control_glfw::update_rotate(camera_scene& camera, GLFWwindow* window, float x1, float y1)
{
    assert(window != nullptr);

    // Get type of click (left, right), is CTRL pressed
    const bool mouse_click_right = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

    if (update == false || key_shift)
    {
        x0 = x1;
        y0 = y1;
        return;
    }

    // Get window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);

    // Set action state accordingly
    camera_action_state state = camera_action_state::none;

    if (!mouse_click_right)
        state = camera_action_state::none;
    else if (mouse_click_right)
        state = camera_action_state::rotation;


    // ************************************************* //
    // Compute transformation to apply on the camera
    // ************************************************* //
    if (state == camera_action_state::rotation)
    {

        // relative position on screen
        const float ux0 = 2 * x0 / float(w) - 1;
        const float uy0 = 1 - 2 * y0 / float(h);

        const float ux1 = 2 * x1 / float(w) - 1;
        const float uy1 = 1 - 2 * y1 / float(h);

        // apply rotation
        camera.apply_rotation(ux0, uy0, ux1, uy1);
    }

    // Update previous click position
    x0 = x1;
    y0 = y1;
}

void camera_control_glfw::update_move(terrain_structure& terrain_struct, camera_scene& camera, GLFWwindow* window, int key, int scancode, int action, int mods)
{
    assert(window != nullptr);

    const bool upward = (glfwGetKey(window, GLFW_KEY_E) != GLFW_RELEASE);
    const bool downward = (glfwGetKey(window, GLFW_KEY_Q) != GLFW_RELEASE);
    const bool rightward = (glfwGetKey(window, GLFW_KEY_D) != GLFW_RELEASE);
    const bool leftward = (glfwGetKey(window, GLFW_KEY_A) != GLFW_RELEASE);
    const bool forward = (glfwGetKey(window, GLFW_KEY_W) != GLFW_RELEASE);
    const bool backward = (glfwGetKey(window, GLFW_KEY_S) != GLFW_RELEASE);

    
    if (update == false || camera.mode == view_mode::orbital)
    {
        return;
    }

    float upwardSpeed = 0.0f;
    float rightwardSpeed = 0.0f;
    float forwardSpeed = 0.0f;

    if (upward && !downward)
    {
        upwardSpeed = -speed;
    }
    else if (downward && !upward)
    {
        upwardSpeed = speed;
    }

    if (rightward && !leftward)
    {
        rightwardSpeed = -speed;
    }
    else if (leftward && !rightward)
    {
        rightwardSpeed = speed;
    }

    if (forward && !backward)
    {
        forwardSpeed = speed;
    }
    else if (backward && !forward)
    {
        forwardSpeed = -speed;
    }

    // ************************************************* //
    // Compute transformation to apply on the camera
    // ************************************************* //

    const float tx = rightwardSpeed * dt;
    camera.apply_translation_in_screen_plane(tx, 0.0f);

    const float ty = upwardSpeed * dt;
    camera.apply_translation_in_world_axis(0.0f, 0.0f, ty);

    const float translation_magnitude = forwardSpeed * dt;
    camera.apply_translation_orthogonal_to_screen_plane(translation_magnitude);

    //float height = terrain_struct.field_height_at(camera.translation.x, camera.translation.y);
    //std::cout << "position: (" << camera.translation.x << ", " << camera.translation.y << ") field height: " << height << "\n";

    //height = (height / 100.0f) + eye_line;
    //camera.snap_to_height(height);

}




}

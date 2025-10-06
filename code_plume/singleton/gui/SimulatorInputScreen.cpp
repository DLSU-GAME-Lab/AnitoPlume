#include "SimulatorInputScreen.hpp"

SimulatorInputScreen::SimulatorInputScreen() : GUIScreen("SIMULATOR_INPUT_SCREEN")
{

}

SimulatorInputScreen::~SimulatorInputScreen()
{

}

void SimulatorInputScreen::drawGUI()
{

    //ImGui::Begin("Simulator Input", &enabled, ImGuiWindowFlags_AlwaysAutoResize);

    //ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 5);
    //ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 0.1f));
    //ImGui::PushItemWidth(200);

    //const float indent_width = 5;
    //const float child_width = 380;

    //// Can set the speed of the animation
    //float scale_min = 0.05f;
    //float scale_max = 5.0f;
    //ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");

    //// Parameters
    //unsigned int spheres_min = 0, spheres_max = 500;
    //ImGui::SliderScalar("Number of subspheres", ImGuiDataType_S32, &plume.subspheres_number, &spheres_min, &spheres_max);
    //ImGui::SliderScalar("Number of subsubspheres", ImGuiDataType_S32, &plume.subsubspheres_number, &spheres_min, &spheres_max);
    //ImGui::PopItemWidth();

    //if (ImGui::CollapsingHeader("Display Settings", ImGuiTreeNodeFlags_DefaultOpen))
    //{
    //    ImGui::BeginChild("Display", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 5.25f));
    //    ImGui::Spacing();
    //    ImGui::Indent(indent_width);

    //    ImGui::Checkbox("Display billboards", &gui_param.display_billboards);
    //    ImGui::Checkbox("Display torus layers", &gui_param.display_smoke_layers);
    //    ImGui::Checkbox("Display free spheres", &gui_param.display_free_spheres);
    //    //ImGui::Checkbox("Display subspheres", &gui_param.display_subspheres);
    //    ImGui::Checkbox("Display spheres with subspheres", &gui_param.display_spheres_with_subspheres);
    //    ImGui::Checkbox("Display Tooltips", &gui_param.display_tooltips);
    //    ImGui::Unindent();
    //    ImGui::EndChild();
    //}

    //// Coeffs

    ////float air_inc_min = 0.5, air_inc_max = 10.;
    ////ImGui::SliderScalar("Air incorporation coefficient", ImGuiDataType_Float, &air_incorporation_coeff, &air_inc_min, &air_inc_max, "%.2f");

    //// Initial conditions
    //if (ImGui::CollapsingHeader("Eruption Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    //{
    //    ImGui::BeginChild("Parameters", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 4.25f));
    //    ImGui::Spacing();
    //    ImGui::Indent(indent_width);
    //    ImGui::PushItemWidth(200);

    //    float initial_speed_min = 0., initial_speed_max = 200.;
    //    ImGui::SliderScalar("Initial plume speed", ImGuiDataType_Float, &plume.U_0, &initial_speed_min, &initial_speed_max, "%.2f m/s");
    //    float initial_density_min = 150., initial_density_max = 250.;
    //    ImGui::SliderScalar("Initial plume density", ImGuiDataType_Float, &plume.rho_0, &initial_density_min, &initial_density_max, "%.2f kg/m3");
    //    float vent_ray_min = 50., vent_ray_max = 200.;
    //    ImGui::SliderScalar("Vent radius", ImGuiDataType_Float, &plume.r_0, &vent_ray_min, &vent_ray_max, "%.2f m");
    //    float vent_altitude_min = 0., vent_altitude_max = 8000.;
    //    ImGui::SliderScalar("Vent altitude", ImGuiDataType_Float, &plume.z_0, &vent_altitude_min, &vent_altitude_max, "%.2f m");

    //    ImGui::PopItemWidth();
    //    ImGui::Unindent();
    //    ImGui::EndChild();

    //}
    //// Wind presets
    //if (ImGui::CollapsingHeader("Wind Settings", ImGuiTreeNodeFlags_DefaultOpen))
    //{
    //    ImGui::BeginChild("Wind", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 13.5f));
    //    ImGui::Spacing();
    //    ImGui::Indent(indent_width);
    //    ImGui::PushItemWidth(200);

    //    float intensity[wind_size] = {};
    //    float angle[wind_size] = {};

    //    for (int i = 0; i < wind_size; i++)
    //    {
    //        intensity[i] = winds[i].intensity;
    //        angle[i] = this->deg_angle[i];
    //    }

    //    bool altitude_selected = false;

    //    const int x_offset = ImGui::GetCursorScreenPos().x;
    //    const int plot_start = ImGui::GetCursorScreenPos().y;

    //    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    //    const float plot_grid_offset = x_offset + slider_width + 11;
    //    for (int i = 0; i < 3; i++)
    //    {
    //        for (int j = 1; j < wind_size - 1; j++)
    //        {
    //            float plot_division = (float)j / (wind_size - 1);
    //            float plot_grid = (plot_division * (plot_width - 8)) + plot_grid_offset;

    //            float total_height = plot_start + plot_height * i + (i * 3);
    //            ImVec2 start = ImVec2(plot_grid, total_height);
    //            ImVec2 end;

    //            if (i == 2) end = ImVec2(plot_grid, total_height + 22);
    //            else end = ImVec2(plot_grid, total_height + plot_height);

    //            draw_list->AddLine(start, end, IM_COL32(255, 255, 255, 100), 1.0f);
    //        }
    //    }

    //    if (ImGui::VSliderScalar("##Intensity Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &winds[selected].intensity, &wind_min, &wind_max))
    //    {
    //        winds[selected] = wind_structure(winds[selected].intensity, this->deg_angle[selected]);
    //        winds[selected].recalc_wind_vector();
    //        calculate_avg_wind_dir();
    //    }

    //    ImGui::SameLine();
    //    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
    //    ImGui::PlotLines("##Wind Intensity", intensity, wind_size, 0, "Wind Intensity (m/s)", wind_min, wind_max, ImVec2(plot_width, plot_height));
    //    ImGui::PopStyleColor();

    //    if (ImGui::VSliderScalar("##Angle Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &this->deg_angle[selected], &angle_min, &angle_max))
    //    {
    //        if (all_angles)
    //        {
    //            for (int i = 0; i < wind_size; i++)
    //            {
    //                this->deg_angle[i] = this->deg_angle[selected];
    //                winds[i] = wind_structure(winds[i].intensity, this->deg_angle[i]);
    //                winds[i].recalc_wind_vector();
    //            }
    //        }
    //        else
    //        {
    //            winds[selected] = wind_structure(winds[selected].intensity, this->deg_angle[selected]);
    //            winds[selected].recalc_wind_vector();
    //        }
    //        calculate_avg_wind_dir();
    //    }
    //    ImGui::SameLine();
    //    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.1f, 0.8f, 0.1f, 1.0f));
    //    ImGui::PlotLines("##Wind Angle", angle, wind_size, 0, "Wind Angle (degrees)", angle_min, angle_max, ImVec2(plot_width, plot_height));
    //    ImGui::PopStyleColor();

    //    float plot_div = (float)selected / (wind_size - 1);
    //    float plot_x = (plot_div * (plot_width - 8)) + plot_grid_offset;

    //    ImVec2 start = ImVec2(plot_x, plot_start);
    //    ImVec2 end = ImVec2(plot_x, plot_start + (plot_height * 2) + 3);
    //    draw_list->AddLine(start, end, IM_COL32(240, 220, 40, 255), 3.0f);

    //    ImGui::Indent(indent_w);
    //    ImGui::PushItemWidth(plot_width + 7);
    //    if (ImGui::SliderScalar("##Altitude", ImGuiDataType_S32, &wind_alt, &alt_min, &alt_max, "%d meters in altitude"))
    //    {
    //        for (int i = 0; i < wind_size && !altitude_selected; i++)
    //        {
    //            if (wind_altitudes[i] == wind_alt)
    //            {
    //                altitude_selected = true;
    //                selected = i;
    //            }
    //        }

    //        if (!altitude_selected)
    //        {
    //            selected = clamp(((float)wind_alt / altitude_step) + 0.5f, 0, wind_size - 1);
    //            wind_alt = wind_altitudes[selected];
    //        }
    //    }
    //    ImGui::PopItemWidth();
    //    ImGui::Unindent(indent_w);
    //    ImGui::Spacing();

    //    if (ImGui::Button("No wind"))
    //    {
    //        is_wind = false;
    //        for (unsigned int i = 0; i < winds.size(); i++)
    //        {
    //            this->deg_angle[i] = angle_min;
    //            winds[i] = wind_structure(wind_min, this->deg_angle[i]);
    //            winds[i].recalc_wind_vector();
    //        }
    //        calculate_avg_wind_dir();
    //    }

    //    ImGui::SameLine();
    //    if (ImGui::Button("Linear Wind"))
    //    {
    //        is_wind = true;
    //        for (unsigned int i = 0; i < winds.size(); i++)
    //        {
    //            winds[i].intensity = i * linear_wind_base;
    //            if (i > 3) winds[i].intensity = 3 * linear_wind_base;
    //            if (winds[i].intensity == wind_min) winds[i].intensity = 1;
    //            winds[i].recalc_wind_vector();
    //        }
    //        calculate_avg_wind_dir();
    //    }

    //    ImGui::SameLine();
    //    if (ImGui::Button("Max Intensity"))
    //    {
    //        is_wind = true;
    //        for (unsigned int i = 0; i < winds.size(); i++)
    //        {
    //            winds[i].intensity = wind_max;
    //            winds[i].recalc_wind_vector();
    //        }
    //        calculate_avg_wind_dir();
    //    }

    //    ImGui::SameLine();
    //    ImGui::Checkbox("All Angles", &all_angles);

    //    // Wind
    //    float lin_windbase_min = 0., lin_windbase_max = 35.;
    //    if (ImGui::SliderScalar("Linear wind speed", ImGuiDataType_Float, &linear_wind_base, &lin_windbase_min, &lin_windbase_max, "%1.f m/s"))
    //    {
    //        if (is_wind)
    //        {
    //            for (unsigned int i = 0; i < winds.size(); i++)
    //            {
    //                winds[i].intensity = i * linear_wind_base;
    //                if (i > 3) winds[i].intensity = 3 * linear_wind_base;
    //                if (winds[i].intensity == wind_min) winds[i].intensity = 1;
    //                winds[i] = wind_structure(winds[i].intensity, this->deg_angle[i]);
    //                winds[i].recalc_wind_vector();
    //            }
    //            calculate_avg_wind_dir();
    //        }
    //    }
    //    ImGui::PopItemWidth();

    //    if (ImGui::Button("Set to 2020 Eruption Params"))
    //    {
    //        //U_0 = 200;
    //        //rho_0 = 250;

    //        winds[0].intensity = 1;
    //        winds[1].intensity = 14;
    //        winds[2].intensity = 20;
    //        winds[3].intensity = 30;
    //        winds[4].intensity = 40;
    //        winds[5].intensity = 58;

    //        this->deg_angle[0] = 0;
    //        this->deg_angle[1] = 30;
    //        this->deg_angle[2] = 330;
    //        this->deg_angle[3] = 90;
    //        this->deg_angle[4] = 120;
    //        this->deg_angle[5] = 135;

    //        for (unsigned int i = 0; i < winds.size(); i++)
    //        {
    //            winds[i] = wind_structure(winds[i].intensity, this->deg_angle[i]);
    //            winds[i].recalc_wind_vector();
    //        }
    //        calculate_avg_wind_dir();
    //    }

    //    ImGui::Unindent();
    //    ImGui::EndChild();
    //}

    //ImGui::PopStyleColor();
    //ImGui::PopStyleVar();
    //ImGui::End();

}

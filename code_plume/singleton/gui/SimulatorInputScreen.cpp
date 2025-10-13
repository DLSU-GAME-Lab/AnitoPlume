#include "SimulatorInputScreen.hpp"
#include "singleton/PlumeManager.hpp"

SimulatorInputScreen::SimulatorInputScreen() : GUIScreen("SIMULATOR_INPUT_SCREEN")
{

}

SimulatorInputScreen::~SimulatorInputScreen()
{

}

void SimulatorInputScreen::drawGUI()
{
    //Plume& plume = PlumeManager::getInstance()->getPlumes()[0];
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

    //show_display_settings();
    //show_wind_settings();
    //show_eruption_parameters();
    //// Coeffs

    ////float air_inc_min = 0.5, air_inc_max = 10.;
    ////ImGui::SliderScalar("Air incorporation coefficient", ImGuiDataType_Float, &air_incorporation_coeff, &air_inc_min, &air_inc_max, "%.2f");

    //ImGui::PopStyleColor();
    //ImGui::PopStyleVar();
    //ImGui::End();
}

void SimulatorInputScreen::show_display_settings()
{
    const float indent_width = 5;
    const float child_width = 380;

    if (ImGui::CollapsingHeader("Display Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Display", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 5.25f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);

        ImGui::Checkbox("Display billboards", &display_billboards);
        ImGui::Checkbox("Display torus layers", &display_smoke_layers);
        ImGui::Checkbox("Display free spheres", &display_free_spheres);
        //ImGui::Checkbox("Display subspheres", &display_subspheres);
        ImGui::Checkbox("Display spheres with subspheres", &display_spheres_with_subspheres);
        ImGui::Checkbox("Display Tooltips", &display_tooltips);
        ImGui::Unindent();
        ImGui::EndChild();
    }

}

void SimulatorInputScreen::show_wind_settings()
{
    const float indent_width = 5;
    const float child_width = 380;

    // Wind presets
    if (ImGui::CollapsingHeader("Wind Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Wind", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 13.5f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);
        ImGui::PushItemWidth(200);

        const int wind_size = 6;
        bool altitude_selected = false;

        int alt_min = 0, alt_max = max_altitude;
        int wind_min = 0, wind_max = 200;
        int angle_min = 0, angle_max = 360;

        const float indent_w = 29;
        const float slider_width = 25;
        const float plot_width = 330;
        const float plot_height = 100;

        float intensity[wind_size] = {};
        float angle[wind_size] = {};

        for (int i = 0; i < wind_size; i++)
        {
            intensity[i] = PlumeManager::getInstance()->getWinds()[i].intensity;
            angle[i] = PlumeManager::getInstance()->getDegAngle()[i];
        }

        const int x_offset = ImGui::GetCursorScreenPos().x;
        const int plot_start = ImGui::GetCursorScreenPos().y;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const float plot_grid_offset = x_offset + slider_width + 11;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 1; j < wind_size - 1; j++)
            {
                float plot_division = (float)j / (wind_size - 1);
                float plot_grid = (plot_division * (plot_width - 8)) + plot_grid_offset;

                float total_height = plot_start + plot_height * i + (i * 3);
                ImVec2 start = ImVec2(plot_grid, total_height);
                ImVec2 end;

                if (i == 2) end = ImVec2(plot_grid, total_height + 22);
                else end = ImVec2(plot_grid, total_height + plot_height);

                draw_list->AddLine(start, end, IM_COL32(255, 255, 255, 100), 1.0f);
            }
        }

        static int wind_intensity = PlumeManager::getInstance()->getWinds()[selected].intensity;
        if (ImGui::VSliderScalar("##Intensity Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &wind_intensity, &wind_min, &wind_max))
        {
            PlumeManager::getInstance()->setWindIntensity(selected, wind_intensity);
            //direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PlotLines("##Wind Intensity", intensity, wind_size, 0, "Wind Intensity (m/s)", wind_min, wind_max, ImVec2(plot_width, plot_height));
        ImGui::PopStyleColor();

        static int wind_angle = PlumeManager::getInstance()->getDegAngle()[selected];
        if (ImGui::VSliderScalar("##Angle Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &wind_angle, &angle_min, &angle_max))
        {
            if (all_angles)
            {
                PlumeManager::getInstance()->setAllWindAngles(wind_angle);
            }
            else
            {
                PlumeManager::getInstance()->setWindAngle(selected, wind_angle);
            }
            //direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.1f, 0.8f, 0.1f, 1.0f));
        ImGui::PlotLines("##Wind Angle", angle, wind_size, 0, "Wind Angle (degrees)", angle_min, angle_max, ImVec2(plot_width, plot_height));
        ImGui::PopStyleColor();

        float plot_div = (float)selected / (wind_size - 1);
        float plot_x = (plot_div * (plot_width - 8)) + plot_grid_offset;

        ImVec2 start = ImVec2(plot_x, plot_start);
        ImVec2 end = ImVec2(plot_x, plot_start + (plot_height * 2) + 3);
        draw_list->AddLine(start, end, IM_COL32(240, 220, 40, 255), 3.0f);

        ImGui::Indent(indent_w);
        ImGui::PushItemWidth(plot_width + 7);
        if (ImGui::SliderScalar("##Altitude", ImGuiDataType_S32, &wind_alt, &alt_min, &alt_max, "%d meters in altitude"))
        {
            for (int i = 0; i < wind_size && !altitude_selected; i++)
            {
                if (PlumeManager::getInstance()->getWindAlts()[i] == wind_alt)
                {
                    altitude_selected = true;
                    selected = i;
                }
            }

            if (!altitude_selected)
            {
                selected = vcl::clamp(((float)wind_alt / altitude_step) + 0.5f, 0, wind_size - 1);
                wind_alt = PlumeManager::getInstance()->getWindAlts()[selected];
            }
        }
        ImGui::PopItemWidth();
        ImGui::Unindent(indent_w);
        ImGui::Spacing();

        if (ImGui::Button("No wind"))
        {
            is_wind = false;
            PlumeManager::getInstance()->setAllWinds(wind_min, angle_min);
            //direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        if (ImGui::Button("Linear Wind"))
        {
            is_wind = true;
            PlumeManager::getInstance()->setLinearWind(linear_wind_base);
            //direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        if (ImGui::Button("Max Intensity"))
        {
            is_wind = true;
            PlumeManager::getInstance()->setAllWindIntensities(wind_max);
            //direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        ImGui::Checkbox("All Angles", &all_angles);

        // Wind
        float lin_windbase_min = 0., lin_windbase_max = 35.;
        if (ImGui::SliderScalar("Linear wind speed", ImGuiDataType_Float, &linear_wind_base, &lin_windbase_min, &lin_windbase_max, "%1.f m/s"))
        {
            if (is_wind)
            {
                PlumeManager::getInstance()->setLinearWind(linear_wind_base);
                //direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
            }
        }
        ImGui::PopItemWidth();

        if (ImGui::Button("Set to 2020 Eruption Params"))
        {
            //U_0 = 200;
            //rho_0 = 250;
            std::vector<int> intensities;
            std::vector<int> angles;

            intensities.push_back(1);
            intensities.push_back(14);
            intensities.push_back(20);
            intensities.push_back(30);
            intensities.push_back(40);
            intensities.push_back(58);

            angles.push_back(0);
            angles.push_back(30);
            angles.push_back(330);
            angles.push_back(90);
            angles.push_back(120);
            angles.push_back(135);

            for (unsigned int i = 0; i < angles.size(); i++)
            {
                PlumeManager::getInstance()->setWind(i, intensities[i], angles[i]);
            }
            //direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::Unindent();
        ImGui::EndChild();
    }
}

void SimulatorInputScreen::show_eruption_parameters()
{
    Plume& plume = PlumeManager::getInstance()->getPlumes()[0];
    const float indent_width = 5;
    const float child_width = 380;



    // Initial conditions
    if (ImGui::CollapsingHeader("Eruption Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Parameters", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 4.25f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);
        ImGui::PushItemWidth(200);

        double initial_speed_min = 0., initial_speed_max = 200.;
        ImGui::SliderScalar("Initial plume speed", ImGuiDataType_Double, &fU0, &initial_speed_min, &initial_speed_max, "%.2f m/s");
        plume.setU0(fU0);
        double initial_density_min = 150., initial_density_max = 250.;
        ImGui::SliderScalar("Initial plume density", ImGuiDataType_Double, &fRho0, &initial_density_min, &initial_density_max, "%.2f kg/m3");
        plume.setRho0(fRho0);
        double vent_ray_min = 50., vent_ray_max = 200.;
        ImGui::SliderScalar("Vent radius", ImGuiDataType_Double, &fR0, &vent_ray_min, &vent_ray_max, "%.2f m");
        plume.setR0(fR0);

        double vent_altitude_min = 0., vent_altitude_max = 8000.;
        ImGui::SliderScalar("Vent altitude", ImGuiDataType_Double, &fZ0, &vent_altitude_min, &vent_altitude_max, "%.2f m");
        plume.setZ0(fZ0);

        ImGui::PopItemWidth();
        ImGui::Unindent();
        ImGui::EndChild();

    }

}

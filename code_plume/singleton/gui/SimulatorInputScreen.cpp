#include "SimulatorInputScreen.hpp"
#include "singleton/PlumeManager.hpp"
#include "singleton/PlumeTracker.hpp"

SimulatorInputScreen::SimulatorInputScreen() : GUIScreen("Simulator Input")
{
    is_wind = false;
    all_angles = false;
    linear_wind_base = 15.;
    max_altitude = 10000;
    altitude_step = 2000;
    timer_scale = 1.0f;

    selected = 0;
    wind_alt = 0;
    plume_index = 0;

    display_billboards = true;
    display_smoke_layers = false;
    display_free_spheres = false;
    display_subspheres = false;
    display_spheres_with_subspheres = false;
    display_tooltips = true;
    display_landmarks = true;

    erupt_on_play.push_back(true);
    for (int i = 1; i < 5; i++)
        erupt_on_play.push_back(false);
}

SimulatorInputScreen::~SimulatorInputScreen()
{

}

void SimulatorInputScreen::drawGUI()
{
    ImGui::Begin("Simulator Input", &enabled, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 5);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 0.1f));
    ImGui::PushItemWidth(200);

    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 10.0f;
    if (ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer_scale, &scale_min, &scale_max, "%.2f s"))
        PlumeManager::getInstance()->setTimerScale(timer_scale);

    // Parameters
    std::vector<Plume>& plume = PlumeManager::getInstance()->getPlumes();
    unsigned int spheres_min = 0, spheres_max = 500;
    static unsigned int subspheres_number = 0, subsubspheres_number = 0;
    if (ImGui::SliderScalar("Number of subspheres", ImGuiDataType_S32, &subspheres_number, &spheres_min, &spheres_max))
        for (int i = 0; i < plume.size(); i++) plume[i].subspheres_number = subspheres_number;

    if (ImGui::SliderScalar("Number of subsubspheres", ImGuiDataType_S32, &subsubspheres_number, &spheres_min, &spheres_max))
        for (int i = 0; i < plume.size(); i++) plume[i].subsubspheres_number = subsubspheres_number;

    ImGui::PopItemWidth();

    showDisplaySettings();
    showWindSettings();
    showEruptionParameters();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::End();
}

void SimulatorInputScreen::showDisplaySettings()
{
    if (ImGui::CollapsingHeader("Display Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Display", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 6.25f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);

        ImGui::Checkbox("Display billboards", &display_billboards);
        ImGui::Checkbox("Display torus layers", &display_smoke_layers);
        ImGui::Checkbox("Display free spheres", &display_free_spheres);
        ImGui::Checkbox("Display spheres with subspheres", &display_spheres_with_subspheres);
        ImGui::Checkbox("Display Tooltips", &display_tooltips);
        ImGui::Checkbox("Display Landmarks", &display_landmarks);
        ImGui::Unindent();
        ImGui::EndChild();
    }
}

void SimulatorInputScreen::showWindSettings()
{
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

        int wind_intensity = PlumeManager::getInstance()->getWinds()[selected].intensity;
        if (ImGui::VSliderScalar("##Intensity Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &wind_intensity, &wind_min, &wind_max))
        {
            PlumeManager::getInstance()->setWindIntensity(selected, wind_intensity);
            PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PlotLines("##Wind Intensity", intensity, wind_size, 0, "Wind Intensity (m/s)", wind_min, wind_max, ImVec2(plot_width, plot_height));
        ImGui::PopStyleColor();

        int wind_angle = PlumeManager::getInstance()->getDegAngle()[selected];
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
            PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
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
        int wind_alt = PlumeManager::getInstance()->getWindAlts()[selected];
        if (ImGui::SliderScalar("##Altitude", ImGuiDataType_S32, &wind_alt, &alt_min, &alt_max, "%d meters in altitude"))
        {
            selected = vcl::clamp(((float)wind_alt / altitude_step) + 0.5f, 0, wind_size - 1);
            wind_alt = PlumeManager::getInstance()->getWindAlts()[selected];
        }
        ImGui::PopItemWidth();
        ImGui::Unindent(indent_w);
        ImGui::Spacing();

        if (ImGui::Button("No wind"))
        {
            is_wind = false;
            PlumeManager::getInstance()->setAllWinds(wind_min, angle_min);
            PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        if (ImGui::Button("Linear Wind"))
        {
            is_wind = true;
            PlumeManager::getInstance()->setLinearWind(linear_wind_base);
            PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        if (ImGui::Button("Max Intensity"))
        {
            is_wind = true;
            PlumeManager::getInstance()->setAllWindIntensities(wind_max);
            PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
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
                PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
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
            PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::Unindent();
        ImGui::EndChild();
    }
}

void SimulatorInputScreen::showEruptionParameters()
{
    if (ImGui::CollapsingHeader("Eruption Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Parameters", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 7.5f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);
        ImGui::PushItemWidth(200);

        const char* vent_names[]{
            "Taal Main Crater",
            "Pira-piraso",
            "Binintiang Munti",
            "Binintiang Malaki",
            //"Calauit Point"
        };

        Plume& plume = PlumeManager::getInstance()->getPlumes()[plume_index];
        double r_0 = plume.get_r_0();
        double U_0 = plume.get_U_0();
        double rho_0 = plume.get_rho_0();
        double z_0 = plume.get_z_0();
        
        ImGui::Combo("Eruption Vent", &plume_index, vent_names, ARRAYSIZE(vent_names));

        if (ImGui::Button("Enable all"))
        {
            PlumeManager::getInstance()->setToUpdate(true);
        }
        ImGui::SameLine();
        if (ImGui::Button("Main crater only") &&
            PlumeManager::getInstance()->getState() == SimulatorState::Stopped)
        {
            PlumeManager::getInstance()->setToUpdate(false);
            PlumeManager::getInstance()->setToUpdate(0, true);
        }

        ImGui::Separator();

        bool erupt = erupt_on_play[plume_index];
        if (ImGui::Checkbox("Erupt on play", &erupt))
        {
            // if playing or stopped and erupt == true: set toUpdate to erupt
            // if stopped and erupt == false: set toUpdate to erupt
            erupt_on_play[plume_index] = erupt;
            if (erupt)
            {
                PlumeManager::getInstance()->setToUpdate(plume_index, true);
            }
            else
            {

            }
        }

        double initial_speed_min = 0., initial_speed_max = 200.;
        if (ImGui::SliderScalar("Initial plume speed", ImGuiDataType_Double, &U_0, &initial_speed_min, &initial_speed_max, "%.2f m/s"))
            plume.set_U_0(U_0);

        double initial_density_min = 150., initial_density_max = 250.;
        if (ImGui::SliderScalar("Initial plume density", ImGuiDataType_Double, &rho_0, &initial_density_min, &initial_density_max, "%.2f kg/m3"))
            plume.set_rho_0(rho_0);

        double vent_ray_min = plume.getMinRadius(), vent_radius_max = plume.getMaxRadius();
        if (ImGui::SliderScalar("Vent radius", ImGuiDataType_Double, &r_0, &vent_ray_min, &vent_radius_max, "%.2f m"))
            plume.set_r_0(r_0);

        double vent_altitude_min = 0., vent_altitude_max = 8000.;
        if (ImGui::SliderScalar("Vent altitude", ImGuiDataType_Double, &z_0, &vent_altitude_min, &vent_altitude_max, "%.2f m"))
            plume.set_z_0(z_0);

        ImGui::PopItemWidth();
        ImGui::Unindent();
        ImGui::EndChild();
    }
}

void SimulatorInputScreen::resetEruptOnPlay()
{
    for (int i = 0; i < erupt_on_play.size(); i++)
    {
        if (erupt_on_play[i]) PlumeManager::getInstance()->setToUpdate(i, true);
    }
}

float SimulatorInputScreen::getTimerScale() const
{
    return this->timer_scale;
}

void SimulatorInputScreen::setTimerScale(float timer_scale)
{
    this->timer_scale = timer_scale;
}

bool SimulatorInputScreen::getDisplaySmokeLayers() const
{
    return this->display_smoke_layers;
}

void SimulatorInputScreen::setDisplaySmokeLayers(bool display)
{
    this->display_smoke_layers = display;
}

bool SimulatorInputScreen::getDisplayFreeSpheres() const
{
    return this->display_free_spheres;
}

void SimulatorInputScreen::setDisplayFreeSpheres(bool display)
{
    this->display_free_spheres = display;
}

bool SimulatorInputScreen::getDisplaySubspheres() const
{
    return this->display_subspheres;
}

void SimulatorInputScreen::setDisplaySubspheres(bool display)
{
    this->display_subspheres = display;
}

bool SimulatorInputScreen::getDisplaySpheresWithSubspheres() const
{
    return this->display_spheres_with_subspheres;
}

void SimulatorInputScreen::setDisplaySpheresWithSubspheres(bool display)
{
    this->display_spheres_with_subspheres = display;
}

bool SimulatorInputScreen::getDisplayBillboards() const
{
    return this->display_billboards;
}

void SimulatorInputScreen::setDisplayBillboards(bool display)
{
    this->display_billboards = display;
}

bool SimulatorInputScreen::getDisplayTooltips() const
{
    return this->display_tooltips;
}

void SimulatorInputScreen::setDisplayTooltips(bool display)
{
    this->display_tooltips;
}

bool SimulatorInputScreen::getDisplayLandmarks() const
{
    return this->display_landmarks;
}

void SimulatorInputScreen::setDisplayLandmarks(bool display)
{
    this->display_landmarks = display;
}

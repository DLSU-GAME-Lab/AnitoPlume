#include "PlumeManager.hpp"
#include "singleton/EngineTime.hpp"
#include "singleton/GUIManager.hpp"
#include "singleton/CameraManager.hpp"
#include "singleton/ShaderManager.hpp"


PlumeManager* PlumeManager::sharedInstance = NULL;

PlumeManager::PlumeManager()
{
    this->dSubspheresNumber = 0;
    this->dSubSubspheresNumber = 0;
    float max_altitude = 10000;
    float altitude_step = 2000;
    unsigned int altitude_size = int(max_altitude / altitude_step) + 1;
    for (unsigned int i = 0; i < altitude_size; i++)
    {
        this->vecWindAlts.push_back(i * altitude_step);
        this->vecHoriWinds.push_back(wind_structure(0, 0));
        this->vecWindAngles.push_back(0);
    }  
}

PlumeManager::~PlumeManager()
{
}
void PlumeManager::initialize()
{
	sharedInstance = new PlumeManager();

}
PlumeManager* PlumeManager::getInstance()
{
	return sharedInstance;
}
void PlumeManager::destroy()
{
    delete sharedInstance;
}
void PlumeManager::setConstantTimeStep(float fDelta)
{
    // Force constant time step
    this->fConstantTimeStep = fDelta <= 1e-6f ? 0.0f : EngineTime::getInstance()->getTimeScale() *0.002f; //0.0003f
}
void PlumeManager::torusSetup(scene_structure scene)
{
    mesh cyl = vcl::mesh_primitive_cylinder(2.5f, { 0,0,1.5 }, { 0,0,-1.5 }, 30, 30);
    mesh d1 = vcl::mesh_primitive_disc(2.5f, { 0,0,1.5 });
    mesh d2 = vcl::mesh_primitive_disc(2.5f, { 0,0,-1.5 });
    mesh t = cyl;
    t.push_back(d1); t.push_back(d2);
    torusMesh = t;
    torusMesh.uniform.color = { 1,0.5,0 };
    torusMesh.shader = ShaderManager::getInstance()->getShader("mesh");
    torusMesh.texture_id = scene.texture_white;
    torusMesh.uniform.color_alpha = 0.6f;
}
void PlumeManager::sphereSetup(scene_structure scene)
{

    sphereMesh = vcl::mesh_primitive_sphere();
    sphereMesh.texture_id = scene.texture_white;
}
void PlumeManager::removeSmokeLayers()
{
    if (!this->vecSmokeLayers.empty())
    {
        //std::cout << "lifetime: " << smoke_layers[0].lifetime << " max: " << max_lifetime << " t_step: " << t_step << "\n";
        while (this->vecSmokeLayers[0].lifetime > fMaxLifetime)
        {
            this->vecSmokeLayers.erase(this->vecSmokeLayers.begin());

            for (int i = 0; i < this->vecFreeSpheres.size(); i++)
            {
                this->vecFreeSpheres[i].closest_layer_idx--;
            }

            while (this->vecFreeSpheres[0].closest_layer_idx < 0)
            {
                this->vecFreeSpheres.erase(this->vecFreeSpheres.begin());
            }
        }
    }
}
void PlumeManager::smokeLayerUpdate(int dId, float fDelta)
{
    float fMass = 0; // to track mass change for equation of dynamics
    vecSmokeLayers[dId].lifetime = vecSmokeLayers[dId].lifetime + fDelta;

    if (vecSmokeLayers[dId].plume == true && vecSmokeLayers[dId].center.z > 0.) sedimentation(dId, fMass, fDelta); // sedimentation in altitude
    if (vecSmokeLayers[dId].rising && !vecSmokeLayers[dId].stagnates_long) editSmokeLayerProp(dId, fMass, fDelta); // convection if v_z > 0 (convection causes air entrainment)
    applyForcesToSmokeLayer(dId, fMass, fDelta);
    checkSmokePos(dId);
}

void PlumeManager::freeSphereUpdate(float fDelta)
{
    for (int i = vecFreeSpheres.size() - 1; i >= 0; i--)
    {
        free_sphere_params& sphere_i = vecFreeSpheres[i];
        sphere_i.lifetime = sphere_i.lifetime + fDelta;

        if (!sphere_i.stagnate_long && !sphere_i.falling)
        {
            // identify closest layer which is rising or begins falling (later : all layers in which the sphere is)
            int closest_layer_id = vecSmokeLayers.size() - 1;
            float min_dist = norm(sphere_i.center - vecSmokeLayers[closest_layer_id].center);
            for (unsigned int j = 0; j < vecSmokeLayers.size(); j++)
            {
                //float dist = abs(vecFreeSpheres[i].center.z - vecSmokeLayers[j].center.z);
                float dist = norm(sphere_i.center - vecSmokeLayers[j].center);
                if (dist < min_dist && (vecSmokeLayers[j].rising || vecSmokeLayers[j].begin_falling) && !vecSmokeLayers[j].stagnates_long)
                {
                    min_dist = dist;
                    closest_layer_id = j;
                }
            }

            if (sphere_i.secondary_column) closest_layer_id = sphere_i.closest_layer_idx;
            if (sphere_i.stagnate || sphere_i.stagnate_long) closest_layer_id = sphere_i.closest_layer_idx;
            closest_layer_id = sphere_i.closest_layer_idx;

            // check if closest layer begins falling (if so, make sphere falling)
            if (vecSmokeLayers[closest_layer_id].begin_falling)
            {
                //subdivide_and_make_falling(i);
            }
            else if (vecSmokeLayers[closest_layer_id].stagnates && !vecSmokeLayers[closest_layer_id].stagnates_long && !sphere_i.stagnate)
            {
                // check if densities have become equal: if so, keep altitude in memory
                sphere_i.stagnate = true;
                sphere_i.closest_layer_idx = closest_layer_id;
                sphere_i.stagnation_altitude = sphere_i.center.z;
            }
            else if (sphere_i.stagnate && !sphere_i.stagnate_long && vecSmokeLayers[sphere_i.closest_layer_idx].stagnates_long)
            {
                // check if closest layer has reached max altitude: if so, keep sphere position in memory for stagnation spreading function
                sphere_i.stagnate_long = true;
                sphere_i.max_altitude = sphere_i.center.z;
                sphere_i.center_at_max_altitude = sphere_i.center;
                sphere_i.layer_center_at_max_altitude = vecSmokeLayers[closest_layer_id].center;
                sphere_i.xy_at_max_altitude = sqrt(sphere_i.center.x * sphere_i.center.x + sphere_i.center.y * sphere_i.center.y);
            }
            else
            {
                // get axial and radial composants relative to layer center
                vec3 p_relative = sphere_i.center - vecSmokeLayers[closest_layer_id].center;
                vec3 p_axial = dot(p_relative, vecSmokeLayers[closest_layer_id].plume_axis) * vecSmokeLayers[closest_layer_id].plume_axis;
                vec3 p_radial = p_relative - p_axial;

                // update rotation axis with new radial vector (can change upon time because axis changes)
                sphere_i.angle_vector = p_radial / norm(p_radial);
                //sphere_i.rotation_axis = normalize(cross(vecSmokeLayers[closest_layer_id].plume_axis, p_radial));

                // update radial position if too close from plume axis
                //if (norm(p_radial) < vecSmokeLayers[closest_layer_id].r)*/ vecFreeSpheres[i].center = vecSmokeLayers[closest_layer_id].center + p_axial + vecSmokeLayers[closest_layer_id].r * normalize(p_radial);

                // update size according to layer
                // size of layer + perturbation (some spheres should grow much more than others, to create diversity)
                float new_r = sphere_i.size_factor * vecSmokeLayers[closest_layer_id].r;

                // update radial speed and position by adding perturbation (one part is random and one depends on radial position, so that spheres do not stay in the middle of the plume and do not go away)
                float random_f = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                sphere_i.perturbation += new_r * 0.001 * 2.0 * (random_f - 0.5);
                if (sphere_i.perturbation > 100.) sphere_i.perturbation = 100.;
                if (sphere_i.perturbation < -100.) sphere_i.perturbation = -100.;
                float new_speed = computeGaussianSpeedInLayer(vecSmokeLayers[closest_layer_id].speed_along_axis, 2. * vecSmokeLayers[closest_layer_id].r, norm(p_relative)); // axial speed
                if (vecSmokeLayers[closest_layer_id].stagnates) new_speed = vecSmokeLayers[closest_layer_id].speed_along_axis;
                float radial_speed = (vecSmokeLayers[closest_layer_id].r - norm(p_radial)) / 0.5;
                if (vecSmokeLayers[closest_layer_id].stagnates) radial_speed = 0;
                sphere_i.perturbation += radial_speed;
                //if (vecSmokeLayers[closest_layer_id].stagnates) std::cout << sphere_i.perturbation << std::endl;
                if (vecSmokeLayers[closest_layer_id].stagnates && sphere_i.perturbation < 0) sphere_i.perturbation = 0;
                radial_speed = 0;
                //std::cout << norm(p_relative) - vecSmokeLayers[closest_layer_id].r << " " << vecFreeSpheres[i].perturbation << " " << radial_speed << std::endl;

                // update rotation speed with new speed and ray
                float new_angular_speed = new_speed / new_r;

                // update
                sphere_i.speed = new_speed * vecSmokeLayers[closest_layer_id].plume_axis + (sphere_i.perturbation) * sphere_i.angle_vector;
                sphere_i.r = new_r;
                sphere_i.relative_distance = norm(sphere_i.center - vecSmokeLayers[closest_layer_id].center);
                sphere_i.rho = vecSmokeLayers[closest_layer_id].rho;

                if (!sphere_i.stagnate && vecSmokeLayers[closest_layer_id].theta > 1)
                {
                    sphere_i.angular_speed = new_angular_speed;
                    float new_angle = sphere_i.current_angle + sphere_i.angular_speed * fDelta;
                    sphere_i.current_angle = new_angle;
                }
                else sphere_i.angular_speed = 0;
            }
        }
    }

    // update positions
    for (unsigned int i = 0; i < vecFreeSpheres.size(); i++)
    {
        if (!vecFreeSpheres[i].stagnate_long && !vecFreeSpheres[i].falling)
        {
            vecFreeSpheres[i].center += fDelta * vecFreeSpheres[i].speed;
        }
        else if (vecFreeSpheres[i].falling)
        {
            vecFreeSpheres[i].center.z = vecSmokeLayers[vecFreeSpheres[i].closest_layer_idx].center.z;
        }
        //update_spheres_on_free_sphere(i);
    }

    // make begin_falling layers falling
    for (int i = vecSmokeLayers.size() - 1; i >= 0; i--)
    {
        if (vecSmokeLayers[i].begin_falling && vecSmokeLayers.size() >= 1)
        {
            vecSmokeLayers[i].falling = true;
            vecSmokeLayers[i].begin_falling = false;
            vecSmokeLayers[i].rising = false;
            vecSmokeLayers[i].plume = false;
            //vecSmokeLayers[i].center.z = -2000;
        }
    }
}

void PlumeManager::addSmokeLayer(float fVel, float fDense, float fRad, vec3 vecPos, bool bSecPlume)
{
    smoke_layer layer = smoke_layer({ 0,0,fVel }, fDense, fRad, vecPos, bSecPlume);
    vecSmokeLayers.push_back(layer);
}

void PlumeManager::addFreeSphereLayer(int dId)
{
    unsigned int nb_spheres = 6;
    float angle_offset = 2 * 3.14 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    //determine size differences
    buffer<float> sizes;
    for (unsigned int k = 0; k < nb_spheres; k++)
    {
        sizes.push_back(0.5f + static_cast <float>(rand()) / static_cast <float>(RAND_MAX));
    }
    float total = sizes[0] + sizes[1] + sizes[2] + sizes[3] + sizes[4] + sizes[5];
    float factor = (float)nb_spheres / total;
    for (unsigned int k = 0; k < sizes.size(); k++)
    {
        sizes[k] *= factor;
    }

    // add spheres
    for (unsigned int j = 0; j < nb_spheres; j++)
    {
        float angle = (float)j * 2.0 * 3.14 / 6.0;
        std::cout << dId << std::endl;
        addFreeSphere(dId, angle + angle_offset, sizes[j]);
    }
}

void PlumeManager::addFreeSphere(int dId, float fAngle, float fSize)
{
    free_sphere_params sphere(freeSphereID, vecSmokeLayers[dId].center, fAngle, fSize * vecSmokeLayers[dId].r, vecSmokeLayers[dId].v.z);
    freeSphereID++;
    sphere.size_factor = fSize;
    sphere.rho = vecSmokeLayers[dId].rho;
    float volume = 4.0 / 3.0 * 3.14 * sphere.r * sphere.r * sphere.r;
    sphere.mass = sphere.rho / volume;
    sphere.closest_layer_idx = dId;

    if (vecSmokeLayers[dId].secondary_plume)
    {
        sphere.secondary_column = true;
        sphere.closest_layer_idx = dId;
    }

    for (unsigned int i = 0; i < dSubspheresNumber; i++)
    {
        // random angles
        float theta = 3.14 * static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
        float phi = 2 * 3.14 * static_cast <float>(rand()) / static_cast <float>(RAND_MAX);

        subsphere_params subs = subsphere_params();
        subs.parent_id = vecFreeSpheres.size();
        subs.relative_position = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        subs.center = sphere.center + sphere.r * subs.relative_position;
        //subs.size_ratio = 0.10 + 0.25 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        subs.size_ratio = 0.2;
        subs.r = sphere.r * subs.size_ratio;

        for (unsigned int j = 0; j < dSubSubspheresNumber; j++)
        {
            float theta2 = 3.14 * static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
            float phi2 = 2 * 3.14 * static_cast <float>(rand()) / static_cast <float>(RAND_MAX);

            subsphere_params subsubs = subsphere_params();
            subsubs.parent_id = vecS2Spheres.size();
            subsubs.relative_position = vec3(sin(theta2) * cos(phi2), sin(theta2) * sin(phi2), cos(theta2));
            subsubs.center = subs.center + subs.r * subsubs.relative_position;
            subsubs.r = subs.r / 5.;
            vecS3Spheres.push_back(subsubs);
        }
        vecS2Spheres.push_back(subs);
    }
    vecFreeSpheres.push_back(sphere);
}

void PlumeManager::sedimentation(int dId, float fMass, float fDelta)
{
    // constant sedimentation
    float layer_volume = 3.14 * vecSmokeLayers[dId].r * vecSmokeLayers[dId].r * vecSmokeLayers[dId].thickness;
    float diff_density = 0.00000005 * fDelta;
    if (vecSmokeLayers[dId].stagnates_long) diff_density = 0.00005 * fDelta;

    if (vecSmokeLayers[dId].rho > diff_density)
    {
        vecSmokeLayers[dId].rho -= diff_density;
        fMass -= diff_density * layer_volume;
    }
}

void PlumeManager::editSmokeLayerProp(int dId, float fMass, float fDelta)
{
    // get wind at altitude
    vec3 wind = computeWindVec(vecSmokeLayers[dId].center.z);

    // preliminary computation
    float thk = fDelta * vecSmokeLayers[dId].v.z;
    //thk = vecSmokeLayers[dId].v.z * 0.002;

    float total_smoke_thk = vecSmokeLayers[dId].thickness;
    float total_smoke_volume = total_smoke_thk * 3.14 * vecSmokeLayers[dId].r * vecSmokeLayers[dId].r;
    float total_smoke_mass = vecSmokeLayers[dId].rho * total_smoke_volume;

    // wind velocity around for air incorporation
    float k_s = 0.09, k_w = 0.9;
    float U_e = k_s * abs(norm(vecSmokeLayers[dId].v) - norm(wind) * cos(vecSmokeLayers[dId].theta)) + k_w * abs(norm(wind) * sin(vecSmokeLayers[dId].theta));
    float r_atm = 5.f * U_e;
    //r_atm = U_e * fDelta;

    // air quantity to put in the plume
    float atm_density = computeAtmDensity(vecSmokeLayers[dId].center.z);
    float atm_volume = thk * 3.14 * (2.0 * vecSmokeLayers[dId].r + r_atm) * r_atm; //air around the smoke layer
    float atm_mass = atm_density * atm_volume;

    // compute new temperature (we take all Cp equal)
    float new_temp = (total_smoke_mass * vecSmokeLayers[dId].temperature + atm_mass * computeAtmTemp(vecSmokeLayers[dId].center.z)) / (total_smoke_mass + atm_mass);
    vecSmokeLayers[dId].temperature = new_temp;

    // new volume after heating
    float atm_new_volume = vecSmokeLayers[dId].temperature * atm_volume / computeAtmTemp(vecSmokeLayers[dId].center.z); // after heating by hot smoke

    // new params
    fMass = atm_mass;
    float mass_new = total_smoke_mass + atm_mass;
    float volume_new = total_smoke_volume + atm_new_volume;
    float rho_new = mass_new / volume_new;
    float r_new = cbrt(volume_new / 3.14);
    vecSmokeLayers[dId].thickness = r_new;

    // new speed due to conservation of energy (old)
    //float energy = 0.5 * total_smoke_mass * vecSmokeLayers[dId].v.z * vecSmokeLayers[dId].v.z;
    //float new_speed = sqrt(2.0 * energy / mass_new);
    //if (rho_new < atm_density) vecSmokeLayers[dId].v = {0,0,new_speed};

    if (vecSmokeLayers[dId].rho > atm_density && rho_new < atm_density) vecSmokeLayers[dId].plume = true;
    vecSmokeLayers[dId].r = r_new;
    vecSmokeLayers[dId].rho = rho_new;
}

void PlumeManager::applyForcesToSmokeLayer(int dId, float fMass, float fDelta)
{
    // get wind at altitude
    vec3 wind = computeWindVec(vecSmokeLayers[dId].center.z);

    // precomputation
    float atm_density = computeAtmDensity(vecSmokeLayers[dId].center.z);
    float volume = vecSmokeLayers[dId].thickness * 3.14 * vecSmokeLayers[dId].r * vecSmokeLayers[dId].r;
    float surface = 2 * 3.14 * vecSmokeLayers[dId].r * vecSmokeLayers[dId].r;
    float surface_eff = 2 * vecSmokeLayers[dId].r * vecSmokeLayers[dId].r;
    float smoke_mass = vecSmokeLayers[dId].rho * volume;
    vec3 v_diff = wind - vec3(vecSmokeLayers[dId].v.x, vecSmokeLayers[dId].v.y, 0);

    vec3 weight = { 0,0, -smoke_mass * fGrav }; // m*g
    vec3 archimede = { 0,0, atm_density * volume * fGrav }; // rho*V*g
    vec3 friction = -0.5 * atm_density * 0.04 * surface * norm(vecSmokeLayers[dId].v) * vecSmokeLayers[dId].v; // axial friction
    //friction = - volume * 0.00005 * norm(vecSmokeLayers[dId].v) * vecSmokeLayers[dId].v; // old way to compute friction
    vec3 wind_force = 600. * norm(v_diff) * v_diff * surface_eff; // horizontal

    vec3 forces = weight + archimede + friction + wind_force;

    vec3 old_v = vecSmokeLayers[dId].v;
    float old_m = smoke_mass - fMass;

    // equation of dynamics without mass conservation
    vec3 mv = old_m * old_v + forces * fDelta;
    vec3 v = mv / smoke_mass;
    vec3 p = vecSmokeLayers[dId].center + v * fDelta;

    // old with mass conservation
    //vec3 a = forces / (smoke_mass);
    //vec3 v = vecSmokeLayers[dId].v + a*fDelta;
    //vec3 p = vecSmokeLayers[dId].center + v*fDelta;


    // check if falls
    if (old_v.z > 0 && v.z < 0 && vecSmokeLayers[dId].plume == false)
    {
        vecSmokeLayers[dId].rising = false;
        vecSmokeLayers[dId].begin_falling = true;
        //std::cout << "layer " << i << " falling frame " << frame_count << std::endl;
    }

    // check if stagnates
    if (vecSmokeLayers[dId].plume && !vecSmokeLayers[dId].stagnates && vecSmokeLayers[dId].center.z > 5000 && (atm_density - vecSmokeLayers[dId].rho < 0.01))
    {
        vecSmokeLayers[dId].stagnates = true;
        stagnationSpeed = vecSmokeLayers[dId].v.z;
    }

    // check if stagnates long
    if (vecSmokeLayers[dId].stagnates && v.z < 0 && !vecSmokeLayers[dId].stagnates_long)
    {
        vecSmokeLayers[dId].stagnates_long = true;
    }

    if (vecSmokeLayers[dId].stagnates && p.z < vecSmokeLayers[dId].center.z)
    {
        p.z = vecSmokeLayers[dId].center.z;
    }

    // update
    vecSmokeLayers[dId].a = v / fDelta;
    vecSmokeLayers[dId].v = v;
    vecSmokeLayers[dId].center = p;
    if (p.z < -1000) vecSmokeLayers[dId].center.z = -1000; // prevent from going oob after falling (layers are not deleted but not used anymore)

    // compute new theta
    // WARNING: wind direction was constant in my tests, if the direction changes it may not work, it has to be tested
    //float dx = v.x*fDelta, dy = v.y*fDelta;
    float dz = v.z * fDelta;
    vecSmokeLayers[dId].speed_along_axis = norm(v);
    vecSmokeLayers[dId].plume_axis = normalize(v);
    if (norm(wind) != 0 && fDelta > 0)
    {
        vecSmokeLayers[dId].theta_axis = normalize(cross(wind, vec3(0, 0, 1)));
        float theta_totest = asin(dz / norm(v * fDelta));
        if (dz / norm(v * fDelta) < 1.000001 && dz / norm(v * fDelta) > 0.999999) theta_totest = 3.14 / 2.; // security to prevent nan values due to asin
        vecSmokeLayers[dId].theta = theta_totest;

        if (theta_totest < 0)
        {
            vecSmokeLayers[dId].theta = 0;
        }
    }


    //// print values in txt files
    //nb_of_iterations++;
    //if (nb_of_iterations % 10 == 1)
    //{
    //    if (altitude) altitude << vecSmokeLayers[dId].center.z << std::endl;
    //    if (plumex) plumex << vecSmokeLayers[dId].center.x << std::endl;
    //    if (speed) speed << vecSmokeLayers[dId].v.z << std::endl;
    //    if (ray) ray << vecSmokeLayers[dId].r << std::endl;
    //    if (smoke_rho) smoke_rho << vecSmokeLayers[dId].rho << std::endl;
    //    if (atm_rho) atm_rho << atm_density << std::endl;
    //    if (temp) temp << vecSmokeLayers[dId].temperature << std::endl;
    //}
}

void PlumeManager::checkSmokePos(int dId)
{
    float max_layer_altitude = vecSmokeLayers[0].center.z;
    fDirectionTrackerStep = fDirectionTrackerStep < 100.0f ? 100.0f : max_layer_altitude / dDirectionTrackerStepSize;
    dirTracker.set_altitude_step(fDirectionTrackerStep);
    for (int j = 0; j < dDirectionTrackerStepSize; j++)
    {
        if (int(vecSmokeLayers[dId].center.z) == int(j * fDirectionTrackerStep) + 1)
        {
            dirTracker.set_plume_positions(j, vecSmokeLayers[dId].center, vecSmokeLayers[dId].r);
        }
    }
}

vec3 PlumeManager::computeWindVec(float fHeight)
{
    // find altitude interval
    unsigned int low_altitude_idx = 0;
    for (unsigned int i = 0; i < vecWindAlts.size(); i++)
    {
        if (vecWindAlts[i] < fHeight)
        {
            low_altitude_idx = i;
        }
    }

    // compute wind vec by interpolating
    if (low_altitude_idx == vecWindAlts.size() - 1)
    {
        return vecHoriWinds[low_altitude_idx].wind_vector;
    }
    else
    {
        float low_height = (float)vecWindAlts[low_altitude_idx];
        float high_height = (float)vecWindAlts[low_altitude_idx + 1];
        float lambda = (fHeight - low_height) / (high_height - low_height);
        vec3 interpo_wind = vecHoriWinds[low_altitude_idx].wind_vector + lambda * (vecHoriWinds[low_altitude_idx + 1].wind_vector - vecHoriWinds[low_altitude_idx].wind_vector);
        return interpo_wind;
    }
}

float PlumeManager::computeAtmDensity(float fHeight)
{
    return 352.995 * pow(1 - 0.0000225577 * fHeight, 5.25516) / (288.15 - 0.0065 * fHeight);
}

float PlumeManager::computeAtmTemp(float fHeight)
{
    return 288.15 - 6.5 * fHeight / 1000.0;
}

float PlumeManager::computeGaussianSpeedInLayer(float fVelZ, float maxR, float fR)
{
    float fA = maxR / (2. * sqrt(log(2)));
    return 2. * fVelZ * exp(-fR * fR / (fA * fA));
}

void PlumeManager::update(float fDelta)
{
    this->setConstantTimeStep(fDelta);
    this->fLayerDelay += fConstantTimeStep;
    this->removeSmokeLayers();
    for (unsigned int nb_steps_per_frame = 0; nb_steps_per_frame < 10; nb_steps_per_frame++)
    {
        if (this->vecSmokeLayers.size() == 0 || (this->fLayerDelay >= this->fInitialRadius / (2 * this->fInitialSpeed) && this->vecSmokeLayers.size() < 1000000000000000))
        {
            this->addSmokeLayer(this->fInitialSpeed, this->fInitialDensity, this->fInitialRadius, vec3(2500, 0, this->fInitialAltitude), false);
            this->addFreeSphereLayer(PlumeManager::getInstance()->getSmokeLayer().size() - 1);
            this->fLayerDelay = 0;
            this->dTotalLayerEjected++;
            std::cout << this->getSmokeLayer().size() << std::endl;
            std::cout << "LAYER ADDED OK" << std::endl;
        }
        for (unsigned int id = 0; id < PlumeManager::getInstance()->getSmokeLayer().size(); id++)
        {
            PlumeManager::getInstance()->smokeLayerUpdate(id, fConstantTimeStep);
        }
        PlumeManager::getInstance()->freeSphereUpdate(fConstantTimeStep);
        //if (frame_count % 100 == 0) falling_spheres_update(100);
        //update_stagnation_spheres();
        this->dFrameCount++;

    }
}

void PlumeManager::drawTorus(bool bDisplay, camera_scene* camera)
{
    for (unsigned int i = 0; i <this->vecSmokeLayers.size(); i++)
    {
        smoke_layer lay = PlumeManager::getInstance()->getSmokeLayer()[i];

        torusMesh.uniform.transform.scaling = lay.r / fRatio;
        torusMesh.uniform.transform.translation = vec3(lay.center.x / fRatio - 25, lay.center.y / fRatio, lay.center.z / fRatio - 2);
        torusMesh.uniform.transform.rotation = rotation_from_axis_angle_mat3(lay.theta_axis, lay.theta - 3.14 / 2.0);
        if(bDisplay) torusMesh.draw(*camera);

    }
}




void PlumeManager::resetValues()
{
    unsigned int dSubspheresNumber = 0;
    unsigned int dSubSubspheresNumber = 0;
    unsigned int dTotalLayerEjected = 0;
    unsigned int dFrameCount = 0;
}

std::vector<smoke_layer> PlumeManager::getSmokeLayer()
{
    return this->vecSmokeLayers;
}

std::vector<free_sphere_params> PlumeManager::getFreeSpheres()
{
    return this->vecFreeSpheres;
}



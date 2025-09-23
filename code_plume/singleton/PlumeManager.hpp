#pragma once
#include <cmath>
#include "vector"
#include "../scenes/sources/smoke/smokeLayer.hpp"
#include "../scenes/sources/smoke/direction_tracker/direction_tracker.hpp"
using namespace vcl;
class PlumeManager
{
private:
	vcl::mesh_drawable torusMesh;
	vcl::mesh_drawable sphereMesh;

	std::vector<int> vecWindAlts;
	std::vector<int> vecWindAngles;
	std::vector<wind_structure> vecHoriWinds;
	std::vector<smoke_layer> vecSmokeLayers;
	std::vector<free_sphere_params> vecFreeSpheres;
	std::vector<subsphere_params>vecS2Spheres;
	std::vector<subsphere_params>vecS3Spheres;
	direction_tracker dirTracker;
	unsigned int dSubspheresNumber = 0;
	unsigned int dSubSubspheresNumber = 0;
	unsigned int dTotalLayerEjected = 0;
	unsigned int dFrameCount = 0;
	int dDirectionTrackerStepSize = 20;
	float fDirectionTrackerStep = 1000.f;
	unsigned short freeSphereID;
	float fGrav = 9.81;
	double stagnationSpeed = 50.;
	float fMinLifeTime = 180.0;
	float fMaxLifetime = 240.0;
	float fLayerDelay = 0;
	float fConstantTimeStep = 0.f;
	float fRatio = 100;

	//inital values
	double fInitialSpeed = 150.f;
	double fInitialDensity = 200.f;
	double fInitialRadius = 100.f;
	double fInitialAltitude =0.f;

private:
	void setConstantTimeStep(float fDelta);
public:
	//mesh Init functions
	void torusSetup(scene_structure scene);
	void sphereSetup(scene_structure scene);
public:
	void removeSmokeLayers();
	void smokeLayerUpdate(int dId, float fDelta);
	void freeSphereUpdate(float fDelta);
	void addSmokeLayer(float fVel, float fDense, float fRad, vec3 vecPos, bool bSecPlume);
	void addFreeSphereLayer(int dId);
	void addFreeSphere(int dId, float fAngle, float fSize);
	void sedimentation(int dID, float fMass, float fDelta);
	void editSmokeLayerProp(int dId, float fMass, float fDelta);
	void applyForcesToSmokeLayer(int dId, float fMass, float fDelta);
	void checkSmokePos(int dId);
	vec3 computeWindVec(float fHeight);
	float computeAtmDensity(float fHeight);
	float computeAtmTemp(float fHeight);
	float computeGaussianSpeedInLayer(float fVelZ, float maxR, float r);
	void update(float fDelta);
	void drawTorus(bool bDisplay, camera_scene* camera);
	void resetValues();
public:
	std::vector<smoke_layer> getSmokeLayer();
	std::vector<free_sphere_params> getFreeSpheres();
public:
	static PlumeManager* getInstance();
	static void initialize();
	static void destroy();


private:
	PlumeManager();
	~PlumeManager();
	PlumeManager(const PlumeManager&) {};
	PlumeManager operator=(const PlumeManager&) {};
	static PlumeManager* sharedInstance;
};

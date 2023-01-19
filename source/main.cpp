#include "framework/engine.h"
#include "framework/utils.h"

#include "solution/spline.h"
#include "solution/rails_drawer.h"
#include "solution/utility.h"
#include "solution/train.h"

using namespace std;
using namespace glm;

//-----------------------------------------------------------------------------
// Global settings
//-----------------------------------------------------------------------------

#define SETTINGS_SPLINE_EPS		    0.01f
#define SETTINGS_APPROX_EPS		    0.001f
#define SETTINGS_TIES_COUNT			std::pow(2, 7)
#define SETTINGS_TIES_WIDTH			1.0f
#define SETTINGS_RAILS_WIDTH		1.3f
#define SETTINGS_RAILS_TRACK_WIDTH  0.2f
#define SETTINGS_TRAIN_SPEED        0.02f
#define SETTINGS_CARS_COUNT         4

#define SETTINGS_IS_LOOP true

#define SETTINGS_WIREFRAME
#define SETTINGS_SHOW_DEBUG_INFO

//-----------------------------------------------------------------------------

/*
* Coordinate system:
* x - right
* y - up
* z - backward
*/

int main() {
	// initialization
	Engine * engine = Engine::get();
	engine->init(1600, 900, "UNIGINE Test Task");

	// set up camera
	Camera & cam = engine->getCamera();
	cam.Position = vec3(0.0f, 20.f, 0.0f);
	cam.Yaw = -90.0f;
	cam.Pitch = -90.0f;
	cam.UpdateCameraVectors();

	// create shared meshes
	Mesh plane_mesh = createPlane();
	Mesh sphere_mesh = createSphere();
	Mesh cube_mesh = createCube();

	// create background objects
	Object * plane = engine->createObject(&plane_mesh);
	plane->setColor(0.2f, 0.37f, 0.2f); // green
	plane->setPosition(0, -0.5f, 0);
	plane->setRotation(-90.0f, 0.0f, 0.0f);
	plane->setScale(20.0f);

	// path
	const float path[] = {
		0.0f, -0.375f, 7.0f, // 1
		-6.0f, -0.375f, 5.0f, // 2
		-8.0f, -0.375f, 1.0f, // 3
		-4.0f, -0.375f, -6.0f, // 4
		0.0f, -0.375f, -7.0f, // 5
		1.0f, -0.375f, -4.0f, // 6
		4.0f, -0.375f, -3.0f, // 7
		8.0f, -0.375f, 7.0f // 8
	};

#ifdef SETTINGS_SHOW_DEBUG_INFO
	vector<Object*> points;
	for (int i = 0; i < 8; i++) {
		Object * sphere = engine->createObject(&sphere_mesh);
		sphere->setColor(1, 0, 0);
		sphere->setPosition(path[i * 3], path[i * 3 + 1], path[i * 3 + 2]);
		sphere->setScale(0.25f);
		points.push_back(sphere);
	}
	LineDrawer path_drawer(path, points.size(), SETTINGS_IS_LOOP);
#endif

	//-----------------------------------------------------------------------------
	// Converting points and constructing spline
	//-----------------------------------------------------------------------------

	Spline spline([&path]() {
		std::vector<vec3> result;
		result.reserve(8);
		for (int i = 0; i < 8; i++) {
			result.emplace_back(path[i * 3], path[i * 3 + 1], path[i * 3 + 2]);
		}
		return result;
	}(), SETTINGS_SPLINE_EPS, SETTINGS_IS_LOOP);

	//-----------------------------------------------------------------------------
	// Drawing spline debug info
	//-----------------------------------------------------------------------------

#ifdef SETTINGS_SHOW_DEBUG_INFO
	auto splineDebugInfoFunc = [&engine, &sphere_mesh](const Spline & spline, const float height,
	                                                   const float scale = 0.025f,
	                                                   const vec3 & color = { 0.0f, 1.0f, 0.0f }) {
		for (const auto & segment : spline) {
			for (const auto & line : segment) {
				Object * s1 = engine->createObject(&sphere_mesh);
				s1->setColor(color);
				s1->setPosition(line.getFirst().x, height, line.getFirst().z);
				s1->setScale(scale);

				Object * s2 = engine->createObject(&sphere_mesh);
				s2->setColor(color);
				s2->setPosition(line.getSecond().x, height, line.getSecond().z);
				s2->setScale(scale * 2);
			}
		}
	};
	splineDebugInfoFunc(spline, 0.5f);
#endif

	//-----------------------------------------------------------------------------
	// Approximating spline and drawing it again if necessary
	//-----------------------------------------------------------------------------

	Spline approxSpline = Spline::approx(spline, SETTINGS_TIES_COUNT, SETTINGS_APPROX_EPS);

#ifdef SETTINGS_SHOW_DEBUG_INFO
	splineDebugInfoFunc(approxSpline, 1.0f, 0.1f, vec3(1.0f, 1.0f, 0.0f));
#endif

	//-----------------------------------------------------------------------------
	// Drawing railroad
	//-----------------------------------------------------------------------------

	generateTies(approxSpline.toVector(), SETTINGS_TIES_WIDTH);

	std::vector<vec3> splinePath = spline.toVector();
	RailsDrawer railsDrawer(splinePath, SETTINGS_IS_LOOP,
	                        SETTINGS_RAILS_TRACK_WIDTH, SETTINGS_RAILS_WIDTH);

	//-----------------------------------------------------------------------------
	// Drawing train
	//-----------------------------------------------------------------------------

	auto makeTrain = [&engine](Mesh & mesh, const vec3 & startingPos = vec3(0.0f),
	                           const vec3 color = { 0.2f, 0.0f, 0.0f }) {
		Object * train = engine->createObject(&mesh);
		train->setColor(color);
		train->setScale(0.5f, 0.5f, 1.0f);
		train->setPosition(startingPos);
		return train;
	};

	std::vector<Train> train;
	train.reserve(SETTINGS_CARS_COUNT);
	for (int i = 0; i < SETTINGS_CARS_COUNT; i++) {
		train.emplace_back(cube_mesh, splinePath.front() + vec3 { 1.4f * i, 0.0f, 0.0f },
		                   SETTINGS_TRAIN_SPEED);
	}

	//-----------------------------------------------------------------------------

#ifdef SETTINGS_WIREFRAME
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

	// main loop
	while (!engine->isDone()) {
		engine->update();
		engine->render();

		// NOTE: Speed is frame rate dependent
		//-----------------------------------------------------------------------------
		railsDrawer.draw();

		for (auto & car : train) {
			car.tutuuu(splinePath);
		}

		//-----------------------------------------------------------------------------

#ifdef SETTINGS_SHOW_DEBUG_INFO
		path_drawer.draw();
#endif

		engine->swap();
	}

	engine->shutdown();
	return 0;
}

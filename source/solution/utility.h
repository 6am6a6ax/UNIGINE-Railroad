#pragma once

#include <vector>
#include "framework/engine.h"

static void generateTies(const std::vector<glm::vec3> & points, const float width = 1.0f) {
	auto createTie = [&points, width](const glm::vec3 & position, const glm::vec3 & lookAt) {
		const glm::vec3 forward = normalize(lookAt - position);
		static Mesh planeMesh = createCube();
		Object * plane = Engine::get()->createObject(&planeMesh);
		plane->setColor(1.0f, 0.8f, 0.1f);
		plane->setScale(width, 0.0f, 0.1f);
		plane->setPosition(position);
		plane->setRotation(quatLookAt(forward, { 0.0f, 1.0f, 0.0f }));
	};

	for (std::size_t i = 0; i < points.size() - 1; i++) {
		createTie(points[i], points[i + 1]);
	}

	if (points.size() > 1) {
		createTie(points[points.size() - 1], points[points.size() - 2]);
	}
}

#pragma once

#include "framework/engine.h"

class Train {
public:
	explicit Train(Mesh & mesh, const glm::vec3 & start = glm::vec3(0.0f), const float speed = 0.1f)
		: m_object(nullptr), m_speed(speed), m_idx(0) {
		m_object = Engine::get()->createObject(&mesh);
		m_object->setPosition(start);
		m_object->setColor(0.2f, 0.0f, 0.0f);
		m_object->setScale(0.5f, 0.5f, 1.0f);
	}

public:
	void tutuuu(const std::vector<glm::vec3> & path) {
		if (!path.empty() && translate(path[m_idx])) {
			if (m_idx < path.size() - 1) {
				++m_idx;
			} else {
				m_idx = 0;
			}
		}
	}

private:
	bool translate(const glm::vec3 & to) const {
		if ((distance(m_object->getPosition(), to)) >= m_speed) {
			const glm::vec3 position = m_object->getPosition();
			const glm::vec3 forward = normalize(to - position);
			m_object->setPosition(position + forward * m_speed);
			m_object->setRotation(quatLookAt(forward, { 0.0f, 1.0f, 0.0f }));
			return false;
		}
		return true;
	}

public:
	Object * getObject() const {
		return m_object;
	}

	float getSpeed() const {
		return m_speed;
	}

private:
	Object * m_object;
	float m_speed;

private:
	std::size_t m_idx;
};

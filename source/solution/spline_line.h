#pragma once

#include "glm/vec3.hpp"

class SplineLine {
public:
	SplineLine() = default;
	SplineLine(const glm::vec3 & p1, const glm::vec3 & p2, const float velocity = .0f) : m_p1(p1), m_p2(p2) {}

	~SplineLine() = default;

	SplineLine(const SplineLine & other) = default;

	SplineLine(SplineLine && other) noexcept : m_p1({}), m_p2({}) {
		if (this != &other) {
			m_p1 = std::exchange(other.m_p1, {});
			m_p2 = std::exchange(other.m_p2, {});
		}
	}

	SplineLine & operator=(const SplineLine & other) = default;

	SplineLine & operator=(SplineLine && other) noexcept {
		if (this != &other) {
			m_p1 = std::exchange(other.m_p1, {});
			m_p2 = std::exchange(other.m_p2, {});
		}
		return *this;
	}

public:
	float distance() const {
		return glm::distance(m_p1, m_p2);
	}

	glm::vec3 lerp(const float dt) const {
		const float f_dt = glm::fract(dt);
		return {
			m_p1.x + f_dt * (m_p2.x - m_p1.x),
			m_p1.y + f_dt * (m_p2.y - m_p1.y),
			m_p1.z + f_dt * (m_p2.z - m_p1.z)
		};
	}

	glm::vec3 lerp(const float dt, const float speed) const {
		const float a_dt = dt / (distance() / speed);
		return {
			m_p1.x + a_dt * (m_p2.x - m_p1.x),
			m_p1.y + a_dt * (m_p2.y - m_p1.y),
			m_p1.z + a_dt * (m_p2.z - m_p1.z)
		};
	}

public:
	const glm::vec3 & getFirst() const {
		return m_p1;
	}

	const glm::vec3 & getSecond() const {
		return m_p2;
	}

	std::pair<glm::vec3, glm::vec3> getPoints() const {
		return { m_p1, m_p2 };
	}

public:
	void setFirst(const glm::vec3 & point) {
		m_p1 = point;
	}

	void setSecond(const glm::vec3 & point) {
		m_p2 = point;
	}

private:
	glm::vec3 m_p1;
	glm::vec3 m_p2;
};

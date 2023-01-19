#pragma once

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/spline.hpp"

#include "spline_line.h"

class SplineSegment {
public:
	SplineSegment() = default;
	explicit SplineSegment(const std::initializer_list<SplineLine> & lines) : m_lines(lines) {}

	explicit SplineSegment(const glm::vec3 & point) : m_lines({}) {
		pushBack(point);
	}

	~SplineSegment() = default;

	SplineSegment(const SplineSegment &) = default;
	SplineSegment(SplineSegment &&) noexcept = default;

	SplineSegment & operator=(const SplineSegment &) = default;
	SplineSegment & operator=(SplineSegment &&) = default;

public:
	SplineSegment & construct(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3, const glm::vec3 & p4,
	                          const float eps = 0.01f) {
		float dt = eps;
		do {
			m_lines.emplace_back(catmullRom(p1, p2, p3, p4, dt - eps), catmullRom(p1, p2, p3, p4, dt));
			dt += eps;
		} while (dt <= 1.0f);
		return *this;
	}

	float distance() const {
		float result = .0f;
		for (const auto & line : m_lines) {
			result += line.distance();
		}
		return result;
	}

	glm::vec3 get(const float dt) const {
		const float interval = glm::fract(dt) * static_cast<float>(m_lines.size());
		const auto idx = glm::clamp<std::size_t>(interval, 0, m_lines.size() - 1);
		return m_lines[idx].lerp(interval);
	}

public:
	void pushFront(const glm::vec3 & point) {
		if (!empty()) {
			if (begin()->getFirst() == begin()->getSecond()) {
				begin()->setFirst(point);
			} else {
				m_lines.insert(m_lines.begin(), { point, static_cast<glm::vec3>(begin()->getFirst()) });
			}
		} else {
			m_lines.emplace_back(point, point);
		}
	}

	void pushBack(const glm::vec3 & point) {
		if (!empty()) {
			const auto last = --end();
			if (last->getFirst() == last->getSecond()) {
				last->setSecond(point);
			} else {
				m_lines.emplace_back(static_cast<glm::vec3>(last->getSecond()), point);
			}
		} else {
			m_lines.emplace_back(point, point);
		}
	}

public:
	void linkFront(const SplineSegment & segment) {
		this->pushFront(segment.getBack().getSecond());
	}

	void linkBack(const SplineSegment & segment) {
		this->pushBack(segment.getFront().getFirst());
	}

public:
	std::vector<glm::vec3> toVector() const {
		std::vector<glm::vec3> result;
		result.reserve(m_lines.size());
		for (const auto & line : m_lines) {
			result.emplace_back(line.getFirst());
		}
		return result;
	}

public:
	const SplineLine & operator[](const std::size_t idx) const {
		return m_lines.operator[](idx);
	}

	bool empty() const noexcept {
		return m_lines.empty();
	}

public:
	std::vector<SplineLine>::iterator begin() noexcept {
		return m_lines.begin();
	}

	std::vector<SplineLine>::iterator end() noexcept {
		return m_lines.end();
	}

	std::vector<SplineLine>::const_iterator begin() const noexcept {
		return m_lines.cbegin();
	}

	std::vector<SplineLine>::const_iterator end() const noexcept {
		return m_lines.cend();
	}

public:
	const SplineLine & getFront() const {
		return m_lines.front();
	}

	const SplineLine & getBack() const {
		return m_lines.back();
	}

	const std::vector<SplineLine> & getLines() const {
		return m_lines;
	}

private:
	std::vector<SplineLine> m_lines;
};

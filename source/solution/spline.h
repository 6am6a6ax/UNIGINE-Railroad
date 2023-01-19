#pragma once

#include "spline_segment.h"

class Spline {
public:
	Spline() : m_segments({}), m_isLoop(false) {}
	explicit Spline(const bool isLoop) : m_segments({}), m_isLoop(isLoop) {}

	explicit Spline(const std::initializer_list<SplineSegment> & segments, const bool isLoop = false) :
		m_segments(segments),
		m_isLoop(isLoop) {}

	explicit Spline(const std::vector<glm::vec3> & path, const float eps = 0.01f,
	                const bool isLoop = false) : m_segments({}), m_isLoop(isLoop) {
		construct(path, eps, isLoop);
	}

	~Spline() = default;

	Spline(const Spline &) = default;
	Spline(Spline &&) noexcept = default;

	Spline & operator=(const Spline &) = default;
	Spline & operator=(Spline &&) noexcept = default;

public:
	Spline & construct(const std::vector<glm::vec3> & path, const float eps = 0.01f, const bool isLoop = false) {
		m_isLoop = isLoop;
		if (m_segments.empty()) {
			m_segments.clear();
			m_segments.reserve(path.size());
		}
		for (std::size_t i = 0; i < path.size(); i++) {
			auto i0 = glm::clamp<std::size_t>(i - 1, 0, path.size() - 1);
			auto i1 = glm::clamp<std::size_t>(i, 0, path.size() - 1);
			auto i2 = glm::clamp<std::size_t>(i + 1, 0, path.size() - 1);
			auto i3 = glm::clamp<std::size_t>(i + 2, 0, path.size() - 1);

			if (m_isLoop) {
				if (i == 0) {
					i0 = path.size() - 1;
				} else if (i == path.size() - 2) {
					i3 = 0;
				} else if (i == path.size() - 1) {
					i2 = 0;
					i3 = path.size() == 1 ? 0 : 1;
				}
			} else {
				if (i1 == path.size() - 1) {
					break;
				}
			}
			m_segments.emplace_back(std::move(SplineSegment().construct(path[i0], path[i1], path[i2], path[i3], eps)));
		}
		return *this;
	}

	float distance() const {
		float res = .0f;
		for (const auto & segment : m_segments) {
			res += segment.distance();
		}
		for (std::size_t i = 0; i < m_segments.size() - 1; i++) {
			const glm::vec3 front = m_segments[i + 1].getFront().getFirst();
			const glm::vec3 back = m_segments[i].getBack().getSecond();
			res += glm::distance(front, back);
		}
		if (!m_segments.empty() && m_isLoop) {
			const glm::vec3 front = m_segments.front().getFront().getFirst();
			const glm::vec3 back = m_segments.back().getBack().getSecond();
			res += glm::distance(front, back);
		}
		return res;
	}

	glm::vec3 get(const float dt) const {
		const float interval = glm::fract(dt) * static_cast<float>(m_segments.size());
		const auto idx = glm::clamp<std::size_t>(static_cast<std::size_t>(interval), 0, m_segments.size() - 1);
		return m_segments[idx].get(interval);
	}

public:
	void pushBack(const glm::vec3 & point) {
		if (!empty()) {
			m_segments.back().pushBack(point);
		} else {
			m_segments.emplace_back(point);
		}
	}

	std::vector<glm::vec3> toVector() const {
		std::vector<glm::vec3> result;
		result.reserve(m_segments.size());
		for (const auto & segment : m_segments) {
			std::vector<glm::vec3> points = segment.toVector();
			result.insert(result.end(), points.begin(), points.end());
		}
		if (!m_segments.empty() && m_isLoop) {
			result.emplace_back(m_segments.back().getBack().getSecond());
		}
		return result;
	}

public:
	static Spline approx(const Spline & spline, const std::size_t n = 60, const float eps = .001f) {
		Spline result(spline.isLoop());
		float haul = .0f;
		float minimum = std::numeric_limits<float>::max();
		const float interval = spline.distance() / static_cast<float>(n);
		float dt = eps;
		do {
			if (result.empty()) {
				result.pushBack(spline.get(dt - eps));
				continue;
			}

			const glm::vec3 p1 = spline.get(dt - eps);
			const glm::vec3 p2 = spline.get(dt);

			const float dx = p2.x - p1.x;
			const float dy = p2.y - p1.y;
			const float dz = p2.z - p1.z;

			const float vx = dx * dt;
			const float vy = dy * dt;
			const float vz = dz * dt;

			const float velocity = sqrtf(vx * vx + vy * vy + vz * vz);
			haul += velocity / dt;
			const float delta = fabs(haul - interval);

			if (delta < minimum) {
				minimum = delta;
				dt += eps;
			} else {
				haul = .0f;
				minimum = std::numeric_limits<float>::max();
				result.pushBack(p2);
			}
		} while (dt <= 1.0f);
		return result;
	}

	static Spline approx2(const Spline & spline, const std::size_t n = 60, const float eps = .001f) {
		return approx(approx(spline, n, eps), n, eps);
	}

public:
	const SplineSegment & operator[](const std::size_t idx) const {
		return m_segments.operator[](idx);
	}

	bool empty() const noexcept {
		return m_segments.empty();
	}

public:
	std::vector<SplineSegment>::iterator begin() noexcept {
		return m_segments.begin();
	}

	std::vector<SplineSegment>::iterator end() noexcept {
		return m_segments.end();
	}

	std::vector<SplineSegment>::const_iterator begin() const noexcept {
		return m_segments.cbegin();
	}

	std::vector<SplineSegment>::const_iterator end() const noexcept {
		return m_segments.cend();
	}

public:
	const std::vector<SplineSegment> & getSegments() const {
		return m_segments;
	}

	bool isLoop() const {
		return m_isLoop;
	}

private:
	std::vector<SplineSegment> m_segments;
	bool m_isLoop;
};

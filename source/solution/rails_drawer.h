#pragma once

#include <vector>
#include "framework/engine.h"

class RailsDrawer
{
public:
    explicit RailsDrawer(
        const std::vector<glm::vec3> & points,
        const bool                     loop       = false,
        const float                    trackWidth = 0.2f,
        const float                    railWidth  = 1.4f,
        const glm::vec3 &              color      = { 0.15f, 0.15f, 0.15f }
    )
        : m_leftRail(Mesh())
      , m_rightRail(Mesh())
      , m_color(color)
    {
        setPoints(points, loop, trackWidth, railWidth);
    }

    void setPoints(
        const std::vector<glm::vec3> & points,
        const bool                     loop       = false,
        const float                    trackWidth = 0.2f,
        const float                    railWidth  = 1.4f
    )
    {
        std::vector<Vertex>       leftVertices;
        std::vector<Vertex>       rightVertices;
        std::vector<unsigned int> indices;
        for (std::size_t i = 0; i < points.size() - 1; i++)
        {
            const glm::vec3 forward = normalize((points[i + 1] - points[i]));
            const glm::vec3 up      = { 0.0f, 1.0f, 0.0f };
            const glm::vec3 right   = cross(forward, up);

            leftVertices.push_back({ points[i] - right * trackWidth * railWidth, up, glm::vec2(0.0f) });
            leftVertices.push_back({ points[i] - right * trackWidth, up, glm::vec2(0.0f) });

            rightVertices.push_back({ points[i] + right * trackWidth, up, glm::vec2(0.0f) });
            rightVertices.push_back({ points[i] + right * trackWidth * railWidth, up, glm::vec2(0.0f) });
        }

        for (unsigned int i = 0; i < points.size() * 2 - 4; i += 2)
        {
            indices.push_back(i);
            indices.push_back(i + 1);
            indices.push_back(i + 2);

            indices.push_back(i + 1);
            indices.push_back(i + 3);
            indices.push_back(i + 2);
        }

        if (!indices.empty() && loop)
        {
            indices.push_back(static_cast<unsigned int>(leftVertices.size() - 2));
            indices.push_back(static_cast<unsigned int>(leftVertices.size() - 1));
            indices.push_back(0);

            indices.push_back(static_cast<unsigned int>(leftVertices.size() - 1));
            indices.push_back(1);
            indices.push_back(0);
        }

        m_leftRail.set(leftVertices, indices);
        m_rightRail.set(rightVertices, indices);
    }

    void setColor(const glm::vec3 & color)
    {
        this->m_color = color;
    }

    void setColor(const float r, const float g, const float b)
    {
        m_color = glm::vec3(r, g, b);
    }

    const glm::vec3 & getColor() const
    {
        return m_color;
    }

    void draw()
    {
        Engine * engine = Engine::get();

        engine->getShader().setMat4("model", glm::mat4(1.0f));
        engine->getShader().setVec3("albedo", m_color);

        m_leftRail.draw(GL_TRIANGLES);
        m_rightRail.draw(GL_TRIANGLES);
    }

private:
    Mesh      m_leftRail;
    Mesh      m_rightRail;
    glm::vec3 m_color;
};

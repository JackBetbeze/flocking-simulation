#pragma once

#include "cgra/mesh.hpp"
#include "cgra/shader.hpp"
#include "cgra/matrix.hpp"
#include "flock.hpp"
#include "predator.hpp"
#include "obstacle.hpp"
#include "glm/glm.hpp"

class Application {
public:
    // The window object managed by GLFW
    GLFWwindow *m_window;

    // The shader program used for drawing
    cgra::Program m_program;

    // Bounding Box mesh
    cgra::Mesh m_boxMesh;
	glm::vec3 m_boxBounds;

	// FLOCK ADDITIONS
	int m_numFlocks;
	Flock m_flock1;
	Flock m_flock2;
	Predator m_pred;
	bool m_predBool;
	float m_neighborRadius;
	float m_cohesion;
	float m_alignment;
	float m_avoidance;
	double m_time;
	float m_timestep;

	// OBSTACLE ADDITIONS
	bool m_obBool;
	Obstacle m_ob1;
	Obstacle m_ob2;
	Obstacle m_ob3;

    // The current size of the viewport
    glm::vec2 m_viewportSize;
    // The current mouse position
    glm::vec2 m_mousePosition;

    // The translation of the mesh as a vec3
    glm::vec3 m_translation;
    // The scale of the mesh
    float m_scale;

    // A 4x4 matrix representing the rotation of the
    // mesh
    glm::mat4 m_rotationMatrix;

    // Whether or not the left, middle or right buttons are down.
    bool m_mouseButtonDown[3];

    Application(GLFWwindow *win)
        : m_window(win),
          m_viewportSize(1, 1), m_mousePosition(0, 0),
          m_translation(0), m_scale(1), m_rotationMatrix(1), m_predBool(false), m_obBool(false),
		  m_boxBounds(20.0, 20.0, 20.0), m_flock1(5, true, m_boxBounds), 
		  m_flock2(5, false, m_boxBounds), m_pred(m_boxBounds, 1.0f), m_neighborRadius(15.0), m_numFlocks(1),
		  m_cohesion(5.0), m_alignment(5.0), m_avoidance(5.0), m_timestep(10),
		  m_ob1(glm::vec3(10.0, 10.0, 10.0), 5), m_ob2(glm::vec3(-10.0, 1.0, -10.0), 2), m_ob3(glm::vec3(-10.0, -10.0, 5.0), 3) {
        m_mouseButtonDown[0] = false;
        m_mouseButtonDown[1] = false;
        m_mouseButtonDown[2] = false;
    }

    void setWindowSize(int width, int height) {
        m_viewportSize.x = float(width);
        m_viewportSize.y = float(height);
    }

    void init();

    void createBox();

    void drawScene();
    void doGUI();

    void onKey(int key, int scancode, int action, int mods);

    void onMouseButton(int button, int action, int mods);

    void onCursorPos(double xpos, double ypos);

    void onScroll(double xoffset, double yoffset);
};

#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "opengl.h"
#include "imgui.h"

#include "cgra/matrix.hpp"

#include "ex.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

using namespace std;

void Application::init() {
	// Load the shader program
	// The use of CGRA_SRCDIR "/path/to/shader" is so you don't
	// have to run the program from a specific folder.
	m_program = cgra::Program::load_program(
		CGRA_SRCDIR "/res/shaders/simple.vs.glsl",
		CGRA_SRCDIR "/res/shaders/simple.fs.glsl");

	// Create a view matrix that positions the camera
	// 10 units behind the object
	glm::mat4 viewMatrix(1);
	viewMatrix[3] = glm::vec4(0, 0, -50, 1);
	m_program.setViewMatrix(viewMatrix);

	// Create the cube mesh
	createBox();

	// Get first frame time
	m_time = glfwGetTime();

	// Initialize the flock(s)
	//initFlock();
}

void Application::createBox() {

	cgra::Matrix<double> vertices(8, 3);
	cgra::Matrix<unsigned int> lines(12, 2);

	vertices.setRow(0, { m_boxBounds.x,  m_boxBounds.y,  m_boxBounds.z });
	vertices.setRow(1, { -m_boxBounds.x, -m_boxBounds.y,  m_boxBounds.z });
	vertices.setRow(2, { m_boxBounds.x, -m_boxBounds.y,  m_boxBounds.z });
	vertices.setRow(3, { -m_boxBounds.x,  m_boxBounds.y,  m_boxBounds.z });
	vertices.setRow(4, { -m_boxBounds.x,  m_boxBounds.y,  -m_boxBounds.z });
	vertices.setRow(5, { -m_boxBounds.x, -m_boxBounds.y,  -m_boxBounds.z });
	vertices.setRow(6, { m_boxBounds.x,  m_boxBounds.y,  -m_boxBounds.z });
	vertices.setRow(7, { m_boxBounds.x, -m_boxBounds.y,  -m_boxBounds.z });


	lines.setRow(0, { 0, 3 });
	lines.setRow(1, { 3, 4 });
	lines.setRow(2, { 4, 6 });
	lines.setRow(3, { 6, 0 });
	lines.setRow(4, { 2, 1 });
	lines.setRow(5, { 1, 5 });
	lines.setRow(6, { 5, 7 });
	lines.setRow(7, { 7, 2 });
	lines.setRow(8, { 0, 2 });
	lines.setRow(9, { 3, 1 });
	lines.setRow(10, { 4, 5 });
	lines.setRow(11, { 6, 7 });


	m_boxMesh.setData(vertices, lines);

}

void Application::drawScene() {
	// NEW FRAME
	double timeDelta = glfwGetTime() - m_time;
	m_time = glfwGetTime();

	// Calculate the aspect ratio of the viewport;
	// width / height
	float aspectRatio = m_viewportSize.x / m_viewportSize.y;
	// Calculate the projection matrix with a field-of-view of 45 degrees
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

	// Set the projection matrix
	m_program.setProjectionMatrix(projectionMatrix);

	// Draw the bouding box mesh
	glm::mat4 modelTransform(1.0f);
	m_program.setModelMatrix(modelTransform);	
	m_boxMesh.draw(GL_LINES);

	// Iterate through boids and draw flocks
	for (int i = 0; i < m_flock1.numBoids; i++) {
		glm::vec3 baseDir(0.0f, 0.0f, 1.0f);
		glm::mat4 modelTransform(1.0f);

		modelTransform = glm::translate(modelTransform, glm::vec3(m_flock1.flockBoids.at(i).boidPosition.x, m_flock1.flockBoids.at(i).boidPosition.y, m_flock1.flockBoids.at(i).boidPosition.z));
		modelTransform = glm::rotate(modelTransform, acos(glm::dot(baseDir, m_flock1.flockBoids.at(i).boidDirection) / (glm::length(baseDir)*glm::length(m_flock1.flockBoids.at(i).boidDirection))), glm::cross(baseDir, glm::normalize(m_flock1.flockBoids.at(i).boidDirection)));
		
		m_program.setModelMatrix(modelTransform);
		m_flock1.boidMesh.draw(GL_TRIANGLES);
	}

	if (m_numFlocks == 2) {
		for (int i = 0; i < m_flock2.numBoids; i++) {
			glm::vec3 baseDir(0.0f, 0.0f, 1.0f);
			glm::mat4 modelTransform(1.0f);

			modelTransform = glm::translate(modelTransform, glm::vec3(m_flock2.flockBoids.at(i).boidPosition.x, m_flock2.flockBoids.at(i).boidPosition.y, m_flock2.flockBoids.at(i).boidPosition.z));
			modelTransform = glm::rotate(modelTransform, acos(glm::dot(baseDir, m_flock2.flockBoids.at(i).boidDirection) / (glm::length(baseDir)*glm::length(m_flock2.flockBoids.at(i).boidDirection))), glm::cross(baseDir, glm::normalize(m_flock2.flockBoids.at(i).boidDirection)));
			
			m_program.setModelMatrix(modelTransform);
			m_flock2.boidMesh.draw(GL_TRIANGLES);
		}
	}

	// Draw Predator
	if (m_predBool == true) {
		glm::vec3 baseDir(0.0f, 0.0f, 1.0f);
		glm::mat4 modelTransform(1.0f);

		modelTransform = glm::translate(modelTransform, glm::vec3(m_pred.predPosition.x, m_pred.predPosition.y, m_pred.predPosition.z));
		modelTransform = glm::rotate(modelTransform, acos(glm::dot(baseDir, m_pred.predDirection) / (glm::length(baseDir)*glm::length(m_pred.predDirection))), glm::cross(baseDir, glm::normalize(m_pred.predDirection)));

		m_program.setModelMatrix(modelTransform);
		m_pred.predMesh.draw(GL_TRIANGLES);
	}

	// Draw Obstacles
	if (m_obBool == true) {
		glm::mat4 modelTransform(1.0f);
		modelTransform = glm::translate(modelTransform, m_ob1.obPosition);
		modelTransform = glm::scale(modelTransform, glm::vec3(m_ob1.obRadius));
		m_program.setModelMatrix(modelTransform);
		m_ob1.obMesh.draw(GL_TRIANGLES);

		modelTransform = glm::mat4(1.0f);
		modelTransform = glm::translate(modelTransform, m_ob2.obPosition);
		modelTransform = glm::scale(modelTransform, glm::vec3(m_ob2.obRadius));
		m_program.setModelMatrix(modelTransform);
		m_ob2.obMesh.draw(GL_TRIANGLES);

		modelTransform = glm::mat4(1.0f);
		modelTransform = glm::translate(modelTransform, m_ob3.obPosition);
		modelTransform = glm::scale(modelTransform, glm::vec3(m_ob3.obRadius));
		m_program.setModelMatrix(modelTransform);
		m_ob3.obMesh.draw(GL_TRIANGLES);
	}

	// Determine next positions of boids
	for (int i = 0; i < m_flock1.numBoids; i++) {
		glm::vec3 fbound(0.0, 0.0, 0.0);
		glm::vec3 fob(0.0, 0.0, 0.0);
		glm::vec3 fpred(0.0, 0.0, 0.0);
		glm::vec3 cent_sum(0.0, 0.0, 0.0);
		glm::vec3 align_sum(0.0, 0.0, 0.0);
		glm::vec3 avoid_sum(0.0, 0.0, 0.0);

		// Detect boundaries
		if (m_flock1.flockBoids.at(i).boidPosition.x > (m_boxBounds.x - 5.0f)) {
			fbound.x -= (m_flock1.flockBoids.at(i).boidPosition.x / m_boxBounds.x);
		}
		if (m_flock1.flockBoids.at(i).boidPosition.x < (-m_boxBounds.x + 5.0f)) {
			fbound.x += (-m_flock1.flockBoids.at(i).boidPosition.x / m_boxBounds.x);
		}
		if (m_flock1.flockBoids.at(i).boidPosition.y >(m_boxBounds.y - 5.0f)) {
			fbound.y -= (m_flock1.flockBoids.at(i).boidPosition.y / m_boxBounds.y);
		}
		if (m_flock1.flockBoids.at(i).boidPosition.y < (-m_boxBounds.y + 5.0f)) {
			fbound.y += (-m_flock1.flockBoids.at(i).boidPosition.y / m_boxBounds.y);
		}
		if (m_flock1.flockBoids.at(i).boidPosition.z >(m_boxBounds.z - 5.0f)) {
			fbound.z -= (m_flock1.flockBoids.at(i).boidPosition.z / m_boxBounds.z);
		}
		if (m_flock1.flockBoids.at(i).boidPosition.z < (-m_boxBounds.z + 5.0f)) {
			fbound.z += (-m_flock1.flockBoids.at(i).boidPosition.z / m_boxBounds.z);
		}

		// Detect obstacles
		if (m_obBool == true) {
			if ((glm::distance(m_flock1.flockBoids.at(i).boidPosition, m_ob1.obPosition) - m_ob1.obRadius) < 10.0f) {
				fob += 5.0f*glm::normalize(m_flock1.flockBoids.at(i).boidPosition + m_ob1.obPosition);
			}
			if ((glm::distance(m_flock1.flockBoids.at(i).boidPosition, m_ob2.obPosition) - m_ob2.obRadius) < 10.0f) {
				fob += 5.0f*glm::normalize(m_flock1.flockBoids.at(i).boidPosition + m_ob2.obPosition);
			}
			if ((glm::distance(m_flock1.flockBoids.at(i).boidPosition, m_ob3.obPosition) - m_ob3.obRadius) < 10.0f) {
				fob += 5.0f*glm::normalize(m_flock1.flockBoids.at(i).boidPosition + m_ob3.obPosition);
			}
		}

		int count = 0;
		for (int j = 0; j < m_flock1.numBoids; j++) {
			if (m_neighborRadius > glm::distance(m_flock1.flockBoids.at(i).boidPosition, m_flock1.flockBoids.at(j).boidPosition)) {
				if (m_flock1.flockBoids.at(i).boidPosition != m_flock1.flockBoids.at(j).boidPosition) {
					cent_sum += m_flock1.flockBoids.at(j).boidPosition;
					align_sum += m_flock1.flockBoids.at(j).boidDirection;
					avoid_sum += ((1 / glm::distance(m_flock1.flockBoids.at(i).boidPosition, m_flock1.flockBoids.at(j).boidPosition)) * ((m_flock1.flockBoids.at(i).boidPosition - m_flock1.flockBoids.at(j).boidPosition) / glm::distance(m_flock1.flockBoids.at(i).boidPosition, m_flock1.flockBoids.at(j).boidPosition)));
					count++;
				}
			}
		}
		if (m_predBool == true) {
			if (m_neighborRadius > glm::distance(m_flock1.flockBoids.at(i).boidPosition, m_pred.predPosition)) {
				fpred = m_pred.predPosition - m_flock1.flockBoids.at(i).boidPosition;
			}
		}
		if (count > 0) {
			glm::vec3 fco = m_cohesion*((cent_sum/(float)count) - m_flock1.flockBoids.at(i).boidPosition);
			glm::vec3 fal = m_alignment*((align_sum / (float)count) - glm::normalize(m_flock1.flockBoids.at(i).boidDirection));
			glm::vec3 fav = m_avoidance*(avoid_sum);
			m_flock1.flockBoids.at(i).next(fbound, fco, fal, fav, fpred, fob, m_timestep * (float)timeDelta);
		}
		else {
			m_flock1.flockBoids.at(i).next(fbound, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), fpred, fob, m_timestep * (float)timeDelta);
		}
	}

	if (m_numFlocks == 2) {
		for (int i = 0; i < m_flock2.numBoids; i++) {
			glm::vec3 fbound(0.0, 0.0, 0.0);
			glm::vec3 fob(0.0, 0.0, 0.0);
			glm::vec3 fpred(0.0, 0.0, 0.0);
			glm::vec3 cent_sum(0.0, 0.0, 0.0);
			glm::vec3 align_sum(0.0, 0.0, 0.0);
			glm::vec3 avoid_sum(0.0, 0.0, 0.0);

			// Detect boundaries
			if (m_flock2.flockBoids.at(i).boidPosition.x >(m_boxBounds.x - 5.0f)) {
				fbound.x -= (m_flock2.flockBoids.at(i).boidPosition.x / m_boxBounds.x);
			}
			if (m_flock2.flockBoids.at(i).boidPosition.x < (-m_boxBounds.x + 5.0f)) {
				fbound.x += (-m_flock2.flockBoids.at(i).boidPosition.x / m_boxBounds.x);
			}
			if (m_flock2.flockBoids.at(i).boidPosition.y >(m_boxBounds.y - 5.0f)) {
				fbound.y -= (m_flock2.flockBoids.at(i).boidPosition.y / m_boxBounds.y);
			}
			if (m_flock2.flockBoids.at(i).boidPosition.y < (-m_boxBounds.y + 5.0f)) {
				fbound.y += (-m_flock2.flockBoids.at(i).boidPosition.y / m_boxBounds.y);
			}
			if (m_flock2.flockBoids.at(i).boidPosition.z >(m_boxBounds.z - 5.0f)) {
				fbound.z -= (m_flock2.flockBoids.at(i).boidPosition.z / m_boxBounds.z);
			}
			if (m_flock2.flockBoids.at(i).boidPosition.z < (-m_boxBounds.z + 5.0f)) {
				fbound.z += (-m_flock2.flockBoids.at(i).boidPosition.z / m_boxBounds.z);
			}

			// Detect obstacles
			if (m_obBool == true) {
				if ((glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob1.obPosition) - m_ob1.obRadius) < 10.0f) {
					fob += ((1 / glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob1.obPosition)) * ((m_flock2.flockBoids.at(i).boidPosition - m_ob1.obPosition) / glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob1.obPosition)));
				}
				if ((glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob2.obPosition) - m_ob2.obRadius) < 10.0f) {
					fob += ((1 / glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob2.obPosition)) * ((m_flock2.flockBoids.at(i).boidPosition - m_ob2.obPosition) / glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob2.obPosition)));
				}
				if ((glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob3.obPosition) - m_ob3.obRadius) < 10.0f) {
					fob += ((1 / glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob3.obPosition)) * ((m_flock2.flockBoids.at(i).boidPosition - m_ob3.obPosition) / glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_ob3.obPosition)));
				}
			}


			int count = 0;
			for (int j = 0; j < m_flock2.numBoids; j++) {
				if (m_neighborRadius > glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_flock2.flockBoids.at(j).boidPosition)) {
					if (m_flock2.flockBoids.at(i).boidPosition != m_flock2.flockBoids.at(j).boidPosition) {
						cent_sum += m_flock2.flockBoids.at(j).boidPosition;
						align_sum += m_flock2.flockBoids.at(j).boidDirection;
						avoid_sum += ((1 / glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_flock2.flockBoids.at(j).boidPosition)) * ((m_flock2.flockBoids.at(i).boidPosition - m_flock2.flockBoids.at(j).boidPosition) / glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_flock2.flockBoids.at(j).boidPosition)));
						count++;
					}
				}
			}
			if (m_predBool == true) {
				if (m_neighborRadius > glm::distance(m_flock2.flockBoids.at(i).boidPosition, m_pred.predPosition)) {
					fpred = m_pred.predPosition - m_flock2.flockBoids.at(i).boidPosition;
				}
			}
			if (count > 0) {
				glm::vec3 fco = m_cohesion*((cent_sum / (float)count) - m_flock2.flockBoids.at(i).boidPosition);
				glm::vec3 fal = m_alignment*((align_sum / (float)count) - glm::normalize(m_flock2.flockBoids.at(i).boidDirection));
				glm::vec3 fav = m_avoidance*(avoid_sum);
				m_flock2.flockBoids.at(i).next(fbound, fco, fal, fav, fpred, fob, m_timestep * (float)timeDelta);
			}
			else {
				m_flock2.flockBoids.at(i).next(fbound, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), fpred, fob, m_timestep * (float)timeDelta);
			}
		}
	}

	// Determine next position of predator
	if (m_predBool == true) {
		glm::vec3 fbound(0.0, 0.0, 0.0);
		glm::vec3 prey(1000000000.0, 10000000000.0, 10000000000.0);

		// Detect boundaries
		if (m_pred.predPosition.x > (m_boxBounds.x - 5.0f)) {
			fbound.x -= (m_pred.predPosition.x / m_boxBounds.x);
		}
		if (m_pred.predPosition.x < (-m_boxBounds.x + 5.0f)) {
			fbound.x += (-m_pred.predPosition.x / m_boxBounds.x);
		}
		if (m_pred.predPosition.y > (m_boxBounds.y - 5.0f)) {
			fbound.y -= (m_pred.predPosition.y / m_boxBounds.y);
		}
		if (m_pred.predPosition.y < (-m_boxBounds.y + 5.0f)) {
			fbound.y += (-m_pred.predPosition.y / m_boxBounds.y);
		}
		if (m_pred.predPosition.z > (m_boxBounds.z - 5.0f)) {
			fbound.z -= (m_pred.predPosition.z / m_boxBounds.z);
		}
		if (m_pred.predPosition.z < (-m_boxBounds.z + 5.0f)) {
			fbound.z += (-m_pred.predPosition.z / m_boxBounds.z);
		}

		for (int i = 0; i < m_flock1.numBoids; i++) {
			if (m_neighborRadius > glm::distance(m_pred.predPosition, m_flock1.flockBoids.at(i).boidPosition)) {
				if (m_pred.predPosition != m_flock1.flockBoids.at(i).boidPosition) {
					if (glm::distance(m_pred.predPosition, m_flock1.flockBoids.at(i).boidPosition) < glm::distance(m_pred.predPosition, prey)) {
						prey = m_flock1.flockBoids.at(i).boidPosition;
					}
				}
			}
			if (m_numFlocks == 2) {
				if (m_neighborRadius > glm::distance(m_pred.predPosition, m_flock2.flockBoids.at(i).boidPosition)) {
					if (m_pred.predPosition != m_flock2.flockBoids.at(i).boidPosition) {
						if (glm::distance(m_pred.predPosition, m_flock2.flockBoids.at(i).boidPosition) < glm::distance(m_pred.predPosition, prey)) {
							prey = m_flock2.flockBoids.at(i).boidPosition;
						}
					}
				}
			}
		}

		glm::vec3 f = (prey - m_pred.predPosition);
		m_pred.next(fbound, f, m_timestep * (float)timeDelta);
	}

}

void Application::doGUI() {
	ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Boids - Flocking Simulation");

	// Timestep coefficient
	static float t = 10.0f;
	if (ImGui::SliderFloat("Timestep", &t, 0.0f, 50.0f)) {
		m_timestep = t;
	}

	// Load bounding box
	static glm::vec3 boxSize(20.0, 20.0, 20.0);
	if (ImGui::SliderFloat3("Bounding Box", &boxSize[0], 0.0, 100.0)) {
		m_boxBounds = boxSize;
		createBox();
	}

	static int i = 0;
	if (ImGui::SliderInt("Number of Flocks", &i, 1, 2)) {
		m_numFlocks = i;
		//initFlock();
	}

	static float r = 15.0f;
	if (ImGui::SliderFloat("Neighborhood Radius", &r, 0.0, 70.0)) {
		m_neighborRadius = r;
	}

	// Cohesion, Alignment, Avoidance Sliders
	static float co = 5.0f;
	if (ImGui::SliderFloat("Cohesion", &co, 0.0, 50.0)) {
		m_cohesion = co;
	}

	static float al = 5.0f;
	if (ImGui::SliderFloat("Alignment", &al, 0.0, 50.0)) {
		m_alignment = al;
	}

	static float av = 5.0f;
	if (ImGui::SliderFloat("Avoidance", &av, 0.0, 50.0)) {
		m_avoidance = av;
	}

	// Number of boids per flock
	static int num = 5;
	if (ImGui::SliderInt("Number of Boids", &num, 1, 300)) {
		m_flock1 = Flock::Flock(num, true, m_boxBounds);
		m_flock2 = Flock::Flock(num, false, m_boxBounds);
	}
	
	// Predator stuff
	static bool checked = false;
	if (ImGui::Checkbox("Toggle Predator", &checked)) {
		if (checked) m_predBool = true;
		else m_predBool = false;
	}

	static float m = 1;
	if (ImGui::SliderFloat("Predator Mass", &m, 0.0f, 10.0f)) {
		if (m == 0.0) {
			m = 0.1f;
		}
		m_pred = Predator::Predator(m_boxBounds, m);
	}

	// Obstacles
	static bool q = false;
	if (ImGui::Checkbox("Toggle Obstacles", &q)) {
		if (q) m_obBool = true;
		else m_obBool = false;
	}

	ImGui::End();
}


// Input Handlers

void Application::onMouseButton(int button, int action, int) {
	if (button >= 0 && button < 3) {
		// Set the 'down' state for the appropriate mouse button
		m_mouseButtonDown[button] = action == GLFW_PRESS;
	}
}

void Application::onCursorPos(double xpos, double ypos) {

	// Make a vec2 with the current mouse position
	glm::vec2 currentMousePosition(xpos, ypos);

	// Get the difference from the previous mouse position
	glm::vec2 mousePositionDelta = currentMousePosition - m_mousePosition;

	if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_LEFT]) {
		glm::mat4 model(1.0f);
		glm::vec2 p1((m_mousePosition[0] / (m_viewportSize[0])), -(m_mousePosition[1] / m_viewportSize[1]));
		glm::vec3 v1(p1[0], p1[1], abs(sqrt(1 - (pow(p1[0], 2.0) + (pow(p1[1], 2.0))))));

		glm::vec2 p2(((float)currentMousePosition[0] / m_viewportSize[0]), -((float)currentMousePosition[1] / m_viewportSize[1]));
		glm::vec3 v2(p2[0], p2[1], abs(sqrt(1 - (pow(p2[0], 2.0) + (pow(p2[1], 2.0))))));

		m_rotationMatrix *= glm::rotate(model, 3 * acos(glm::dot(v1, v2) / (glm::length(v1)*glm::length(v2))), glm::cross(v1, v2));

	}
	else if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_MIDDLE]) {
		m_translation += glm::vec3((float)mousePositionDelta[0] / 100.0f, -(float)mousePositionDelta[1] / 100.0f, 0.0f);

	}
	else if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_RIGHT]) {
		m_scale -= ((float)mousePositionDelta[1] / 300.0f);
	}

	// Update the mouse position to the current one
	m_mousePosition = currentMousePosition;
}

void Application::onKey(int key, int scancode, int action, int mods) {
	// `(void)foo` suppresses unused variable warnings
	(void)key;
	(void)scancode;
	(void)action;
	(void)mods;
}

void Application::onScroll(double xoffset, double yoffset) {
	// `(void)foo` suppresses unused variable warnings
	(void)xoffset;
	(void)yoffset;
}

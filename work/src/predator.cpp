// predator.cpp

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include "predator.hpp"

Predator::Predator(
	glm::vec3 bounds,
	float mass)
	: predMass(mass) {

	// Load mesh depending on type
	unsigned int numVertices = 0;
	unsigned int numTriangles = 0;
	std::vector<float> x_vertices;
	std::vector<float> y_vertices;
	std::vector<float> z_vertices;
	std::vector<unsigned int> face1;
	std::vector<unsigned int> face2;
	std::vector<unsigned int> face3;

	std::string temp_line, temp_first;
	float temp_vertex;
	unsigned int temp_face;

	std::ifstream boidObj("res/models/predator_boid.obj");
	if (boidObj.is_open()) {
		while (getline(boidObj, temp_line)) {
			std::istringstream iss(temp_line);
			iss >> temp_first;
			if (temp_first == "v") {
				numVertices++;
				iss >> temp_vertex;
				x_vertices.push_back(temp_vertex);
				iss >> temp_vertex;
				y_vertices.push_back(temp_vertex);
				iss >> temp_vertex;
				z_vertices.push_back(temp_vertex);
			}
			if (temp_first == "f") {
				numTriangles++;
				iss >> temp_face;
				face1.push_back(temp_face);
				iss >> temp_face;
				face2.push_back(temp_face);
				iss >> temp_face;
				face3.push_back(temp_face);
			}

		}
	}
	boidObj.close();

	cgra::Matrix<double> vertices(numVertices, 3);
	cgra::Matrix<unsigned int> triangles(numTriangles, 3);

	for (size_t i = 0; i < numVertices; i++) {
		vertices.setRow(i, { x_vertices[i],  y_vertices[i],  z_vertices[i] });
	}

	for (size_t i = 0; i < numTriangles; i++) {
		triangles.setRow(i, { face1[i] - 1, face2[i] - 1, face3[i] - 1 });
	}

	predMesh.setData(vertices, triangles);

	// Initialize boids with random positions depending on numBoids and bounding box size
	std::random_device rdx; // obtain a random number from hardware
	std::mt19937 engx(rdx()); // seed the generator
	std::uniform_real_distribution<> distrx(-bounds.x, bounds.x); // define the range

	std::random_device rdy; // obtain a random number from hardware
	std::mt19937 engy(rdy()); // seed the generator
	std::uniform_real_distribution<> distry(-bounds.y, bounds.y); // define the range

	std::random_device rdz; // obtain a random number from hardware
	std::mt19937 engz(rdz()); // seed the generator
	std::uniform_real_distribution<> distrz(-bounds.z, bounds.z); // define the range

	std::random_device rda; // obtain a random number from hardware
	std::mt19937 enga(rda()); // seed the generator
	std::uniform_real_distribution<> distra(-5, 5); // define the range

	predPosition = glm::vec3(distrx(engx), distry(engy), distrz(engz));
	predDirection = glm::normalize(glm::vec3(distra(enga), distra(enga), distra(enga)));

}

void Predator::next(glm::vec3 fbound, glm::vec3 f, float h) {
	predDirection = glm::normalize(predDirection);
	predDirection += ((glm::normalize(f) + fbound) * h);
	predPosition += ((glm::normalize(predDirection) / predMass) * h);
}
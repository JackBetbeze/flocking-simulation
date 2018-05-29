// obstacle.hpp

#include "glm/glm.hpp"
#include "cgra/matrix.hpp"
#include "cgra/mesh.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

class Obstacle {
public:
	glm::vec3 obPosition;
	float obRadius;
	cgra::Mesh obMesh;

	Obstacle(glm::vec3 position,
		float radius)
		: obPosition(position),
		obRadius(radius) {

		// Load sphere obstacle meshes
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

		std::ifstream sphereObj("res/models/sphere.obj");
		if (sphereObj.is_open()) {
			while (getline(sphereObj, temp_line)) {
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
		sphereObj.close();

		cgra::Matrix<double> vertices(numVertices, 3);
		cgra::Matrix<unsigned int> triangles(numTriangles, 3);

		for (size_t i = 0; i < numVertices; i++) {
			vertices.setRow(i, { x_vertices[i],  y_vertices[i],  z_vertices[i] });
		}

		for (size_t i = 0; i < numTriangles; i++) {
			triangles.setRow(i, { face1[i] - 1, face2[i] - 1, face3[i] - 1 });
		}

		obMesh.setData(vertices, triangles);

	}

};
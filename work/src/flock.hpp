#include <vector>
#include "glm/glm.hpp"
#include "cgra/matrix.hpp"
#include "cgra/mesh.hpp"

class Boid;
class Flock {
public:
	std::vector<Boid> flockBoids;
	int numBoids;
	bool boidType;
	cgra::Mesh boidMesh;

	Flock(int num,
		  bool type,
		  glm::vec3 bounds);

};

class Boid {
public:
	// True for boid1, false for boid2
	bool boidType;
	glm::vec3 boidPosition;
	glm::vec3 boidDirection;

	Boid(bool type)
		: boidType(type) {}

	void next(glm::vec3 fbound, glm::vec3 fco, glm::vec3 fal, glm::vec3 fav, glm::vec3 fp, glm::vec3 fo, float h) {
		glm::vec3 a = fco + fal + fav - fp + (5.0f * fo);
		boidDirection = glm::normalize(boidDirection);
		boidDirection += ((glm::normalize(a)+fbound) * h);
		boidPosition += (glm::normalize(boidDirection) * h);
	}
};
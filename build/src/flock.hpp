#include <vector>
#include "glm/glm.hpp"
#include "cgra/matrix.hpp"
#include "cgra/mesh.hpp"

class Boid;
class Flock {
	std::vector<Boid> flockBoids;
public:
	int numBoids;
	bool boidType;
	cgra::Mesh boidMesh;

	Flock(int num,
		  bool type,
		  glm::vec3 bounds);

	void drawFlock();

};

class Boid {
public:
	// True for boid1, false for boid2
	bool boidType;
	glm::vec3 boidPosition;
	glm::vec3 boidDirection;

	Boid(bool type)
		: boidType(type) {}
};
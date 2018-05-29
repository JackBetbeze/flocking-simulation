#include "glm/glm.hpp"
#include "cgra/matrix.hpp"
#include "cgra/mesh.hpp"

class Predator {
public:
	glm::vec3 predPosition;
	glm::vec3 predDirection;
	cgra::Mesh predMesh;
	float predMass;

	Predator(glm::vec3 bounds,
			 float mass);

	void next(glm::vec3 fbound, glm::vec3 f, float h);
};
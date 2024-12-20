#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
using namespace std;

void part1() {
	cout << "Vector operations" << endl;

	// vectors
	glm::vec3		v1(1, 2, 3);
	glm::vec3		v2(3, 2, 1);
	cout << " v1 = " << to_string(v1) << endl;
	cout << " v2 = " << to_string(v2) << endl;

	//addition, subtraction
	cout << " v1 + v2 = " << to_string(v1 + v2) << endl;
	cout << " v1 - v2 = " << to_string(v1 - v2) << endl;
	// negation
	cout << " -v1 = " << to_string(-v1) << endl;
	// scalar multiplicatio and subtraction
	cout << " v1 - 2.0*v2 = " << to_string(v1 - 2.0f * v2) << endl;
	// dot product
	cout << " dot(v1, v2) = " << dot(v1, v2) << endl;
	// cross product
	cout << " cross(v1, v2) = " << to_string(cross(v1, v2)) << endl;
}

void part2() {
	cout << "Matrix operations" << endl;

	// vectors
	glm::vec3		v1(1, 2, 3);
	glm::vec3		v2(3, 2, 1);
	cout << " v1 = " << to_string(v1) << endl;
	cout << " v2 = " << to_string(v2) << endl;
	// matrices
	glm::mat3	A1(1.0, 2.0, 1.0, 2.0, 3.0, 1.0, 3.0, 2.0, 2.0);
	glm::mat3	A2(2.0, 2.0, 1.0, 1.0, 2.0, 1.0, 2.0, 1.0, 1.0);
	cout << " A1 = " << to_string(A1) << endl;
	cout << " A2 = " << to_string(A2) << endl;

	// additon, subtraction
	cout << " A1 + A2 = " << to_string(A1 + A2) << endl;
	cout << " A1 - A2 = " << to_string(A1 - A2) << endl;
	// negation
	cout << " -A1 = " << to_string(-A1) << endl;
	// scalar multiplication, subtraction
	cout << " A1 - 2.0*A2 = " << to_string(A1 - 2.0f * A2) << endl;
	// matrix multiplication
	cout << " A1 x A2 = " << to_string(A1 * A2) << endl;
	cout << " A2 x A1 = " << to_string(A2 * A1) << endl;
	//matrix-vector multiplication
	cout << " A1 x v1 = " << to_string(A1 * v1) << endl;
	cout << " A2 x v2 = " << to_string(A2 * v2) << endl;

}

int main(int argc, char* argv[]) {
	int e = (argc < 2) ? 1 : atoi(argv[1]);

	switch (e) {
	case 1: part1(); break;
	case 2: part2(); break;
	}

	return 0;
}
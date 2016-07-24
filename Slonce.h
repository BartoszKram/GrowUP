#pragma once

#include <math.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <cstdio>
#include "GL/glew.h"
#include "GL/glut.h"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


using namespace std;
using namespace glm;

namespace Slonca {
	class Slonce {
	public:
		vector<vec4> loc;
		vector<vec4> color;

		Slonce();
	};
	extern Slonce slonca;
}

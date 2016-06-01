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

namespace Models {
	class Model
	{
	public:
		vector<glm::vec3> vertices;
		vector<glm::vec2> uvs;
		vector<glm::vec3> normals;
		Model();
		Model(const char * nazwaModelu);
		void init(const char* nazwaModelu);
		void drawSolid();
		float* convert3(vector<glm::vec3> wektor);
		float* convert2(vector<glm::vec2> wektor);
		void loadModel(const char * nazwaModelu, std::vector < glm::vec3 > & out_vertices,
			std::vector < glm::vec2 > & out_uvs,
			std::vector < glm::vec3 > & out_normals);
	};
	extern Model model;
}


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

namespace Models {
	class Model
	{
	public:
		//Pola
		vector<vec3> vertices;
		vector<vec2> uvs;
		vector<vec3> normals;
		mat4 M;
		GLuint objectVao;
		GLuint vertexbuffer;
		GLuint vertexUV;
		GLuint bufNormals;

		GLint texModelu;
		GLint texOdbicia;
		float relLevel;

		vec3 rotacja;
		vec3 translacja;
		vec3 skalowanie;


		//Metody
		Model(const char * nazwaModelu, char* texModelu, char* texOdbicia, float relLevel);
		Model(const char * nazwaModelu, char* texModelu, char* texOdbicia, float relLevel, vec3 rotacja, vec3 translacja, vec3 skalowanie);
		void init(const char* nazwaModelu);
		void drawSolid();
		float* convert3(vector<vec3> wektor);
		float* convert2(vector<vec2> wektor);
		void setValue(GLuint value,GLuint &valueToSet);
		void loadModel(const char * nazwaModelu, std::vector < vec3 > & out_vertices,
			vector < vec2 > & out_uvs,
			vector < vec3 > & out_normals);
		GLuint readTexture(char* filename);
	};
	extern Model model;
}


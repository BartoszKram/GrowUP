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
#include "Model.h"


using namespace std;
using namespace glm;
using namespace Models;

class Czlowiek
{
	public:
		Czlowiek();
		Model GetModel(int n);
		Model GetAktModel();
		void ZmienStan();
		void ZacznijAnimacje();
		int Getn();
	
	private:
		vector<Model> modele;
		int stan;
		int n;
};
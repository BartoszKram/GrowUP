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
using namespace Models;

class Lvl{
public:
	Lvl(vector<vector<Model>> modele, int maxlvl);
	Lvl(vector<vector<Model>> modele, int maxlvl, vec3 rotacja, vec3 translacja, vec3 skalowanie);
	vector<Model>* GetModele(int n);
	vector<Model>* GetAktModele();
	int Getlvl();
	void Inclvl();
	int Getmaxlvl();
	
	vec3 rotacja;
	vec3 translacja;
	vec3 skalowanie;

private:
	int lvl;
	int maxlvl;
	vector<vector<Model>> modele;
	
};
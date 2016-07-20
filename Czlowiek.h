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
		Czlowiek(vec3 rotacja, vec3 translacja, vec3 skalowanie);
		Model* GetModel(int n);
		Model* GetAktModel();
		void ZmienStan();
		void ZacznijAnimacje();
		int Getn();
		bool GetAnimacja();
		void SetAnimacja();
		bool GetStan();
		void SetCel(vec3 cel);
		vec3 GetCel();
		void Idz();
		bool Zajety();
		void SpojzNa(vec3 cel);
		void Restart();

		vec3 rotacja;
		vec3 translacja;
		vec3 skalowanie;
	
	private:
		vector<Model> modele;
		int stan;
		int n;
		bool animacja;
		vec3 cel;
		int opoznienie;
};
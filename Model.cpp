#include "stdafx.h"
#include "Model.h"

namespace Models {

	Model model;

	Model::Model()
	{
	}

	Model::Model(const char * nazwaModelu) {
		loadModel(nazwaModelu, vertices, uvs, normals);
	}

	void Model::init(const char* nazwaModelu) {
		this->loadModel(nazwaModelu, vertices, uvs, normals);
	}

	void Model::setValue(GLuint value,GLuint &valueToSet) {
		valueToSet=value;
	};


	void Model::loadModel(const char * nazwaModelu, vector <vec3> & out_vertices,
		vector < vec2 > & out_uvs,
		vector < vec3 > & out_normals)
	{

		vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		vector< vec3 > tmp_vertices;
		vector< vec2 > tmp_uvs;
		vector< vec3 > tmp_normals;

		FILE* file;
		errno_t err;
		if ((err = fopen_s(&file, nazwaModelu, "r")) != NULL)
			printf("Nie odczytano pliku");
		else
			while (1) {
				char lineHeader[128];
				// read the first word of the line
				int res;
				if ((res = fscanf_s(file, "%s", lineHeader, 127)) == EOF)
					break; // Koniec pliku, wczytany obiekt
					//Sekcja odpowiedzialna za wczytywanie wierzcho³ków modelu
				if (strcmp(lineHeader, "v") == 0) {
					vec3 vertex;
					fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					tmp_vertices.push_back(vertex);
				}
				//Sekcja odpowiedzialna za wczytywanie uvs modelu
				else if (strcmp(lineHeader, "vt") == 0) {
					vec2 uv;
					fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
					tmp_uvs.push_back(uv);
				}
				//Sekcja odpowiedzialna za wczytywanie normalnych modelu
				else if (strcmp(lineHeader, "vn") == 0) {
					vec3 normal;
					fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
					tmp_normals.push_back(normal);
				}
				else if (strcmp(lineHeader, "f") == 0) {
					string vertex1, vertex2, vertex3;
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					if (matches != 9) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						break;
					}
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
			}
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			unsigned int vertexIndex = vertexIndices[i];
			vec3 vertex = tmp_vertices[vertexIndex - 1];
			out_vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < uvIndices.size(); i++) {
			unsigned int uvIndex = uvIndices[i];
			vec2 uv = tmp_uvs[uvIndex - 1];
			out_uvs.push_back(uv);
		}
		for (unsigned int i = 0; i < normalIndices.size(); i++) {
			unsigned int normalIndex = normalIndices[i];
			vec3 normal;
			if (tmp_normals[normalIndex-1] != vec3(0,0,0)){
			normal = tmp_normals[normalIndex-1];
				out_normals.push_back(normal); }
		}
		if (file != NULL)
			fclose(file);
	}

	float* Model::convert3(vector<vec3> wektor) {
		int size = wektor.size() * 3;
		float* flatArray = new float[size];
		int j = 0;
		for (int i = 0; i < wektor.size(); i++) {
			flatArray[j++] = wektor[i].x;
			flatArray[j++] = wektor[i].y;
			flatArray[j++] = wektor[i].z;
		}
		return flatArray;
	}

	float* Model::convert2(vector<vec2> wektor) {
		int size = wektor.size() * 2;
		float* flatArray = new float[size];
		int j = 0;
		for (int i = 0; i < wektor.size(); i++) {
			flatArray[j++] = wektor[i].x;
			flatArray[j++] = wektor[i].y;
		}
		return flatArray;
	}

	void Model::drawSolid() {
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, this->vertices.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glBindVertexArray(0);
	}
}
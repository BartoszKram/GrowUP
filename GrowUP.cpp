// GrowUP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "targetver.h"
#include "Model.h"
#include "shaderprogram.h"
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include "GL/glew.h"
#include "GL/glut.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace std;
using namespace glm;




Models::Model myModel("RoboDay.obj");
Models::Model cube("cube.obj");


float cameraSpeed_x = 0; // [radiany/s]
float cameraSpeed_y = 0; // [radiany/s]

				   //Uchwyty na shadery
ShaderProgram *shaderProgram; //Wska�nik na obiekt reprezentuj�cy program cieniuj�cy.
GLint tex0;



//Procedura obs�ugi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) cameraSpeed_y = -3.14;
		if (key == GLFW_KEY_RIGHT) cameraSpeed_y = 3.14;
		if (key == GLFW_KEY_UP) cameraSpeed_x = -3.14;
		if (key == GLFW_KEY_DOWN) cameraSpeed_x = 3.14;
	}


	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) cameraSpeed_y = 0;
		if (key == GLFW_KEY_RIGHT) cameraSpeed_y = 0;
		if (key == GLFW_KEY_UP) cameraSpeed_x = 0;
		if (key == GLFW_KEY_DOWN) cameraSpeed_x = 0;
	}
}

//Procedura obs�ugi b��d�w
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

GLuint readTexture(char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	//Wczytanie do pami�ci komputera
	std::vector<unsigned char> image; //Alokuj wektor do wczytania obrazka
	unsigned width, height; //Zmienne do kt�rych wczytamy wymiary obrazka
							//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);
	//Import do pami�ci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
									   //Wczytaj obrazek do pami�ci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

void loadObjectVBO(Models::Model &model,mat4 modelMatrix) {
	//Zbuduj VBO z danymi obiektu do narysowania
	GLuint vb, vuv, vn;
	GLuint vao;
	//vb = makeBuffer(model.convert3(model.vertices), model.vertices.size(), sizeof(glm::vec3)); //VBO ze wsp�rz�dnymi wierzcho�k�w
	//vuv = makeBuffer(model.convert2(model.uvs), model.uvs.size(), sizeof(glm::vec2));//VBO z UV
	//vn = makeBuffer(model.convert3(model.normals), model.normals.size(), sizeof(glm::vec3));//VBO z wektorami normalnymi wierzcho�k�w

	glGenVertexArrays(1, &vao); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej
	glBindVertexArray(vao); //Uaktywnij nowo utworzony VAO

	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), &model.vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vuv);
	glBindBuffer(GL_ARRAY_BUFFER, vuv);
	glBufferData(GL_ARRAY_BUFFER, model.uvs.size() * sizeof(glm::vec2), &model.uvs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &vn);
	glBindBuffer(GL_ARRAY_BUFFER, vn);
	glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), &model.normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	model.setValue(vao, model.objectVao);
	model.setValue(vb, model.vertexbuffer);
	model.setValue(vuv, model.vertexUV);
	model.setValue(vn, model.bufNormals);
	model.M = modelMatrix;

	//	assignVBOtoAttribute(shaderProgram, "vertex", vb, 3); //"vertex" odnosi si� do deklaracji "in vec4 vertex;" w vertex shaderze
	//	assignVBOtoAttribute(shaderProgram, "vertexUV", vuv, 2); //"vertexUV" odnosi si� do deklaracji "in vec2 vertexUV;" w vertex shaderze
	//	assignVBOtoAttribute(shaderProgram, "normal", vn, 3); //"bufNormals" odnosi si� do deklaracji "in vec3 normal;" w vertex shaderze
}

//Procedura inicjuj�ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, kt�ry nale�y wykona� raz, na pocz�tku programu************

	glClearColor(0, 0, 0.5, 1); //Czy�� ekran na czarno
	glEnable(GL_DEPTH_TEST); //W��cz u�ywanie Z-Bufora

	glfwSetKeyCallback(window, key_callback); //Zarejestruj procedur� obs�ugi klawiatury

	shaderProgram = new ShaderProgram("vshader.txt", NULL, "fshader.txt"); //Wczytaj program cieniuj�cy 

	//Cz�� wczytuj�ca modele i ustawiaj�ca je na miejscach
	mat4 matrix = glm::mat4(1.0f);
	loadObjectVBO(myModel,matrix);

	matrix = glm::rotate(matrix, 10.0f, glm::vec3(0,1,0));
	matrix = glm::translate(matrix, glm::vec3(2.0f, 0.0f, 0.0f));
	loadObjectVBO(cube, matrix);

	//Cz�� wczytuj�ca tekstury
	tex0 = readTexture("example2.png");
}



//Zwolnienie zasob�w zaj�tych przez program
void freeOpenGLProgram() {
	delete shaderProgram; //Usuni�cie programu cieniuj�cego
}

void drawObject(Models::Model model, ShaderProgram *shaderProgram, mat4 mV, mat4 mP) {

	/*TODO LIST
	1. Stworzy� modele i roz�o�y� je odpowiednio na siatki.
	2. Ze stworzonych modeli zrobi� model scenerii (�eby wiedzie� co i gdzie ustawi�)
	3. Animacje powinny by� animacjami obracaj�cymi ca�y obiekt.
	*/

	shaderProgram->use();

	//1.Aktualizacja uniform�w
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(model.M));
	glUniform4f(shaderProgram->getUniformLocation("lightPos0"), 0, 0, -5, 1); //Przekazanie wsp�rz�dnych �r�d�a �wiat�a do zmiennej jednorodnej lightPos0

	//2.Bindowanie tekstury
	glUniform1i(shaderProgram->getUniformLocation("myTextureSampler"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	//glVertexAttribPointer(locTex, model.vertices.size()*sizeof(glm::vec2), GL_FLOAT, GL_FALSE,0,NULL);

	//*****Przygotowanie do rysowania pojedynczego obiektu*******
	//3.Bindowanie Vao
	glBindVertexArray(model.objectVao);

	//4.Narysowanie obiektu
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(mV*model.M));

	glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());

	glBindVertexArray(0);
}

//Procedura rysuj�ca zawarto�� sceny
void drawScene(GLFWwindow* window, float camera_x, float camera_y) {
	//************Tutaj umieszczaj kod rysuj�cy obraz******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolor�w

	glm::mat4 V = glm::lookAt( //Wylicz macierz widoku
		glm::vec3(0.0f, 0.0f, -10.0f), //oddalenie
		glm::vec3(-2.0f, 2.0f, 0.0f), // wysoko��
		glm::vec3(0.0f, 10.0f, 0.0f));

	//Obracamy nasz� kamer� - nie zmieniamy po�o�enia modeli
	V = glm::rotate(V, camera_x, glm::vec3(1, 0, 0));
	V = glm::rotate(V, camera_y, glm::vec3(0, 1, 0));

	glm::mat4 P = glm::perspective(50 * 3.14f / 180, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	//P = glm::rotate(P, camera_x, glm::vec3(1, 0, 0));
	//P = glm::rotate(P, camera_y, glm::vec3(0, 1, 0));
																		 //Za�aduj macierze do OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(P));
	
	//myModel.M = glm::rotate(myModel.M, camera_y, vec3(0, 1, 0));
	drawObject(myModel, shaderProgram, V, P);
	drawObject(cube, shaderProgram, V, P);

	glDisableVertexAttribArray(0);
	//Przerzu� tylny bufor na przedni
	glfwSwapBuffers(window);

}



int main(void)
{
	GLFWwindow* window; //Wska�nik na obiekt reprezentuj�cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedur� obs�ugi b��d�w

	if (!glfwInit()) { //Zainicjuj bibliotek� GLFW
		fprintf(stderr, "Nie mo�na zainicjowa� GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utw�rz okno 500x500 o tytule "OpenGL" i kontekst OpenGL. 

	if (!window) //Je�eli okna nie uda�o si� utworzy�, to zamknij program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	float angle_x = 0; //K�t obrotu kamery w osi x
	float angle_y = 0; //K�t obrotu kamery w osi y

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje si� aktywny i polecenia OpenGL b�d� dotyczy� w�a�nie jego.
	glfwSwapInterval(1); //Czekaj na 1 powr�t plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotek� GLEW
		fprintf(stderr, "Nie mo�na zainicjowa� GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjuj�ce

	glfwSetTime(0); //Wyzeruj licznik czasu

					//G��wna p�tla
	while (!glfwWindowShouldClose(window)) //Tak d�ugo jak okno nie powinno zosta� zamkni�te
	{
		angle_x += cameraSpeed_x*glfwGetTime(); //Zwi�ksz k�t o pr�dko�� k�tow� razy czas jaki up�yn�� od poprzedniej klatki
		angle_y += cameraSpeed_y*glfwGetTime(); //Zwi�ksz k�t o pr�dko�� k�tow� razy czas jaki up�yn�� od poprzedniej klatki
		glfwSetTime(0); //Wyzeruj licznik czasu
		/*
		Tu b�d� wykonywane wszystkie modyfikacje macierzy modeli. Pozwoli to na 
		wykonywanie animacji na pojedynczych modelach nie ruszaj�c innych. Obs�uga klawiszy musi by�
		blokowana przyk�adow� zmienn� "enable" do czasu zako�czenia animacji. Kod b�dzie brzydki ale dzia�aj�cy ;)
		*/
		drawScene(window, angle_x, angle_y); //Wykonaj procedur� rysuj�c�
		glfwPollEvents(); //Wykonaj procedury callback w zalezno�ci od zdarze� jakie zasz�y.
	}

	freeOpenGLProgram();

	glfwDestroyWindow(window); //Usu� kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zaj�te przez GLFW
	exit(EXIT_SUCCESS);
}
// GrowUP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "targetver.h"
#include "Model.h"
#include "shaderprogram.h"
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
Models::Model myModel2("cube.obj");

GLuint vertexbuffer;

//Procedura obs³ugi b³êdów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************
	glClearColor(0, 0, 0, 1); //Czyœæ ekran na czarno
	glEnable(GL_LIGHTING); //W³¹cz tryb cieniowania
	glEnable(GL_LIGHT0); //W³¹cz domyslne œwiat³o
	glEnable(GL_DEPTH_TEST); //W³¹cz u¿ywanie Z-Bufora
	glEnable(GL_COLOR_MATERIAL); //glColor3d ma modyfikowaæ w³asnoœci materia³u	

	// This will identify our vertex buffer
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	
}

//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window) {
	//************Tutaj umieszczaj kod rysuj¹cy obraz******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów

	glm::mat4 M = glm::mat4(1.0f);

	glm::mat4 V = glm::lookAt( //Wylicz macierz widoku
		glm::vec3(0.0f, 0.0f, -20.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 20.0f, 0.0f));

	glm::mat4 P = glm::perspective(50 * 3.14f / 180, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	//Za³aduj macierze do OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(P));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(V*M));

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glColor3d(0, 1, 0); //Rysuj na zielono	
	myModel.drawSolid();
	glColor3d(0, 0, 1); //Rysuj na zielono	
	myModel2.drawSolid();


	glDisableVertexAttribArray(0);
						 //Przerzuæ tylny bufor na przedni
	glfwSwapBuffers(window);

}

int main(void)
{
	GLFWwindow* window; //WskaŸnik na obiekt reprezentuj¹cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurê obs³ugi b³êdów

	if (!glfwInit()) { //Zainicjuj bibliotekê GLFW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL. 

	if (!window) //Je¿eli okna nie uda³o siê utworzyæ, to zamknij program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje siê aktywny i polecenia OpenGL bêd¹ dotyczyæ w³aœnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekê GLEW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjuj¹ce

	glfwSetTime(0); //Wyzeruj licznik czasu

					//G³ówna pêtla
	while (!glfwWindowShouldClose(window)) //Tak d³ugo jak okno nie powinno zostaæ zamkniête
	{
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window); //Wykonaj procedurê rysuj¹c¹
		glfwPollEvents(); //Wykonaj procedury callback w zaleznoœci od zdarzeñ jakie zasz³y.
	}

	glfwDestroyWindow(window); //Usuñ kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
	exit(EXIT_SUCCESS);
}

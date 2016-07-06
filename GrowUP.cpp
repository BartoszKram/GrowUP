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


float speed_x = 0; // [radiany/s]
float speed_y = 0; // [radiany/s]

//Uchwyty na shadery
ShaderProgram *shaderProgram; //WskaŸnik na obiekt reprezentuj¹cy program cieniuj¹cy.
GLuint vao;

GLint tex0;



//Procedura obs³ugi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_y = -3.14;
		if (key == GLFW_KEY_RIGHT) speed_y = 3.14;
		if (key == GLFW_KEY_UP) speed_x = -3.14;
		if (key == GLFW_KEY_DOWN) speed_x = 3.14;
	}


	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_y = 0;
		if (key == GLFW_KEY_RIGHT) speed_y = 0;
		if (key == GLFW_KEY_UP) speed_x = 0;
		if (key == GLFW_KEY_DOWN) speed_x = 0;
	}
}

//Procedura obs³ugi b³êdów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Tworzy bufor VBO z tablicy
GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1, &handle);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który bêdzie zawiera³ tablicê danych
	glBindBuffer(GL_ARRAY_BUFFER, handle);  //Uaktywnij wygenerowany uchwyt VBO 
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);//Wgraj tablicê do VBO

	return handle;
}


//Przypisuje bufor VBO do atrybutu 
void assignVBOtoAttribute(ShaderProgram *shaderProgram, char* attributeName, GLuint bufVBO, int vertexSize) {
	GLuint location = shaderProgram->getAttribLocation(attributeName); //Pobierz numery slotów dla atrybutu
	glBindBuffer(GL_ARRAY_BUFFER, bufVBO);  //Uaktywnij uchwyt VBO 
	glEnableVertexAttribArray(location); //W³¹cz u¿ywanie atrybutu o numerze slotu zapisanym w zmiennej location
	glVertexAttribPointer(location, vertexSize, GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu location maj¹ byæ brane z aktywnego VBO
}

GLuint readTexture(char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	//Wczytanie do pamiêci komputera
	std::vector<unsigned char> image; //Alokuj wektor do wczytania obrazka
	unsigned width, height; //Zmienne do których wczytamy wymiary obrazka
							//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);
	//Import do pamiêci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
									   //Wczytaj obrazek do pamiêci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

void loadObjectVBO(Models::Model &model) {
	//Zbuduj VBO z danymi obiektu do narysowania
	GLuint vb, vuv, vn;
	GLuint vao;
	//vb = makeBuffer(model.convert3(model.vertices), model.vertices.size(), sizeof(glm::vec3)); //VBO ze wspó³rzêdnymi wierzcho³ków
	//vuv = makeBuffer(model.convert2(model.uvs), model.uvs.size(), sizeof(glm::vec2));//VBO z UV
	//vn = makeBuffer(model.convert3(model.normals), model.normals.size(), sizeof(glm::vec3));//VBO z wektorami normalnymi wierzcho³ków

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

	model.setValue(vao,model.objectVao);
	model.setValue(vb, model.vertexbuffer);
	model.setValue(vuv, model.vertexUV);
	model.setValue(vn, model.bufNormals);

//	assignVBOtoAttribute(shaderProgram, "vertex", vb, 3); //"vertex" odnosi siê do deklaracji "in vec4 vertex;" w vertex shaderze
//	assignVBOtoAttribute(shaderProgram, "vertexUV", vuv, 2); //"vertexUV" odnosi siê do deklaracji "in vec2 vertexUV;" w vertex shaderze
//	assignVBOtoAttribute(shaderProgram, "normal", vn, 3); //"bufNormals" odnosi siê do deklaracji "in vec3 normal;" w vertex shaderze
}

//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************
	
	glClearColor(0, 0, 0.5, 1); //Czyœæ ekran na czarno
	glEnable(GL_DEPTH_TEST); //W³¹cz u¿ywanie Z-Bufora
	
	glfwSetKeyCallback(window, key_callback); //Zarejestruj procedurê obs³ugi klawiatury

	shaderProgram = new ShaderProgram("vshader.txt", NULL, "fshader.txt"); //Wczytaj program cieniuj¹cy 
	
	loadObjectVBO(myModel);
	loadObjectVBO(cube);
	tex0 = readTexture("example2.png");
}



//Zwolnienie zasobów zajêtych przez program
void freeOpenGLProgram() {
	delete shaderProgram; //Usuniêcie programu cieniuj¹cego
}

void drawObject(Models::Model model, ShaderProgram *shaderProgram, mat4 mV, mat4 mM, mat4 mP) {
	
	shaderProgram->use();

	//1.Aktualizacja uniformów
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(mM));
	glUniform4f(shaderProgram->getUniformLocation("lightPos0"), 0, 0, -5, 1); //Przekazanie wspó³rzêdnych Ÿród³a œwiat³a do zmiennej jednorodnej lightPos0
	
	glUniform1i(shaderProgram->getUniformLocation("myTextureSampler"), 0);

	//2.Bindowanie tekstury
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	//glVertexAttribPointer(locTex, model.vertices.size()*sizeof(glm::vec2), GL_FLOAT, GL_FALSE,0,NULL);
	
	//*****Przygotowanie do rysowania pojedynczego obiektu*******

	//3.Bindowanie Vao
	glBindVertexArray(model.objectVao);
	//4.Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES, 0,model.vertices.size());

	glBindVertexArray(0);
}

//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	//************Tutaj umieszczaj kod rysuj¹cy obraz******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów

	glm::mat4 V = glm::lookAt( //Wylicz macierz widoku
		glm::vec3(0.0f, 0.0f, -10.0f),
		glm::vec3(0.0f, 2.0f, 0.0f),
		glm::vec3(0.0f, 10.0f, 0.0f));

	//Wylicz macierz modelu rysowanego obiektu
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, angle_x, glm::vec3(1, 0, 0));
	M = glm::rotate(M, angle_y, glm::vec3(0, 1, 0));

	

	glm::mat4 P = glm::perspective(50 * 3.14f / 180, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	//Za³aduj macierze do OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(P));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(V*M));

	drawObject(myModel, shaderProgram, V, M, P);

	M = glm::scale(M, glm::vec3(0.5f));
	M = glm::translate(M, glm::vec3(4.0f, 0.0f, 0.0f));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(V*M));

	drawObject(cube,shaderProgram, V, M, P);

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

	float angle_x = 0; //K¹t obrotu obiektu
	float angle_y = 0; //K¹t obrotu obiektu

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
		angle_x += speed_x*glfwGetTime(); //Zwiêksz k¹t o prêdkoœæ k¹tow¹ razy czas jaki up³yn¹³ od poprzedniej klatki
		angle_y += speed_y*glfwGetTime(); //Zwiêksz k¹t o prêdkoœæ k¹tow¹ razy czas jaki up³yn¹³ od poprzedniej klatki
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, angle_x, angle_y); //Wykonaj procedurê rysuj¹c¹
		glfwPollEvents(); //Wykonaj procedury callback w zaleznoœci od zdarzeñ jakie zasz³y.
	}

	freeOpenGLProgram();

	glfwDestroyWindow(window); //Usuñ kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
	exit(EXIT_SUCCESS);
}

// GrowUP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

Lvl *nauka;
Lvl *praca;
Lvl *dom;
Lvl *alkohol;

Model *myModel;
Model *cube;
Model *dolar;
Model *ksiazka;

Czlowiek *czlowiek;

float cameraSpeed_x = 0; // [radiany/s]
float cameraSpeed_y = 0; // [radiany/s]
float cameraMove = 0;

				   //Uchwyty na shadery
ShaderProgram *shaderProgram; //WskaŸnik na obiekt reprezentuj¹cy program cieniuj¹cy.
GLint tex0;
GLint texKsiazka;
GLint texDolar;



//Procedura obs³ugi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) cameraSpeed_y = -3.14;
		if (key == GLFW_KEY_RIGHT) cameraSpeed_y = 3.14;
		if (key == GLFW_KEY_UP) cameraSpeed_x = -3.14;
		if (key == GLFW_KEY_DOWN) cameraSpeed_x = 3.14;
		if (key == GLFW_KEY_Q) czlowiek->SetAnimacja();
		if (key == GLFW_KEY_A) cameraMove = -3.14;
		if (key == GLFW_KEY_D) cameraMove = 3.14;
		if (key == GLFW_KEY_Z) nauka->Inclvl();
		if (key == GLFW_KEY_X) praca->Inclvl();
		if (key == GLFW_KEY_C) dom->Inclvl();
		if (key == GLFW_KEY_V) alkohol->Inclvl();

	}


	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) cameraSpeed_y = 0;
		if (key == GLFW_KEY_RIGHT) cameraSpeed_y = 0;
		if (key == GLFW_KEY_UP) cameraSpeed_x = 0;
		if (key == GLFW_KEY_DOWN) cameraSpeed_x = 0;
		if (key == GLFW_KEY_A) cameraMove = 0;
		if (key == GLFW_KEY_D) cameraMove = 0;
	}
}

//Procedura obs³ugi b³êdów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
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

void loadObjectVBO(Models::Model *model) {
	//Zbuduj VBO z danymi obiektu do narysowania
	GLuint vb, vuv, vn;
	GLuint vao;
	vb = makeBuffer((*model).convert3((*model).vertices), (*model).vertices.size(), sizeof(glm::vec3)); //VBO ze wspó³rzêdnymi wierzcho³ków
	vuv = makeBuffer((*model).convert2((*model).uvs), (*model).uvs.size(), sizeof(glm::vec2));//VBO z UV
	vn = makeBuffer((*model).convert3((*model).normals), (*model).normals.size(), sizeof(glm::vec3));//VBO z wektorami normalnymi wierzcho³ków
	
	glGenVertexArrays(1, &vao); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej
	glBindVertexArray(vao); //Uaktywnij nowo utworzony VAO

	assignVBOtoAttribute(shaderProgram, "vertex", vb, 3); //"vertex" odnosi siê do deklaracji "in vec4 vertex;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "vertexUV", vuv, 2); //"vertexUV" odnosi siê do deklaracji "in vec2 vertexUV;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "normal", vn, 3); //"bufNormals" odnosi siê do deklaracji "in vec3 normal;" w vertex shaderze

	(*model).setValue(vao, (*model).objectVao);
	(*model).setValue(vb, (*model).vertexbuffer);
	(*model).setValue(vuv, (*model).vertexUV);
	(*model).setValue(vn, (*model).bufNormals);
}

//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************

	glClearColor(0, 0, 0.5, 1); //Czyœæ ekran na czarno
	glEnable(GL_DEPTH_TEST); //W³¹cz u¿ywanie Z-Bufora

	glfwSetKeyCallback(window, key_callback); //Zarejestruj procedurê obs³ugi klawiatury

	shaderProgram = new ShaderProgram("vshader.txt", NULL, "fshader.txt"); //Wczytaj program cieniuj¹cy 

	//Czêœæ wczytuj¹ca modele i ustawiaj¹ca je na miejscach
	loadObjectVBO(myModel);
	loadObjectVBO(ksiazka);
	loadObjectVBO(dolar);
	loadObjectVBO(cube);

	for (int i = 0; i < czlowiek->Getn(); i++)
	{
		loadObjectVBO(czlowiek->GetModel(i));
	}

	vector<Model>* modele;

	for (int i = 0; i <= nauka->Getmaxlvl(); i++)
	{
		modele = nauka->GetModele(i);
		for (int j=0; j < modele->size(); j++)
		{
			loadObjectVBO(&modele->at(j));
		}
	}

	/*for (int i = 0; i <= praca->Getmaxlvl(); i++)
	{
		modele = praca->GetModele(i);
		for (int j = 0; j < modele->size(); j++)
		{
			loadObjectVBO(&modele->at(j));
		}
	}

	for (int i = 0; i <= dom->Getmaxlvl(); i++)
	{
		modele = dom->GetModele(i);
		for (int j = 0; j < modele->size(); j++)
		{
			loadObjectVBO(&modele->at(j));
		}
	}

	for (int i = 0; i <= alkohol->Getmaxlvl(); i++)
	{
		modele = alkohol->GetModele(i);
		for (int j = 0; j < modele->size(); j++)
		{
			loadObjectVBO(&modele->at(j));
		}
	}*/

	//Czêœæ wczytuj¹ca tekstury
	tex0 = readTexture("example2.png");
	texKsiazka = readTexture("Tekstury/Ksiazka.png");
	texDolar = readTexture("Tekstury/Dolar.png");
}

void LoadNauka()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 4;
	
	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - ksiazka
	Model ksiazka("Modele/Nauka1.obj", "Tekstury/Nauka1.png", "Tekstury/Nauka1.png", 3, vec3(0, 2, 1), vec3(0, 0, 0), vec3(0.45, 0.45, 0.45));
	modele.push_back(ksiazka);

	Model stol1("Modele/Nauka2.obj", "Tekstury/Nauka23.png", "Tekstury/Nauka23Ref.png", 3, vec3(0, 0, 0), vec3(-1.695, 0, 0), vec3(1, 1, 1));
	modele.push_back(stol1);
	
	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - stol i ksiazka
	//modele.push_back(ksiazka1);
	Model stol("Modele/Nauka2.obj", "Tekstury/Nauka23.png", "Tekstury/Nauka23Ref.png", 3, vec3(0,0,0), vec3(0,0,0), vec3(1,1,1));
	modele.push_back(stol);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - stol i ksiazka x5 (?)
	//modele.push_back(ksiazka1);
	Model stolKsiazki("Modele/Nauka3.obj", "Tekstury/Nauka23.png", "Tekstury/Nauka23Ref.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(stolKsiazki);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 4 - stol i komputer
	//modele.push_back(stol);
	Model komputer("Modele/Nauka4.obj", "Tekstury/Nauka4.png", "Tekstury/Nauka4Ref.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(komputer);

	poziomy.push_back(modele);
	
	nauka = new Lvl(poziomy, maxlvl);
}

void LoadPraca()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 0;
	praca = new Lvl(poziomy, maxlvl);

	/*vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 4;

	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - ksiazka
	Model ksiazka1("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka1);
	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - stol i ksiazka
	modele.push_back(ksiazka1);
	Model stol("RoboDay.obj", "example2.png", "example2.png", 3);
	modele.push_back(stol);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - stol i ksiazka x5 (?)
	modele.push_back(ksiazka1);
	Model ksiazka2("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka2);
	Model ksiazka3("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka3);
	Model ksiazka4("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka4);
	Model ksiazka5("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka5);
	modele.push_back(stol);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 4 - stol i komputer
	modele.push_back(stol);
	Model komputer("RoboDay.obj", "example2.png", "example2.png", 3);
	modele.push_back(komputer);

	poziomy.push_back(modele);

	nauka = new Lvl(poziomy, maxlvl);

	//myModel = new Model("RoboDay.obj", "example2.png", "example2.png", 3);
	
	//dolar = new Model("Modele/Banknot.obj", "Tekstury/Dolar.png", "Tekstury/Dolar.png", 3);*/
}

void LoadDom()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 0;
	dom = new Lvl(poziomy, maxlvl);

	/*vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 4;

	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - ksiazka
	Model ksiazka1("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka1);
	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - stol i ksiazka
	modele.push_back(ksiazka1);
	Model stol("RoboDay.obj", "example2.png", "example2.png", 3);
	modele.push_back(stol);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - stol i ksiazka x5 (?)
	modele.push_back(ksiazka1);
	Model ksiazka2("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka2);
	Model ksiazka3("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka3);
	Model ksiazka4("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka4);
	Model ksiazka5("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka5);
	modele.push_back(stol);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 4 - stol i komputer
	modele.push_back(stol);
	Model komputer("RoboDay.obj", "example2.png", "example2.png", 3);
	modele.push_back(komputer);

	poziomy.push_back(modele);

	nauka = new Lvl(poziomy, maxlvl);*/

}

void LoadAlkohol()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 0;
	/*
	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - ksiazka
	Model ksiazka1("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka1);
	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - stol i ksiazka
	modele.push_back(ksiazka1);
	Model stol("RoboDay.obj", "example2.png", "example2.png", 3);
	modele.push_back(stol);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - stol i ksiazka x5 (?)
	modele.push_back(ksiazka1);
	Model ksiazka2("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka2);
	Model ksiazka3("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka3);
	Model ksiazka4("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka4);
	Model ksiazka5("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ksiazka5);
	modele.push_back(stol);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 4 - stol i komputer
	modele.push_back(stol);
	Model komputer("RoboDay.obj", "example2.png", "example2.png", 3);
	modele.push_back(komputer);

	poziomy.push_back(modele);*/

	alkohol = new Lvl(poziomy, maxlvl);
}

void LoadLvl()
{
	LoadNauka();
	LoadPraca();
	LoadDom();
	LoadAlkohol();

	myModel = new Model("RoboDay.obj", "example2.png", "example2.png", 3);
	cube = new Model("cube.obj", "example2.png", "example2.png", 3);
	dolar = new Model("Modele/Banknot.obj", "Tekstury/Dolar.png", "Tekstury/Dolar.png", 3);
	ksiazka = new Model("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	czlowiek = new Czlowiek();
}

void DeleteLvl()
{
	delete nauka;
	delete praca;
	delete dom;
	delete alkohol;
	delete czlowiek;

	delete myModel;
	delete cube;
	delete dolar;
	delete ksiazka;
}

//Zwolnienie zasobów zajêtych przez program
void freeOpenGLProgram() {
	delete shaderProgram; //Usuniêcie programu cieniuj¹cego
}

void drawObject(Model model, ShaderProgram *shaderProgram,mat4 mV, mat4 mP, float rotation_x,float rotation_y) {

	/*TODO LIST
	1. Stworzyæ modele i roz³o¿yæ je odpowiednio na siatki.
	2. Ze stworzonych modeli zrobiæ model scenerii (¿eby wiedzieæ co i gdzie ustawiæ)
	3. Animacje powinny byæ animacjami obracaj¹cymi ca³y obiekt.
	*/

	mat4 modelMatrix = model.M;
	modelMatrix = scale(modelMatrix, model.skalowanie);
	modelMatrix = translate(modelMatrix, model.translacja);
	modelMatrix = translate(modelMatrix, model.rotacja);
	modelMatrix = rotate(modelMatrix, rotation_x, glm::vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, rotation_y, glm::vec3(0, 1, 0));
	
	shaderProgram->use();

	//1.Aktualizacja uniformów
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(modelMatrix));
	glUniform4f(shaderProgram->getUniformLocation("lightPos0"), 0, 2, 0, 1); //Przekazanie wspó³rzêdnych Ÿród³a œwiat³a do zmiennej jednorodnej lightPos0

	//2.Bindowanie tekstury
	glUniform1i(shaderProgram->getUniformLocation("myTextureSampler"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.texModelu);
	
	//3.Bindowanie Vao
	glBindVertexArray(model.objectVao);

	//4.Narysowanie obiektu
	
	glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());

	glBindVertexArray(0);
}

vec3 eye = vec3(0.0f, 5.0f, -5.0f);

//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_cam) {
	//************Tutaj umieszczaj kod rysuj¹cy obraz******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów

	mat4 P = glm::perspective(50 * 3.14f / 180, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	//if (angle_x!=0)
	eye = vec3(eye.x * cos(angle_cam) + eye.z * sin(angle_cam), eye.y, -eye.x * sin(angle_cam) + eye.z * cos(angle_cam));
	//eye = vec3(eye.x, eye.y, eye.z+1);

	mat4 V = glm::lookAt( //Wylicz macierz widoku
		eye,
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	
	vector<Model>* modele; //Modele z danego poziomu lvl
	
	//Nauka
	modele = nauka->GetAktModele();
	for (std::vector<Model>::iterator it = modele->begin(); it != modele->end(); ++it)
	{
		drawObject(*it, shaderProgram, V, P, angle_x, angle_y);
	}


	//drawObject(*myModel, shaderProgram, V, P, angle_x, angle_y);


	//drawObject(*cube, shaderProgram, V, P, angle_x, angle_y);

	//drawObject(*ksiazka, shaderProgram, V, P, angle_x, angle_y);
	//drawObject(*dolar, shaderProgram, V, P, angle_x, angle_y);
	

	//Czlowiek
	/*if (czlowiek.GetAnimacja() == true && !czlowiek.GetStan())
	{
		czlowiek.ZacznijAnimacje();
	}
	else{
		czlowiek.ZmienStan();
	}

	drawObject(*czlowiek.GetAktModel(), shaderProgram, V, P, angle_x, angle_y);*/


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

	float angle_x = 0; //K¹t obrotu kamery w osi x
	float angle_y = 0; //K¹t obrotu kamery w osi y
	float angle_cam = 0; //K¹t obrotu kamery wokó³ osi y

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje siê aktywny i polecenia OpenGL bêd¹ dotyczyæ w³aœnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekê GLEW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLEW.\n");
		exit(EXIT_FAILURE);
	}
	
	//Laduje modele poziomow i czlowieka
	LoadLvl();
	nauka->Inclvl();
	initOpenGLProgram(window); //Operacje inicjuj¹ce

	glfwSetTime(0); //Wyzeruj licznik czasu

					//G³ówna pêtla
	while (!glfwWindowShouldClose(window)) //Tak d³ugo jak okno nie powinno zostaæ zamkniête
	{
		if (glfwGetTime() > 0.01)
		{
			angle_cam = cameraMove * glfwGetTime();
			angle_x += cameraSpeed_x*glfwGetTime(); //Zwiêksz k¹t o prêdkoœæ k¹tow¹ razy czas jaki up³yn¹³ od poprzedniej klatki
			angle_y += cameraSpeed_y*glfwGetTime(); //Zwiêksz k¹t o prêdkoœæ k¹tow¹ razy czas jaki up³yn¹³ od poprzedniej klatki
			glfwSetTime(0); //Wyzeruj licznik czasu
			/*
			Tu bêd¹ wykonywane wszystkie modyfikacje macierzy modeli. Pozwoli to na
			wykonywanie animacji na pojedynczych modelach nie ruszaj¹c innych. Obs³uga klawiszy musi byæ
			blokowana przyk³adow¹ zmienn¹ "enable" do czasu zakoñczenia animacji. Kod bêdzie brzydki ale dzia³aj¹cy ;)
			*/
			drawScene(window, angle_x, angle_y, angle_cam); //Wykonaj procedurê rysuj¹c¹
			glfwPollEvents(); //Wykonaj procedury callback w zaleznoœci od zdarzeñ jakie zasz³y.
		}
	}

	freeOpenGLProgram();
	DeleteLvl();

	glfwDestroyWindow(window); //Usuñ kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
	exit(EXIT_SUCCESS);
}

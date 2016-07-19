// GrowUP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//Sciezki gry
Lvl *nauka;
Lvl *praca;
Lvl *krzeslo;
Lvl *alkohol;

//Student chodzacy po planszy
Czlowiek *czlowiek;

//SkyBox
Model *skybox;

//vector umiejscowienia kamery
vec3 eye = vec3(0.0f, 8.0f, 14.0f);

float cameraSpeed_x = 0; // [radiany/s] obrot obiektu wokol osi x (pozniej do wywalenia)
float cameraSpeed_y = 0; // [radiany/s] obrot obiektu wokol osi y (pozniej do wywalenia)
float cameraMove = 0; // [radiany/s] obrot kamery z czasie gry

				   //Uchwyty na shadery
ShaderProgram *shaderProgram; //Wska�nik na obiekt reprezentuj�cy program cieniuj�cy.

//Wybor gracza
const int BRAK = 0;
const int NAUKA = 1;
const int PRACA = 2;
const int KRZESLO = 3;
const int ALKOHOL = 4;

//koniec ruchu
const int KONIEC = 0;

//przejdz na pozycje startowa
const int START = 0;

int opoznienie = 0;

struct SStan
{
	int wartosc = 0;
	int wybrany = BRAK;
	int nr_ruchu = 1;
	int sekwencja = 0;
	int decyzja = KONIEC;
}stangry, *stany;

int ilstanow;

bool ifwybrany[5] = { 0 };

//Procedura obs�ugi klawiatury
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
		if (key == GLFW_KEY_C) krzeslo->Inclvl();
		if (key == GLFW_KEY_V) alkohol->Inclvl();
		if (stangry.wybrany == BRAK)
		{
			if (key == GLFW_KEY_1 && !ifwybrany[NAUKA])
			{
				stangry.wybrany = NAUKA;
				ifwybrany[NAUKA] = true;
			}
			if (key == GLFW_KEY_2 && !ifwybrany[PRACA]) 
			{
				stangry.wybrany = PRACA;
				ifwybrany[PRACA] = true;
			}
			if (key == GLFW_KEY_3 && !ifwybrany[KRZESLO]) 
			{
				stangry.wybrany = KRZESLO;
				ifwybrany[KRZESLO] = true;
			}
			if (key == GLFW_KEY_4 && !ifwybrany[ALKOHOL])
			{
				stangry.wybrany = ALKOHOL;
				ifwybrany[ALKOHOL] = true;
			}
		}
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

//Procedura obs�ugi b��d�w
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Tworzy bufor VBO z tablicy
GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1, &handle);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), kt�ry b�dzie zawiera� tablic� danych
	glBindBuffer(GL_ARRAY_BUFFER, handle);  //Uaktywnij wygenerowany uchwyt VBO 
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);//Wgraj tablic� do VBO

	return handle;
}

//Przypisuje bufor VBO do atrybutu 
void assignVBOtoAttribute(ShaderProgram *shaderProgram, char* attributeName, GLuint bufVBO, int vertexSize) {
	GLuint location = shaderProgram->getAttribLocation(attributeName); //Pobierz numery slot�w dla atrybutu
	glBindBuffer(GL_ARRAY_BUFFER, bufVBO);  //Uaktywnij uchwyt VBO 
	glEnableVertexAttribArray(location); //W��cz u�ywanie atrybutu o numerze slotu zapisanym w zmiennej location
	glVertexAttribPointer(location, vertexSize, GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu location maj� by� brane z aktywnego VBO
}

void loadObjectVBO(Models::Model *model) {
	//Zbuduj VBO z danymi obiektu do narysowania
	GLuint vb, vuv, vn;
	GLuint vao;
	vb = makeBuffer((*model).convert3((*model).vertices), (*model).vertices.size(), sizeof(glm::vec3)); //VBO ze wsp�rz�dnymi wierzcho�k�w
	vuv = makeBuffer((*model).convert2((*model).uvs), (*model).uvs.size(), sizeof(glm::vec2));//VBO z UV
	vn = makeBuffer((*model).convert3((*model).normals), (*model).normals.size(), sizeof(glm::vec3));//VBO z wektorami normalnymi wierzcho�k�w
	
	glGenVertexArrays(1, &vao); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej
	glBindVertexArray(vao); //Uaktywnij nowo utworzony VAO

	assignVBOtoAttribute(shaderProgram, "vertex", vb, 3); //"vertex" odnosi si� do deklaracji "in vec4 vertex;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "vertexUV", vuv, 2); //"vertexUV" odnosi si� do deklaracji "in vec2 vertexUV;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "normal", vn, 3); //"bufNormals" odnosi si� do deklaracji "in vec3 normal;" w vertex shaderze

	(*model).setValue(vao, (*model).objectVao);
	(*model).setValue(vb, (*model).vertexbuffer);
	(*model).setValue(vuv, (*model).vertexUV);
	(*model).setValue(vn, (*model).bufNormals);
}

//Procedura inicjuj�ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, kt�ry nale�y wykona� raz, na pocz�tku programu************

	glClearColor(0, 0, 0.5, 1); //Czy�� ekran na czarno
	glEnable(GL_DEPTH_TEST); //W��cz u�ywanie Z-Bufora

	glfwSetKeyCallback(window, key_callback); //Zarejestruj procedur� obs�ugi klawiatury

	shaderProgram = new ShaderProgram("vshader.txt", NULL, "fshader.txt"); //Wczytaj program cieniuj�cy 

	//Cz�� wczytuj�ca modele i ustawiaj�ca je na miejscach

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

	for (int i = 0; i <= praca->Getmaxlvl(); i++)
	{
		modele = praca->GetModele(i);
		for (int j = 0; j < modele->size(); j++)
		{
			loadObjectVBO(&modele->at(j));
		}
	}

	for (int i = 0; i <= krzeslo->Getmaxlvl(); i++)
	{
		modele = krzeslo->GetModele(i);
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
	}

	loadObjectVBO(skybox);
}

//Zwolnienie zasob�w zaj�tych przez program
void freeOpenGLProgram() {
	delete shaderProgram; //Usuni�cie programu cieniuj�cego
}

//Laduje modele do sciezki nauka
void LoadNauka()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 4;
	
	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - ksiazka
	Model ksiazka("Modele/Nauka1.obj", "Tekstury/Nauka23.png", "Tekstury/Nauka23.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(ksiazka);
	
	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - stol i ksiazka
	Model stol("Modele/Nauka2.obj", "Tekstury/Nauka23.png", "Tekstury/Nauka23Ref.png", 3, vec3(0,0,0), vec3(0,0,0), vec3(1,1,1));
	modele.push_back(stol);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - stol i ksiazka x5 (?)
	Model stolKsiazki("Modele/Nauka3.obj", "Tekstury/Nauka23.png", "Tekstury/Nauka23Ref.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(stolKsiazki);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 4 - stol i komputer
	Model komputer("Modele/Nauka4.obj", "Tekstury/Nauka4.png", "Tekstury/Nauka4Ref.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(komputer);

	poziomy.push_back(modele);
	
	nauka = new Lvl(poziomy, maxlvl, vec3(0,-M_PI/2,0), vec3(3,0,0), vec3(1,1,1));
}

//Laduje modele do sciezki praca
void LoadPraca()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 3;

	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - Reka z banknotem
	Model reka("Modele/Praca1.obj", "Tekstury/Rock.png", "Tekstury/Rock.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(reka);
	//Model banknot("Modele/Banknot.obj", "Tekstury/Dolar.png", "Tekstury/Dolar.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	//modele.push_back(banknot);
	
	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - biuro
	Model biuro("Modele/Praca3.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(biuro);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - biuro2
	Model qwer("Modele/Praca4.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(qwer);

	poziomy.push_back(modele);

	praca = new Lvl(poziomy, maxlvl, vec3(0, M_PI/2, 0), vec3(-5, 0, 2), vec3(1, 1, 1));
}

//Laduje modele do sciezki krzeslo
void LoadKrzeslo()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 3;

	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - taboret
	Model taboret("Modele/Krzeslo1.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(taboret);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - krzeslo
	Model krzeslo1("Modele/Krzeslo2.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.6, 0.6, 0.6));
	modele.push_back(krzeslo1);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - fotel
	Model fotel("Modele/Krzeslo3.obj", "Tekstury/Rock.png", "Tekstury/Rock.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(fotel);

	poziomy.push_back(modele);

	krzeslo = new Lvl(poziomy, maxlvl, vec3(0, 0, 0), vec3(0, 0, -3), vec3(1, 1, 1));
}

//Laduje modele do sciezki alkohol
void LoadAlkohol()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 2;

	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - piwo + barek
	Model barek("Modele/Alkohol1.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(barek);
	
	Model piwo("Modele/Alkohol2.obj", "Tekstury/Alkohol2.png", "Tekstury/Alkohol2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(piwo);

	poziomy.push_back(modele);

	//Poziom 2 - whisky + wodka + piwo + barek
	Model wodka("Modele/Alkohol3.obj", "Tekstury/Alkohol3.png", "Tekstury/Alkohol3.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(wodka);
	
	Model whisky("Modele/Alkohol4.obj", "Tekstury/Alkohol4.png", "Tekstury/Alkohol4.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(whisky);
	
	poziomy.push_back(modele);

	alkohol = new Lvl(poziomy, maxlvl, vec3(0, 0, 0), vec3(-2, 0, -3), vec3(1, 1, 1));
}

//Laduje zawartosc pliku rozwoj.txt do tabeli stany
void LoadStany()
{
	ifstream file;
	file.open("rozwoj.txt", ios::in);
	file >> ilstanow;
	stany = new SStan[ilstanow];
	for (int i = 0; i < ilstanow; i++)
	{
		file >> stany[i].wartosc >> stany[i].wybrany >> stany[i].nr_ruchu >> stany[i].sekwencja >> stany[i].decyzja;
	}

	/*for (int i = 0; i < ilstanow; i++)
	{
		cout << stany[i].wartosc << " " << stany[i].wybrany << " " << stany[i].nr_ruchu << " " << stany[i].sekwencja << " " << stany[i].decyzja << endl;
	}*/

	file.close();
}

//Laduje sciezki, studenta i stany
void LoadLvl()
{
	cout << "Laduje nauka" << endl;
	LoadNauka();
	cout << "Laduje praca" << endl;
	LoadPraca();
	cout << "Laduje krzeslo" << endl;
	LoadKrzeslo();
	cout << "Laduje alkohol" << endl;
	LoadAlkohol();
	cout << "Laduje czlowiek" << endl;
	vec3 rotacja(0, 0, 0);
	rotacja.y = ((-1 * 360 / M_PI *(atan2(eye.z, eye.x)) / 2) + 90) / 180 * M_PI;

	czlowiek = new Czlowiek(rotacja, vec3(0, 0, 0), vec3(0.5, 0.5, 0.5));
	cout << "Laduje skybox" << endl;
	skybox = new Model("Modele/SkyBox.obj", "Tekstury/SkyBox.png", "Tekstury/SkyBox.png", 3, vec3(0,0,0), vec3(0,-1,0), vec3(1,1,1));

	cout << "Laduje stany" << endl;
	LoadStany();
}

//usuwa sciezki, studenta i stany
void DeleteLvl()
{
	delete nauka;
	delete praca;
	delete krzeslo;
	delete alkohol;
	delete czlowiek;
	delete skybox;
	delete[] stany;
}

void drawObject(Model model, ShaderProgram *shaderProgram,mat4 mV, mat4 mP, float rotation_x,float rotation_y, vec3 rotacja, vec3 translacja, vec3 skalowanie) {

	/*TODO LIST
	1. Stworzy� modele i roz�o�y� je odpowiednio na siatki.
	2. Ze stworzonych modeli zrobi� model scenerii (�eby wiedzie� co i gdzie ustawi�)
	3. Animacje powinny by� animacjami obracaj�cymi ca�y obiekt.
	*/

	mat4 modelMatrix = model.M;

	//Transformacje danego modelu 
	modelMatrix = translate(modelMatrix, translacja);

	modelMatrix = rotate(modelMatrix, rotacja.x, vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, rotacja.y, vec3(0, 1, 0));
	modelMatrix = rotate(modelMatrix, rotacja.z, vec3(0, 0, 1));
	modelMatrix = scale(modelMatrix, skalowanie);

	modelMatrix = translate(modelMatrix, model.translacja);
	modelMatrix = rotate(modelMatrix, model.rotacja.x, vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, model.rotacja.y, vec3(0, 1, 0));
	modelMatrix = rotate(modelMatrix, model.rotacja.z, vec3(0, 0, 1));
	modelMatrix = scale(modelMatrix, model.skalowanie);
	

	//Transformacje lvl lub czlowieka
	
	

	//Rotacja wynikajaca z wcisniecia strzalek
	modelMatrix = rotate(modelMatrix, rotation_x, vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, rotation_y, vec3(0, 1, 0));


	shaderProgram->use();

	//1.Aktualizacja uniform�w
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(modelMatrix));
	glUniform4f(shaderProgram->getUniformLocation("lightPos0"), 10, 10, 10, 1); //Przekazanie wsp�rz�dnych �r�d�a �wiat�a do zmiennej jednorodnej lightPos0

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

//Procedura rysuj�ca zawarto�� sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_cam) {
	//************Tutaj umieszczaj kod rysuj�cy obraz******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolor�w

	mat4 P = glm::perspective(50 * 3.14f / 180, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	eye = vec3(eye.x * cos(angle_cam) + eye.z * sin(angle_cam), eye.y, -eye.x * sin(angle_cam) + eye.z * cos(angle_cam));

	mat4 V = glm::lookAt( //Wylicz macierz widoku
		eye,
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	
	vector<Model>* modele; //Modele z danego poziomu lvl

	//Nauka
	modele = nauka->GetAktModele();
	for (std::vector<Model>::iterator it = modele->begin(); it != modele->end(); ++it)
	{
		drawObject(*it, shaderProgram, V, P, angle_x, angle_y, nauka->rotacja, nauka->translacja, nauka->skalowanie);
	}

	//Praca
	modele = praca->GetAktModele();
	for (std::vector<Model>::iterator it = modele->begin(); it != modele->end(); ++it)
	{
		drawObject(*it, shaderProgram, V, P, angle_x, angle_y, praca->rotacja, praca->translacja, praca->skalowanie);
	}

	//Krzeslo
	modele = krzeslo->GetAktModele();
	for (std::vector<Model>::iterator it = modele->begin(); it != modele->end(); ++it)
	{
		drawObject(*it, shaderProgram, V, P, angle_x, angle_y, krzeslo->rotacja, krzeslo->translacja, krzeslo->skalowanie);
	}

	//Alkohol
	modele = alkohol->GetAktModele();
	for (std::vector<Model>::iterator it = modele->begin(); it != modele->end(); ++it)
	{
		drawObject(*it, shaderProgram, V, P, angle_x, angle_y, alkohol->rotacja, alkohol->translacja, alkohol->skalowanie);
	}
	

	//Czlowiek
	if (czlowiek->GetAnimacja() == true && !czlowiek->GetStan())
	{
		czlowiek->ZacznijAnimacje();
	}
	else{
		czlowiek->ZmienStan();
	}

	drawObject(*czlowiek->GetAktModel(), shaderProgram, V, P, angle_x, angle_y, czlowiek->rotacja, czlowiek->translacja, czlowiek->skalowanie);

	drawObject(*skybox, shaderProgram, V, P, 0, 0, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));

	glDisableVertexAttribArray(0);
	//Przerzu� tylny bufor na przedni
	glfwSwapBuffers(window);
}

//Liczy stan gry
int StanGry()
{
	int stan = 0;
	stan += nauka->Getlvl();
	stan += praca->Getlvl() * 8;
	stan += krzeslo->Getlvl() * 32;
	stan += alkohol->Getlvl() * 128;
	return stan;
}

void Decyzja()
{
	bool brak = true;
	for (int i = 0; i < ilstanow; i++)
	{
		if (stangry.wartosc == stany[i].wartosc && stangry.wybrany == stany[i].wybrany &&
			stangry.nr_ruchu == stany[i].nr_ruchu && stangry.sekwencja == stany[i].sekwencja)
		{
			stangry.decyzja = stany[i].decyzja;
			brak = false;

			cout << "******" << endl;
			cout << "Decyzja: " << stany[i].decyzja << endl;
			cout << "Gra Decyzja: " << stangry.decyzja << endl;
			cout << "Wartosc: " << stany[i].wartosc << endl;
			cout << "Wybrany: " << stany[i].wybrany << endl;
			cout << "Nr ruchu: " << stany[i].nr_ruchu << endl;
			cout << "Sekwencja: " << stany[i].sekwencja << endl;
			cout << "******" << endl << endl;
		}
	}

	if (brak)
	{
		cout << endl;
		cout << "Brak stanu gry: " << endl;
		cout << "Wartosc: " << stangry.wartosc << endl;
		cout << "Wybrany: " << stangry.wybrany << endl;
		cout << "Nr ruchu: " << stangry.nr_ruchu << endl;
		cout << "Sekwencja: " << stangry.sekwencja << endl<<endl;
		cout << "Nauka: " << nauka->Getlvl() << endl;
		cout << "Praca: " << praca->Getlvl() << endl;
		cout << "Krzeslo: " << krzeslo->Getlvl() << endl;
		cout << "Alkohol: " << alkohol->Getlvl() << endl;
		cout << "Stan gry (liczony) " << StanGry() << endl;
		exit(EXIT_FAILURE);
	}
}

void NowyRuch()
{
	cout << endl<< "----------Nowy ruch-------------" << endl<<endl;
	stangry.decyzja = KONIEC;
	stangry.nr_ruchu++;
	stangry.sekwencja = 0;
	stangry.wartosc = StanGry();
	stangry.wybrany = BRAK;
}

void IdzDo(int gdzie)
{
	switch (gdzie)
	{
	case START:{
		czlowiek->SetCel(vec3(0,0,0));
	}break;
	
	case NAUKA: {
		czlowiek->SetCel(nauka->translacja);
	}break;

	case PRACA: {
		czlowiek->SetCel(praca->translacja);
	}break;

	case KRZESLO: {
		czlowiek->SetCel(krzeslo->translacja);
	}break;

	case ALKOHOL: {
		czlowiek->SetCel(alkohol->translacja);
	}break;

	default:
	{
		cout << "Nieznany cel!" << endl;
		exit(EXIT_FAILURE);
	}
		break;
	}
}

void Ewoluuj()
{
	if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
	{
		cout << "---------------------------------" << endl << endl;
		cout << "Wartosc: " << stangry.wartosc << endl;
		cout << "Wybrany: " << stangry.wybrany << endl;
		cout << "Nr ruchu: " << stangry.nr_ruchu << endl;
		cout << "Sekwencja: " << stangry.sekwencja << endl << endl;
		cout << "Nauka: " << nauka->Getlvl() << endl;
		cout << "Praca: " << praca->Getlvl() << endl;
		cout << "Krzeslo: " << krzeslo->Getlvl() << endl;
		cout << "Alkohol: " << alkohol->Getlvl() << endl;
		cout << "Stan gry (liczony) " << StanGry() << endl << endl;

		if (stangry.wartosc != StanGry())
		{
			cout << "Stan gry nie jest zgodny!" << endl;
			exit(EXIT_FAILURE);
		}

		if (stangry.sekwencja == 0)
		{
			switch (stangry.wybrany)
			{
			case NAUKA: {
				IdzDo(NAUKA);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					nauka->Inclvl();
					stangry.sekwencja++;
					stangry.wartosc = StanGry();
					IdzDo(START);
				}
			}break;

			case PRACA: {
				IdzDo(PRACA);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					praca->Inclvl();
					stangry.sekwencja++;
					stangry.wartosc = StanGry();
					IdzDo(START);
				}
			}break;

			case KRZESLO: {
				IdzDo(KRZESLO);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					krzeslo->Inclvl();
					stangry.sekwencja++;
					stangry.wartosc = StanGry();
					IdzDo(START);
				}
			}break;

			case ALKOHOL: {
				IdzDo(ALKOHOL);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					alkohol->Inclvl();
					stangry.sekwencja++;
					stangry.wartosc = StanGry();
					IdzDo(START);
				}
			}break;

			default:
			{
				cout << "Zly wybor!" << endl;
				exit(EXIT_FAILURE);
			}
				break;
			}
		}
		else
		{
			Decyzja();
			switch (stangry.decyzja)
			{
			case KONIEC:{
				IdzDo(START);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					NowyRuch();
				}
				
			}break;

			case NAUKA: {
				IdzDo(NAUKA);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					nauka->Inclvl();
					stangry.sekwencja++;
					IdzDo(START);
				}
			}break;

			case PRACA: {
				IdzDo(PRACA);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					praca->Inclvl();
					stangry.sekwencja++;
					IdzDo(START);
				}
			}break;

			case KRZESLO: {
				IdzDo(KRZESLO);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					krzeslo->Inclvl();
					stangry.sekwencja++;
					IdzDo(START);
				}
			}break;

			case ALKOHOL: {
				IdzDo(ALKOHOL);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					alkohol->Inclvl();
					stangry.sekwencja++;
					IdzDo(START);
				}
			}break;

			default:
			{
				cout << "Zla decyzja!" << endl;
				exit(EXIT_FAILURE);
			}
				break;
			}
			stangry.wartosc = StanGry();
		}
		cout << "zerowanie" << endl;
		opoznienie = 0;
	}
}

int main(void)
{
	GLFWwindow* window; //Wska�nik na obiekt reprezentuj�cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedur� obs�ugi b��d�w

	if (!glfwInit()) { //Zainicjuj bibliotek� GLFW
		fprintf(stderr, "Nie mo�na zainicjowa� GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(900, 700, "OpenGL", NULL, NULL);  //Utw�rz okno 500x500 o tytule "OpenGL" i kontekst OpenGL. 

	if (!window) //Je�eli okna nie uda�o si� utworzy�, to zamknij program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	float angle_x = 0; //K�t obrotu kamery w osi x
	float angle_y = 0; //K�t obrotu kamery w osi y
	float angle_cam = 0; //K�t obrotu kamery wok� osi y

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje si� aktywny i polecenia OpenGL b�d� dotyczy� w�a�nie jego.
	glfwSwapInterval(1); //Czekaj na 1 powr�t plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotek� GLEW
		fprintf(stderr, "Nie mo�na zainicjowa� GLEW.\n");
		exit(EXIT_FAILURE);
	}
	
	//Laduje modele poziomow i czlowieka
	LoadLvl();
	
	initOpenGLProgram(window); //Operacje inicjuj�ce

	glfwSetTime(0); //Wyzeruj licznik czasu
	
	//Do jakiej wartosci ma dojsc opoznienie
	int maxop = 100;

					//G��wna p�tla
	while (!glfwWindowShouldClose(window)) //Tak d�ugo jak okno nie powinno zosta� zamkni�te
	{
		if (stangry.wybrany != BRAK && opoznienie == maxop)
		{
			Ewoluuj();
		}

		if (glfwGetTime() > 0.01)
		{
			angle_cam = cameraMove * glfwGetTime();
			angle_x += cameraSpeed_x*glfwGetTime(); //Zwi�ksz k�t o pr�dko�� k�tow� razy czas jaki up�yn�� od poprzedniej klatki
			angle_y += cameraSpeed_y*glfwGetTime(); //Zwi�ksz k�t o pr�dko�� k�tow� razy czas jaki up�yn�� od poprzedniej klatki
			glfwSetTime(0); //Wyzeruj licznik czasu
			/*
			Tu b�d� wykonywane wszystkie modyfikacje macierzy modeli. Pozwoli to na
			wykonywanie animacji na pojedynczych modelach nie ruszaj�c innych. Obs�uga klawiszy musi by�
			blokowana przyk�adow� zmienn� "enable" do czasu zako�czenia animacji. Kod b�dzie brzydki ale dzia�aj�cy ;)
			*/
			czlowiek->Idz();
			drawScene(window, angle_x, angle_y, angle_cam); //Wykonaj procedur� rysuj�c�
			
			
			
			if (opoznienie < maxop)
			{
				opoznienie++;
			}
		}
		glfwPollEvents(); //Wykonaj procedury callback w zalezno�ci od zdarze� jakie zasz�y.
	}

	freeOpenGLProgram();
	DeleteLvl();

	glfwDestroyWindow(window); //Usu� kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zaj�te przez GLFW
	exit(EXIT_SUCCESS);
}

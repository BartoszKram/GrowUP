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
//Swiatlo sloneczne
Slonce *slonce;

//SkyBox
Model *skybox;
Model *grass;

//vector umiejscowienia kamery
vec3 eye = vec3(0.0f, 8.0f, 14.0f);

float cameraMove = 0; // [radiany/s] obrot kamery z czasie gry

				   //Uchwyty na shadery
ShaderProgram *shaderProgram; //WskaŸnik na obiekt reprezentuj¹cy program cieniuj¹cy.

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

struct SStan
{
	int wartosc = 0;
	int wybrany = BRAK;
	int nr_ruchu = 1;
	int sekwencja = 0;
	int decyzja = KONIEC;
}stangry, *stany;

int ilstanow;

bool ifwybrany[5] = { false };

void NowaGra()
{
	stangry.wartosc = 0;
	stangry.wybrany = BRAK;
	stangry.nr_ruchu = 1;
	stangry.sekwencja = 0;
	stangry.decyzja = KONIEC;
	for (int i = 0; i < 5; i++)
	{
		ifwybrany[i] = false;
	}

	nauka->Restart();
	praca->Restart();
	krzeslo->Restart();
	alkohol->Restart();
	czlowiek->Restart();

	cout << endl << endl << endl << "******************NOWA GRA*******************" << endl << endl << endl;
}

//Procedura obs³ugi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) cameraMove = -3.14;
		if (key == GLFW_KEY_RIGHT) cameraMove = 3.14;
		
		if (key == GLFW_KEY_Z) nauka->Inclvl();
		if (key == GLFW_KEY_X) praca->Inclvl();
		if (key == GLFW_KEY_C) krzeslo->Inclvl();
		if (key == GLFW_KEY_V) alkohol->Inclvl();
		
		if (key == GLFW_KEY_N) NowaGra();
		
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
		if (key == GLFW_KEY_LEFT) cameraMove = 0;
		if (key == GLFW_KEY_RIGHT) cameraMove = 0;
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
	slonce = new Slonce();
	//Czêœæ wczytuj¹ca modele i ustawiaj¹ca je na miejscach

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
	loadObjectVBO(grass);
}

//Zwolnienie zasobów zajêtych przez program
void freeOpenGLProgram() {
	delete shaderProgram; //Usuniêcie programu cieniuj¹cego
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
	Model reka("Modele/Praca1.obj", "Tekstury/Praca1.png", "Tekstury/Praca1.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(reka);
	//Model banknot("Modele/Banknot.obj", "Tekstury/Dolar.png", "Tekstury/Dolar.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	//modele.push_back(banknot);
	
	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - biuro
	Model biuro("Modele/Praca4.obj", "Tekstury/Praca4.png", "Tekstury/Praca4.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(biuro);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - biuro2
	Model qwer("Modele/Praca3.obj", "Tekstury/Praca3.png", "Tekstury/Praca3.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
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
	Model taboret("Modele/Krzeslo1.obj", "Tekstury/Krzeslo1.png", "Tekstury/Krzeslo1.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(taboret);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - krzeslo
	Model krzeslo1("Modele/Krzeslo2.obj", "Tekstury/Krzeslo2.png", "Tekstury/Krzeslo2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.6, 0.6, 0.6));
	modele.push_back(krzeslo1);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - fotel
	Model fotel("Modele/Krzeslo3.obj", "Tekstury/Krzeslo3.png", "Tekstury/Krzeslo3.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(fotel);

	poziomy.push_back(modele);

	krzeslo = new Lvl(poziomy, maxlvl, vec3(0, 0, 0), vec3(0, 0, -5), vec3(1, 1, 1));
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
	Model barek("Modele/Alkohol1.obj", "Tekstury/Alkohol1.png", "Tekstury/Alkohol1.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
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

	alkohol = new Lvl(poziomy, maxlvl, vec3(0, 0, 0), vec3(-2, 0, -5), vec3(1, 1, 1));
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
	skybox = new Model("Modele/SkyBox.obj", "Tekstury/SkyBox.png", "Tekstury/SkyBox.png", 3, vec3(0,0,0), vec3(0,0,0), vec3(1,1,1));
	grass = new Model("Modele/Grass.obj", "Tekstury/Grass.png", "Tekstury/Grass.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));

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
	delete grass;
	delete slonce;
	delete[] stany;
}

void drawObject(Model model, ShaderProgram *shaderProgram,mat4 mV, mat4 mP, vec3 rotacja, vec3 translacja, vec3 skalowanie, float jasnosc) {
	/*TODO LIST
	1. Stworzyæ modele i roz³o¿yæ je odpowiednio na siatki.
	2. Ze stworzonych modeli zrobiæ model scenerii (¿eby wiedzieæ co i gdzie ustawiæ)
	3. Animacje powinny byæ animacjami obracaj¹cymi ca³y obiekt.
	*/

	mat4 modelMatrix = model.M;

	//Transformacje lvl lub czlowieka
	modelMatrix = translate(modelMatrix, translacja);

	modelMatrix = rotate(modelMatrix, rotacja.x, vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, rotacja.y, vec3(0, 1, 0));
	modelMatrix = rotate(modelMatrix, rotacja.z, vec3(0, 0, 1));
	modelMatrix = scale(modelMatrix, skalowanie);
	
	//Transformacje danego modelu 
	modelMatrix = translate(modelMatrix, model.translacja);
	modelMatrix = rotate(modelMatrix, model.rotacja.x, vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, model.rotacja.y, vec3(0, 1, 0));
	modelMatrix = rotate(modelMatrix, model.rotacja.z, vec3(0, 0, 1));
	modelMatrix = scale(modelMatrix, model.skalowanie);

	shaderProgram->use();

	//1.Aktualizacja uniformów
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(modelMatrix));
	
	//glUniform4f(shaderProgram->getUniformLocation("lightPos0"), 10, 10, 10, 1); //Przekazanie wspó³rzêdnych Ÿród³a œwiat³a do zmiennej jednorodnej lightPos0
	glUniform4f(shaderProgram->getUniformLocation("lightPos0"), eye.x, eye.y, eye.z, 1); //Przekazanie wspó³rzêdnych Ÿród³a œwiat³a do zmiennej jednorodnej lightPos0

	glUniform1f(shaderProgram->getUniformLocation("relLevel"), model.relLevel);
	glUniform4f(shaderProgram->getUniformLocation("SunPos"), slonce->loc[stangry.nr_ruchu % 4].x, slonce->loc[stangry.nr_ruchu % 4].y, slonce->loc[stangry.nr_ruchu % 4].z, slonce->loc[stangry.nr_ruchu % 4].w);
	glUniform4f(shaderProgram->getUniformLocation("SunCol"), slonce->color[stangry.nr_ruchu % 4].x, slonce->color[stangry.nr_ruchu % 4].y, slonce->color[stangry.nr_ruchu % 4].z, slonce->color[stangry.nr_ruchu % 4].w);
	glUniform4f(shaderProgram->getUniformLocation("Mam"), jasnosc, jasnosc, jasnosc, 1);

	//2.Bindowanie tekstury
	glUniform1i(shaderProgram->getUniformLocation("tex0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.texModelu);
	
	//3.Bindowanie Vao
	glBindVertexArray(model.objectVao);

	//4.Narysowanie obiektu
	
	glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());

	glBindVertexArray(0);
}

//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window, float angle_cam) {
	//************Tutaj umieszczaj kod rysuj¹cy obraz******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów

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
		drawObject(*it, shaderProgram, V, P, nauka->rotacja, nauka->translacja, nauka->skalowanie, nauka->GetJasnosc());
	}

	//Praca
	modele = praca->GetAktModele();
	for (std::vector<Model>::iterator it = modele->begin(); it != modele->end(); ++it)
	{
		drawObject(*it, shaderProgram, V, P, praca->rotacja, praca->translacja, praca->skalowanie, praca->GetJasnosc());
	}

	//Krzeslo
	modele = krzeslo->GetAktModele();
	for (std::vector<Model>::iterator it = modele->begin(); it != modele->end(); ++it)
	{
		drawObject(*it, shaderProgram, V, P, krzeslo->rotacja, krzeslo->translacja, krzeslo->skalowanie, krzeslo->GetJasnosc());
	}

	//Alkohol
	modele = alkohol->GetAktModele();
	for (std::vector<Model>::iterator it = modele->begin(); it != modele->end(); ++it)
	{
		drawObject(*it, shaderProgram, V, P, alkohol->rotacja, alkohol->translacja, alkohol->skalowanie, alkohol->GetJasnosc());
	}
	
	//Czlowiek
	drawObject(*czlowiek->GetAktModel(), shaderProgram, V, P, czlowiek->rotacja, czlowiek->translacja, czlowiek->skalowanie, 0);

	drawObject(*skybox, shaderProgram, V, P, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1), 0);
	drawObject(*grass, shaderProgram, V, P, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1), 0);

	glDisableVertexAttribArray(0);
	//Przerzuæ tylny bufor na przedni
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

void Wynik()
{
	int wynik=0;
	cout << "***********KONIEC GRY!*************" << endl << endl;
	cout << "Twoj wynik:" << endl;
	cout << "Nauka: lvl" << nauka->Getlvl();
	if (nauka->Getlvl() == nauka->Getmaxlvl())
	{
		cout << "(max)";
		wynik++;
	}
	cout << endl;

	cout << "Praca: lvl" << praca->Getlvl();
	if (praca->Getlvl() == praca->Getmaxlvl())
	{
		cout << " (max)";
		wynik++;
	}
	cout << endl;

	cout << "Krzeslo: " << "lvl" << krzeslo->Getlvl();
	if (krzeslo->Getlvl() == krzeslo->Getmaxlvl())
	{
		cout << " (max)";
		wynik++;
	}
	cout << endl;

	cout << "Alkohol: " << "lvl" << alkohol->Getlvl();
	if (alkohol->Getlvl() == alkohol->Getmaxlvl())
	{
		cout << " (max)";
		wynik++;
	}
	cout << endl<<endl;
	if (wynik == 4)
	{
		cout << "Odnalazles prawidlowa sekwencje! Gratulujemy!" << endl;
	}
	else
	{
		cout << "Niestety Twoja sekwencja nie jest najlepsza. Powodzenia nastepnym razem!" << endl;
	}
	cout << endl;
}

void NowyRuch()
{
	cout << endl<< "----------Nowy ruch-------------" << endl<<endl;
	stangry.decyzja = KONIEC;
	stangry.nr_ruchu++;
	stangry.sekwencja = 0;
	stangry.wartosc = StanGry();
	stangry.wybrany = BRAK;
	if (stangry.nr_ruchu > 4)
		Wynik();
}

void IdzDo(int gdzie)
{
	switch (gdzie)
	{
	case START:{
		czlowiek->SetCel(vec3(0,0,0));
	}break;
	
	case NAUKA: {
		vec3 cel = nauka->translacja;
		cel.x -= 1;
		czlowiek->SetCel(cel);
	}break;

	case PRACA: {
		vec3 cel = praca->translacja;
		cel.x += 2.3;
		czlowiek->SetCel(cel);
		//vec3(-5, 0, 2)
	}break;

	case KRZESLO: {
		vec3 cel = krzeslo->translacja;
		cel.z += 1.3;
		czlowiek->SetCel(cel);
		//vec3(0, 0, -5)
	}break;

	case ALKOHOL: {
		vec3 cel = alkohol->translacja;
		cel.z += 1;
		czlowiek->SetCel(cel);
		//vec3(-2, 0, -5)
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

		stangry.wartosc = StanGry();

		if (stangry.sekwencja == 0)
		{
			switch (stangry.wybrany)
			{
			case NAUKA: {
				IdzDo(NAUKA);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					czlowiek->SpojzNa(nauka->translacja);
					nauka->StartEwolucji();
					stangry.sekwencja++;
				}
			}break;

			case PRACA: {
				IdzDo(PRACA);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					czlowiek->SpojzNa(praca->translacja);
					praca->StartEwolucji();
					stangry.sekwencja++;
				}
			}break;

			case KRZESLO: {
				IdzDo(KRZESLO);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					czlowiek->SpojzNa(krzeslo->translacja);
					krzeslo->StartEwolucji();
					stangry.sekwencja++;
				}
			}break;

			case ALKOHOL: {
				IdzDo(ALKOHOL);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					czlowiek->SpojzNa(alkohol->translacja);
					alkohol->StartEwolucji();
					stangry.sekwencja++;
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
					czlowiek->SpojzNa(nauka->translacja);
					nauka->StartEwolucji();
					stangry.sekwencja++;
				}
			}break;

			case PRACA: {
				IdzDo(PRACA);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					czlowiek->SpojzNa(praca->translacja);
					praca->StartEwolucji();
					stangry.sekwencja++;
				}
			}break;

			case KRZESLO: {
				IdzDo(KRZESLO);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					czlowiek->SpojzNa(krzeslo->translacja);
					krzeslo->StartEwolucji();
					stangry.sekwencja++;
				}
			}break;

			case ALKOHOL: {
				IdzDo(ALKOHOL);
				if (czlowiek->GetCel().x == czlowiek->translacja.x && czlowiek->GetCel().z == czlowiek->translacja.z)
				{
					czlowiek->SpojzNa(alkohol->translacja);
					alkohol->StartEwolucji();
					stangry.sekwencja++;
				}
			}break;

			default:
			{
				cout << "Zla decyzja!" << endl;
				exit(EXIT_FAILURE);
			}
				break;
			}
		}
	}
}

bool NastSekw()
{
	if (nauka->Zajety())
		return false;
	if (praca->Zajety())
		return false;
	if (krzeslo->Zajety())
		return false;
	if (alkohol->Zajety())
		return false;
	if (czlowiek->Zajety())
		return false;
	return true;
}

//Jesli ktorys lvl ewoluuje, to zmienia jego jasnosci, na koncu ewolucji SetJasnosc zwraca true -> sygnal, ze czlowiek moze wracac na start
void SetJasnosci()
{
	if (nauka->SetJasnosc() || praca->SetJasnosc() || krzeslo->SetJasnosc() || alkohol->SetJasnosc())
		IdzDo(START);
}

void Instrukcja()
{
	cout << endl << endl << "Witamy w grze Grow Student!" << endl << endl;
	cout << "W pewnym swiecie zyje samotny student. Chcialby jak najlepiej sie rozwinac, ale nie wie, od czego ma zaczac. ";
	cout <<	"Ma przed soba do wyboru cztery rzeczy:" <<endl;
	cout << "-Nauke" << endl;
	cout << "-Prace" << endl;
	cout << "-Krzeslo" << endl;
	cout << "-Alkohol" << endl;
	cout << "Twoim zadaniem jest odnalezienie kolejnosci wyboru tych rzeczy, by jak najlepiej poprowadzic go przez czas studiow" << endl;
	cout << "Powodzenia!" << endl << endl;
	
	cout << "Sterowanie:" << endl;
	cout << " 1 - nauka" << endl;
	cout << " 2 - praca" << endl;
	cout << " 3 - krzeslo" << endl;
	cout << " 4 - alkohol" << endl;
	cout << " N - nowa gra" << endl;
	cout << " <- - kamera w lewo" << endl;
	cout << " -> - kamera w prawo" << endl <<endl;
	
	cout << "Bartosz Kram & Filip Nienartowicz" << endl;
}

int main(void)
{
	GLFWwindow* window; //WskaŸnik na obiekt reprezentuj¹cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurê obs³ugi b³êdów

	if (!glfwInit()) { //Zainicjuj bibliotekê GLFW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(900, 700, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL. 

	if (!window) //Je¿eli okna nie uda³o siê utworzyæ, to zamknij program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	float angle_cam = 0; //K¹t obrotu kamery wokó³ osi y

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje siê aktywny i polecenia OpenGL bêd¹ dotyczyæ w³aœnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekê GLEW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLEW.\n");
		exit(EXIT_FAILURE);
	}
	
	//Laduje modele poziomow i czlowieka
	LoadLvl();
	
	initOpenGLProgram(window); //Operacje inicjuj¹ce

	glfwSetTime(0); //Wyzeruj licznik czasu

	Instrukcja();
					//G³ówna pêtla
	while (!glfwWindowShouldClose(window)) //Tak d³ugo jak okno nie powinno zostaæ zamkniête
	{
		if (stangry.wybrany != BRAK && NastSekw())//opoznienie == maxop)
		{
			Ewoluuj();
		}

		if (glfwGetTime() > 0.025)
		{
			angle_cam = cameraMove * glfwGetTime();
			glfwSetTime(0); //Wyzeruj licznik czasu
			/*
			Tu bêd¹ wykonywane wszystkie modyfikacje macierzy modeli. Pozwoli to na
			wykonywanie animacji na pojedynczych modelach nie ruszaj¹c innych. Obs³uga klawiszy musi byæ
			blokowana przyk³adow¹ zmienn¹ "enable" do czasu zakoñczenia animacji. Kod bêdzie brzydki ale dzia³aj¹cy ;)
			*/
			czlowiek->Idz();
			SetJasnosci();
			drawScene(window, angle_cam); //Wykonaj procedurê rysuj¹c¹
		}
		glfwPollEvents(); //Wykonaj procedury callback w zaleznoœci od zdarzeñ jakie zasz³y.
	}

	freeOpenGLProgram();
	DeleteLvl();

	glfwDestroyWindow(window); //Usuñ kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
	exit(EXIT_SUCCESS);
}

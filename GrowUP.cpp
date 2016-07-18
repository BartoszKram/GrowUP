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
vec3 eye = vec3(0.0f, 5.0f, 5.0f);

float cameraSpeed_x = 0; // [radiany/s] obrot obiektu wokol osi x (pozniej do wywalenia)
float cameraSpeed_y = 0; // [radiany/s] obrot obiektu wokol osi y (pozniej do wywalenia)
float cameraMove = 0; // [radiany/s] obrot kamery z czasie gry

				   //Uchwyty na shadery
ShaderProgram *shaderProgram; //WskaŸnik na obiekt reprezentuj¹cy program cieniuj¹cy.

//Wybor gracza
const int BRAK = 0;
const int NAUKA = 1;
const int PRACA = 2;
const int KRZESLO = 3;
const int ALKOHOL = 4;
const int CZLOWIEK = 5;

int wybrany = BRAK;
bool ifwybrany[5] = { 0 };

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
		if (key == GLFW_KEY_C) krzeslo->Inclvl();
		if (key == GLFW_KEY_V) alkohol->Inclvl();
		if (wybrany == BRAK)
		{
			if (key == GLFW_KEY_1 && !ifwybrany[NAUKA])
			{
				wybrany = NAUKA;
				ifwybrany[NAUKA] = true;
			}
			if (key == GLFW_KEY_2 && !ifwybrany[PRACA]) 
			{
				wybrany = PRACA;
				ifwybrany[PRACA] = true;
			}
			if (key == GLFW_KEY_3 && !ifwybrany[KRZESLO]) 
			{
				wybrany = KRZESLO;
				ifwybrany[KRZESLO] = true;
			}
			if (key == GLFW_KEY_4 && !ifwybrany[ALKOHOL])
			{
				wybrany = ALKOHOL;
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
	
	nauka = new Lvl(poziomy, maxlvl, vec3(0,0,0), vec3(3,0,0), vec3(1,1,1));
}

//Laduje modele do sciezki praca
void LoadPraca()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 4;

	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - Reka z banknotem
	Model reka("Modele/Praca1.obj", "Tekstury/Rock.png", "Tekstury/Rock.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(reka);
	//Model banknot("Modele/Banknot.obj", "Tekstury/Dolar.png", "Tekstury/Dolar.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	//modele.push_back(banknot);
	
	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - praca (mac?)
	Model mc("Modele/Praca2.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(mc);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - biuro
	Model biuro("Modele/Praca3.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(biuro);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 4 - ??
	Model qwer("Modele/Praca4.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(qwer);

	poziomy.push_back(modele);

	praca = new Lvl(poziomy, maxlvl, vec3(0, 0, 0), vec3(-3, 0, 0), vec3(1, 1, 1));
}

//Laduje modele do sciezki krzeslo
void LoadKrzeslo()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 4;

	//Poziom 0 - pusty, brak obiektu
	poziomy.push_back(modele);

	//Poziom 1 - taboret
	Model taboret("Modele/Krzeslo1.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 1, 1));
	modele.push_back(taboret);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 2 - krzeslo
	Model krzeslo1("Modele/Krzeslo2.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(krzeslo1);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - fotel
	Model fotel("Modele/Krzeslo3.obj", "Tekstury/Rock.png", "Tekstury/Rock.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(fotel);

	poziomy.push_back(modele);

	modele.clear();

	//Poziom 3 - fotel
	Model asdf("Modele/Krzeslo4.obj", "Tekstury/Rock.png", "Tekstury/Rock.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));
	modele.push_back(asdf);

	poziomy.push_back(modele);

	krzeslo = new Lvl(poziomy, maxlvl, vec3(0, 0, 0), vec3(0, 0, -3), vec3(1, 1, 1));
}

//Laduje modele do sciezki alkohol
void LoadAlkohol()
{
	vector<vector<Model>> poziomy;
	vector<Model> modele;
	int maxlvl = 3;

	//Poziom 0 - pusty, brak obiektu (barek??)
	Model barek("Modele/Alkohol1.obj", "example2.png", "example2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(barek);

	poziomy.push_back(modele);

	//Poziom 1 - piwo + barek
	Model piwo("Modele/Alkohol2.obj", "Tekstury/Alkohol2.png", "Tekstury/Alkohol2.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(piwo);

	poziomy.push_back(modele);

	//Poziom 2 - wodka + piwo + barek
	Model wodka("Modele/Alkohol3.obj", "Tekstury/Alkohol3.png", "Tekstury/Alkohol3.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(wodka);

	poziomy.push_back(modele);

	//Poziom 3 - whisky + wodka + piwo + barek
	Model whisky("Modele/Alkohol4.obj", "Tekstury/Alkohol4.png", "Tekstury/Alkohol4.png", 3, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0.4, 0.4, 0.4));
	modele.push_back(whisky);

	poziomy.push_back(modele);

	alkohol = new Lvl(poziomy, maxlvl, vec3(0, 0, 0), vec3(-2, 0, -3), vec3(1, 1, 1));
}

//Laduje sciezki i studenta
void LoadLvl()
{
	cout << "nauka" << endl;
	LoadNauka();
	cout << "praca" << endl;
	LoadPraca();
	cout << "krzeslo" << endl;
	LoadKrzeslo();
	cout << "alkohol" << endl;
	LoadAlkohol();
	cout << "czlowiek" << endl;
	vec3 rotacja(0, 0, 0);
	rotacja.y = ((-1 * 360 / M_PI *(atan2(eye.z, eye.x)) / 2) + 90) / 180 * M_PI;

	czlowiek = new Czlowiek(rotacja, vec3(0,1.5,0), vec3(0.35,0.35,0.35));
	cout << "skybox" << endl;
	skybox = new Model("Modele/SkyBox.obj", "Tekstury/SkyBox.png", "Tekstury/SkyBox.png", 3);
}

//usuwa sciezki i ludzika
void DeleteLvl()
{
	delete nauka;
	delete praca;
	delete krzeslo;
	delete alkohol;
	delete czlowiek;
	delete skybox;
}

//Zwolnienie zasobów zajêtych przez program
void freeOpenGLProgram() {
	delete shaderProgram; //Usuniêcie programu cieniuj¹cego
}

void drawObject(Model model, ShaderProgram *shaderProgram,mat4 mV, mat4 mP, float rotation_x,float rotation_y, vec3 rotacja, vec3 translacja, vec3 skalowanie) {

	/*TODO LIST
	1. Stworzyæ modele i roz³o¿yæ je odpowiednio na siatki.
	2. Ze stworzonych modeli zrobiæ model scenerii (¿eby wiedzieæ co i gdzie ustawiæ)
	3. Animacje powinny byæ animacjami obracaj¹cymi ca³y obiekt.
	*/

	mat4 modelMatrix = model.M;
	
	//Transformacje lvl lub czlowieka
	modelMatrix = scale(modelMatrix, skalowanie);
	modelMatrix = translate(modelMatrix, translacja);

	modelMatrix = rotate(modelMatrix, rotacja.x, vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, rotacja.y, vec3(0, 1, 0));
	modelMatrix = rotate(modelMatrix, rotacja.z, vec3(0, 0, 1));

	//Transformacje danego modelu 
	modelMatrix = scale(modelMatrix, model.skalowanie);
	modelMatrix = translate(modelMatrix, model.translacja);
	
	modelMatrix = rotate(modelMatrix, model.rotacja.x, vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, model.rotacja.y, vec3(0, 1, 0));
	modelMatrix = rotate(modelMatrix, model.rotacja.z, vec3(0, 0, 1));

	//Rotacja wynikajaca z wcisniecia strzalek
	modelMatrix = rotate(modelMatrix, rotation_x, vec3(1, 0, 0));
	modelMatrix = rotate(modelMatrix, rotation_y, vec3(0, 1, 0));
	
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

//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_cam) {
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
	//Przerzuæ tylny bufor na przedni
	glfwSwapBuffers(window);
}

int ruch = BRAK;
bool ewoluowal[5];

void Koniec_tury()
{
	//Zerowanie tab ewolucji
	for (int i = 0; i <= 4; i++)
		ewoluowal[i] = false;
	wybrany = BRAK;
}

//Funkcje z *Up zwracaja true, jesli dana sciezka moze ewoluowac
bool NaukaUp()
{
	return false;
}

bool PracaUp()
{
	return false;
}

bool KrzesloUp()
{
	return false;
}

bool AlkoholUp()
{
	return false;
}

bool(*Up[5])() = { NULL, NaukaUp, PracaUp, KrzesloUp, AlkoholUp};

void Ewoluuj()
{
	bool zmiana = false;

	if (ruch == BRAK)
	{
		switch (wybrany)
		{
		case NAUKA:
		{
			nauka->Inclvl();
			ewoluowal[NAUKA] = true;
		}break;

		case PRACA:
		{
			praca->Inclvl();
			ewoluowal[PRACA] = true;
		}break;

		case KRZESLO:
		{
			krzeslo->Inclvl();
			ewoluowal[KRZESLO] = true;
		}break;

		case ALKOHOL:
		{
			alkohol->Inclvl();
			ewoluowal[ALKOHOL] = true;
		}break;

		default:
			cout << "Blad! ruch = BRAK";
			break;
		}
		ruch = wybrany;
		zmiana = true;
	}
	else
	{
		if (ruch == CZLOWIEK)
		{
			if (czlowiek->GetCel() == czlowiek->translacja)
				ruch = BRAK;
			else
				zmiana = true;
		}

		if (ruch!=CZLOWIEK)
		{
			for (int i = 1; i <= 4; i++)
			{
				if (!ewoluowal[i] && Up[i])
				{

					ruch = i;
					zmiana = true;
					break;
				}
				
			}
		}
	}

	/*switch (wybrany)
	{
	case NAUKA:
	{

	}break;

	case PRACA:
	{

	}break;

	case KRZESLO:
	{

	}break;

	case ALKOHOL:
	{

	}break;

	default:
		break;
	}*/

	if (!zmiana)
		Koniec_tury();
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
	
	initOpenGLProgram(window); //Operacje inicjuj¹ce

	//Zerowanie tab ewolucji
	for (int i = 0; i <= 4; i++)
	{
		ewoluowal[i] = false;
	}

	glfwSetTime(0); //Wyzeruj licznik czasu
	
					//G³ówna pêtla
	while (!glfwWindowShouldClose(window)) //Tak d³ugo jak okno nie powinno zostaæ zamkniête
	{
		if (wybrany != BRAK)
		{
			Ewoluuj();
		}

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
			//czlowiek->SetCel(eye);
			czlowiek->Idz();
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

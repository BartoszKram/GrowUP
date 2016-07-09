#pragma once

#include "stdafx.h"
#include "Czlowiek.h"

Czlowiek::Czlowiek()
{
	stan = 0;
	n = 4;
	Model ludzik("Modele/Ludzik.obj", "example2.png", "example2.png", 3);
	Model myModel("RoboDay.obj", "example2.png", "example2.png", 3);
	Model cube("cube.obj", "example2.png", "example2.png", 3);
	Model banknot("Modele/Banknot.obj", "Tekstury/Dolar.png", "Tekstury/Dolar.png", 3);
	Model ksiazka("Modele/Ksiazka.obj", "Tekstury/Ksiazka.png", "Tekstury/Ksiazka.png", 3);
	modele.push_back(ludzik);
	modele.push_back(myModel);
	modele.push_back(cube);
	modele.push_back(banknot);
	modele.push_back(ksiazka);
}

Model* Czlowiek::GetModel(int n)
{
	return &this->modele[n];
}

Model* Czlowiek::GetAktModel()
{
	return &this->modele[this->stan];
}

void Czlowiek::ZmienStan()
{
	if (this->stan != 0)
	{
		this->stan++;
		if (this->stan >= this->n)
		{
			this->stan = 0;
			this->animacja = false;
		}
	}
}

void Czlowiek::ZacznijAnimacje()
{
	if (this->stan == 1)
	{
		this->ZmienStan();
	}
	else
	{
		this->stan = 1;
	}
}

int Czlowiek::Getn()
{
	return this->n;
}

bool Czlowiek::GetAnimacja()
{
	return this->animacja;
}

void Czlowiek::SetAnimacja()
{
	this->animacja = true;
}

bool Czlowiek::GetStan()
{
	return this->stan;
}	
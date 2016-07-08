#pragma once

#include "stdafx.h"
#include "Czlowiek.h"

Czlowiek::Czlowiek()
{
	stan = 0;
	n = 4;
	Model myModel("RoboDay.obj");
	Model cube("cube.obj");
	Model banknot("Modele/Banknot.obj");
	Model ksiazka("Modele/Ksiazka.obj");
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
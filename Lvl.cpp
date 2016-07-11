#pragma once

#include "stdafx.h"

Lvl::Lvl(vector<vector<Model>> modele, int maxlvl)
{
	this->modele = modele;
	this->lvl = 0;
	this->maxlvl = maxlvl;
	this->modele;

	this->rotacja = vec3(0, 0, 0);
	this->translacja = vec3(0, 0, 0);
	this->skalowanie = vec3(1, 1, 1);
}

Lvl::Lvl(vector<vector<Model>> modele, int maxlvl, vec3 rotacja, vec3 translacja, vec3 skalowanie)
	:Lvl(modele, maxlvl)
{
	this->rotacja = rotacja;
	this->translacja = translacja;
	this->skalowanie = skalowanie;
}

vector<Model>* Lvl::GetModele(int n)
{
	return &this->modele[n];
}

vector<Model>* Lvl::GetAktModele()
{
	return &this->modele[this->lvl];
}

int Lvl::Getlvl()
{
	return this->lvl;
}

void Lvl::Inclvl()
{
	if (this->lvl < this->maxlvl)
		this->lvl++;
}

int Lvl::Getmaxlvl()
{
	return this->maxlvl;
}
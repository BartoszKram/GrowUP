#pragma once

#include "stdafx.h"

Lvl::Lvl(vector<vector<Model>> modele, int maxlvl)
{
	this->modele = modele;
	this->lvl = 0;
	this->maxlvl = maxlvl;
	this->modele;
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
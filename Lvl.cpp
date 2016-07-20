#pragma once

#include "stdafx.h"

Lvl::Lvl(vector<vector<Model>> modele, int maxlvl)
{
	this->modele = modele;
	this->lvl = 0;
	this->maxlvl = maxlvl;

	this->rotacja = vec3(0, 0, 0);
	this->translacja = vec3(0, 0, 0);
	this->skalowanie = vec3(1, 1, 1);

	jasnosc = 0;
	ewoluuje = 0;
	jasnoscmod = 0;
	zwlvl = false;
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

void Lvl::StartEwolucji()
{
	if (this->lvl < this->maxlvl)
	{
		this->ewoluuje = 3;
		jasnoscmod = 1;
		zwlvl = true;
	}
	
	if (this->lvl == 0)
		this->ewoluuje = 0;
}

float Lvl::GetJasnosc()
{
	return this->jasnosc;
}

bool Lvl::SetJasnosc()
{
	if (this->jasnoscmod != 0)
	{
		float min = -1, max = 1, krok = 0.1;
		if (ewoluuje == 0 && abs(jasnosc) < krok)
		{
			this->jasnosc = 0;
			this->jasnoscmod = 0;
			if (this->zwlvl)
			{
				this->Inclvl();
				zwlvl = false;
				this->ewoluuje = 1;
				jasnoscmod = 1;
			}
			else
			{
				return true;
			}
		}
		else
		{
			if (this->jasnoscmod > 0)
			{
				if (this->jasnosc < max)
				{
					this->jasnosc += krok;
				}
				else
				{
					jasnoscmod = -1;
				}
			}
			else
			{
				if (this->jasnosc > min)
				{
					this->jasnosc -= krok;
				}
				else
				{
					this->jasnoscmod = 1;
					if (this->ewoluuje > 0)
						this->ewoluuje--;
				}
			}
		}	
	}
	return false;
}

int Lvl::GetEwoluuje()
{
	return this->ewoluuje;
}

bool Lvl::Zajety()
{
	if (this->ewoluuje != 0 || this->jasnoscmod != 0)
		return true;
	return false;
}

void Lvl::Restart()
{
	this->lvl = 0;
	jasnosc = 0;
	ewoluuje = 0;
	jasnoscmod = 0;
	zwlvl = false;
}
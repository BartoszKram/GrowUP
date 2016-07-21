#pragma once

#include "stdafx.h"
#include "Czlowiek.h"

Czlowiek::Czlowiek()
{
	stan = 0;
	
	char nazwa[30] = "Modele/Czlowiek00.obj";
	n = 12;

	for (int i = 1; i <= n; i++)
	{
		nazwa[15] = '0' + i/10;
		nazwa[16] = '0' + i%10;

		Model ludzik(nazwa, "Tekstury/Czlowiek.png", "Tekstury/Czlowiek.png", 3);
		modele.push_back(ludzik);
	}

	this->rotacja = vec3(0, 0, 0);
	this->translacja = vec3(0, 0, 0);
	this->skalowanie = vec3(1, 1, 1);
	this->cel = this->translacja;
	opoznienie = 0;
}

Czlowiek::Czlowiek(vec3 rotacja, vec3 translacja, vec3 skalowanie)
	:Czlowiek()
{
	this->rotacja = rotacja;
	this->translacja = translacja;
	this->skalowanie = skalowanie;
	this->cel = this->translacja;
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
	this->stan++;
	if (this->stan >= this->n)
	{
		this->stan = 0;
	}
}

int Czlowiek::Getn()
{
	return this->n;
}

bool Czlowiek::GetStan()
{
	return this->stan;
}	

void Czlowiek::SetCel(vec3 cel)
{
	this->cel = cel;
}

vec3 Czlowiek::GetCel()
{
	return this->cel;
}

void Czlowiek::Idz()
{
	if (this->GetCel().x != this->translacja.x || this->GetCel().z != this->translacja.z)
	{
		//Dlugosc kroku
		double speed = 0.1;

		//Uklad (0,0) , (x2-x1, z2-z1)
		double x = this->cel.x - this->translacja.x;
		double z = this->cel.z - this->translacja.z;

		//znak x i z
		int xzn = sign(x);
		int zzn = sign(z);

		double odl = sqrt(x*x + z*z);

		if (odl > speed)
		{
			//y z twierdzenia talesa, x z pitagorasa
			vec3 przem;
			przem.y = 0;
			przem.z = z / odl * speed;
			if (zzn * przem.z < speed)
				przem.x = xzn * sqrt(speed * speed - przem.z * przem.z);
			else
				przem.x = 0;

			this->translacja += przem;

			this->rotacja.y = ((-1 * 360 / M_PI *(atan2(this->cel.z - this->translacja.z, this->cel.x - this->translacja.x)) / 2) + 90) / 180 * M_PI;
			this->ZmienStan();
		}
		else
		{
			this->translacja.x = this->cel.x;
			this->translacja.z = this->cel.z;
			if (this->translacja.x == 0 && this->translacja.z == 0)
				SpojzNa(vec3(0, 0, 1));

			if (this->cel == vec3(0, 0, 0))
			{
				opoznienie = 20;
			}
			this->stan = 0;
		}
	}
}

bool Czlowiek::Zajety()
{
	if (this->GetCel().x != this->translacja.x || this->GetCel().z != this->translacja.z)
		return true;
	if (this->opoznienie > 0)
	{
		opoznienie--;
		return true;
	}
	return false;
}

void Czlowiek::SpojzNa(vec3 cel)
{
	if (this->translacja.x != cel.x || this->translacja.z != cel.z)
		this->rotacja.y = ((-1 * 360 / M_PI *(atan2(cel.z - this->translacja.z, cel.x - this->translacja.x)) / 2) + 90) / 180 * M_PI;
}

void Czlowiek::Restart()
{
	stan = 0;
	n = 4;
	this->translacja = vec3(0, 0, 0);
	SpojzNa(vec3(0, 0, 1));
	this->cel = this->translacja;
	opoznienie = 0;
}
#pragma once

#include "stdafx.h"
#include "Czlowiek.h"

Czlowiek::Czlowiek()
{
	stan = 0;
	n = 4;
	Model ludzik("Modele/Ludzik.obj", "Tekstury/Czlowiek.png", "Tekstury/Czlowiek.png", 3);
	modele.push_back(ludzik);
	modele.push_back(ludzik);
	modele.push_back(ludzik);
	modele.push_back(ludzik);
	this->rotacja = vec3(0, 0, 0);
	this->translacja = vec3(0, 0, 0);
	this->skalowanie = vec3(1, 1, 1);
	this->cel = this->translacja;
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
		}
		else
		{
			this->translacja.x = this->cel.x;
			this->translacja.z = this->cel.z;
			if (this->translacja.x == 0 && this->translacja.z == 0)
				this->rotacja.y = ((-1 * 360 / M_PI *(atan2(1 - this->translacja.z, 0 - this->translacja.x)) / 2) + 90) / 180 * M_PI;
		}
	}
}
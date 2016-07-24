
#include "stdafx.h"
#include "Slonce.h"

namespace Slonca {
	Slonce::Slonce() {
		this->loc.push_back(vec4(0.0f, 2.0f, 1.0f, 1.0f));
		this->loc.push_back(vec4(0.0f, 4.0f, 1.0f, 1.0f));
		this->loc.push_back(vec4(0.0f, 6.0f, 1.0f, 1.0f));
		this->loc.push_back(vec4(0.0f, 8.0f, 1.0f, 1.0f));
		this->loc.push_back(vec4(0.0f, 10.0f, 1.0f, 1.0f));

		this->color.push_back(vec4(1.0f, 1.0f, 0.0f, 1.0f));
		this->color.push_back(vec4(1.0f, 0.9f, 0.0f, 1.0f));
		this->color.push_back(vec4(1.0f, 0.77f, 0.0f, 1.0f));
		this->color.push_back(vec4(1.0f, 0.55f, 0.0f, 1.0f));
		this->color.push_back(vec4(1.0f, 0.32f, 0.0f, 1.0f));
	}
}
#pragma once

#include "Application.h"

namespace App {

	class Game : public Application
	{
	public:
		Game(sf::RenderWindow* renderWindow, const std::string& title);
		virtual ~Game() = default;

		virtual void Update(sf::Time ts) override;
		virtual void OnEvent(sf::Event& event) override;
	private:
		sf::RenderWindow* window = nullptr;

		sf::CircleShape shape;
	};

}


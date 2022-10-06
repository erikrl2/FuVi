#pragma once

#include "Application.h"

namespace App {

	class Visualizer : public Application
	{
	public:
		Visualizer(sf::RenderWindow* renderWindow);

		virtual void Update(sf::Time ts) override;
		virtual void OnEvent(sf::Event& event) override;

		void Draw();
	private:
		sf::RenderWindow* window = nullptr;
		sf::Vector2u size{ 1280, 720 };

		sf::Texture texture;
		sf::Sprite sprite;
	};

}


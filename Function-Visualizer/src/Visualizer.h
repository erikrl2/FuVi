#pragma once

#include "Application.h"

#include <vector>

namespace App {

	struct FunctionData
	{
		sf::Color Color;
		float (*Function)(float);

		FunctionData(sf::Color color, float (*func)(float))
			: Color{ color }, Function{ func } {}
	};

	class Visualizer : public Application
	{
	public:
		Visualizer(sf::RenderWindow* renderWindow);

		virtual void Update(float ts) override;
		virtual void OnEvent(sf::Event& event) override;

		void Draw();
	private:
		sf::RenderWindow* window = nullptr;
		sf::Vector2u windowSize{ 1280, 720 };

		sf::Image image;
		sf::Texture texture;
		sf::Sprite sprite;

		std::vector<FunctionData> functions;
	};

}


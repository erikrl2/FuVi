#pragma once

#include "Application.h"

#include <vector>

namespace App {

	constexpr uint32_t Width = 1280;
	constexpr uint32_t Height = 720;

	struct FunctionData
	{
		sf::Color Color;
		float (*Function)(float);
		sf::Vertex vertices[Width]{};

		FunctionData(sf::Color color, float (*func)(float))
			: Color{ color }, Function{ func } {}
	};

	class Visualizer : public Application
	{
	public:
		Visualizer(sf::RenderWindow* renderWindow);

		virtual void Update(float ts) override;
		virtual void OnEvent(sf::Event& event) override;
	private:
		sf::RenderWindow* window = nullptr;

		std::vector<FunctionData> functions;
	};

}


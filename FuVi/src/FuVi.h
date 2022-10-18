#pragma once

#include "Application.h"

#include <exprtk.hpp>

#include <vector>

namespace App {

	struct FunctionData
	{
		sf::Color Color{ -1 };
		char Buffer[32]{};
		exprtk::expression<float> Expression{};
		std::unique_ptr<float> X{ std::make_unique<float>() };
		sf::VertexArray Vertices{ sf::LineStrip };
	};

	class Visualizer : public Application
	{
	public:
		Visualizer(sf::RenderWindow* renderWindow);
		~Visualizer() override;

		void Update(sf::Time ts) override;
		void Draw() override;
		void OnEvent(sf::Event& event) override;
	private:
		void UpdateImGui(sf::Time ts);
		void UpdateGraphOffset();
		void UpdateFunctions();
		void UpdateGrid();

		float GetGridCellSize();
	private:
		sf::RenderWindow* window;

		int width = 1280;
		int height = 720;

		const float baseUnit = 80;
		float pixelsPerUnit = baseUnit;

		sf::Vector2i graphOffset;
		bool canDragGraph = false;

		std::vector<FunctionData> functions;

		sf::VertexArray grid{ sf::Lines };

		sf::Font font;
		std::vector<sf::Text> gridNumbers;
	};

}


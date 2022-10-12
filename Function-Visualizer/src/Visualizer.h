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
		std::unique_ptr<float> X = std::make_unique<float>();
		sf::VertexArray Vertices{ sf::LineStrip };
	};

	class Visualizer : public Application
	{
	public:
		Visualizer(sf::RenderWindow* renderWindow);
		~Visualizer() override;

		void Update(sf::Time ts) override;
		void UpdateImGui(sf::Time ts);
		void UpdateGraphOffset();

		void Draw() override;

		void OnEvent(sf::Event& event) override;
	private:
		sf::RenderWindow* window = nullptr;

		int width = 1280;
		int height = 720;

		std::vector<FunctionData> functions;
		float pixelsPerUnit = 40;

		sf::Vector2i graphOffset;
		bool canDragGraph = true;

		sf::Vertex grid[4];
	};

}


#pragma once

#include "Application.h"
#include "pch.h"

namespace App {

	constexpr uint32_t Width = 1280;
	constexpr uint32_t Height = 720;

	struct FunctionData
	{
		sf::Color Color{ -1 };
		char Buffer[32]{};
		float X{};
		exprtk::expression<float> Expression;
		sf::Vertex Vertices[Width]{};

		float Function(float x)
		{
			X = x;
			return Expression.value();
		}
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

		std::vector<std::shared_ptr<FunctionData>> functions;
		float pixelsPerUnit = 40;

		sf::Vector2i graphOffset;
		bool canDragGraph = true;

		sf::Vertex grid[4];
	};

}


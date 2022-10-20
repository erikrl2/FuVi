#pragma once

#include "Application.h"
#include "Grid.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>

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

	class FuVi : public Application
	{
	public:
		FuVi(sf::RenderWindow* renderWindow);
		~FuVi() override;

		void Update(sf::Time ts) override;
		void Draw() override;
		void OnEvent(sf::Event& event) override;
	private:
		void UpdateImGui(sf::Time ts);
		void UpdateFunctions();
	private:
		sf::RenderWindow* window;

		int width = 1280;
		int height = 720;

		Grid grid;
		std::vector<FunctionData> functions;
	};

}


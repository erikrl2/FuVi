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

	struct GridNumber
	{
		sf::Text text;

		GridNumber(double number, int precision, const sf::Font& font)
			: text("", font, 14)
		{
			char format[] = "%.0f";
			format[2] = '0' + std::max(0, std::min(7, precision));
			char numberString[32]{};
			sprintf_s(numberString, sizeof(numberString), format, number);
			text.setString(numberString);

			textWidth = strnlen_s(numberString, sizeof(numberString)) * 8.f;
		}

		void SetPositionWithinBounds(sf::Vector2f pos, const sf::FloatRect& bounds, bool isYAxis = true)
		{
			float x = isYAxis ? std::max(bounds.left, std::min(bounds.width - textWidth, pos.x - textWidth)) : pos.x - textWidth / 2;
			float y = !isYAxis ? std::max(bounds.top, std::min(bounds.height - 16, pos.y)) : pos.y - 8;
			text.setPosition(x, y);
		}

		operator sf::Text() { return text; }
	private:
		float textWidth{};
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
		double zoomFactor = 1;

		sf::Vector2i graphOffset;
		bool canDragGraph = false;

		sf::VertexArray gridLines{ sf::Lines };
		std::vector<sf::Text> gridNumbers;

		std::vector<FunctionData> functions;

		sf::Font font;
	};

}


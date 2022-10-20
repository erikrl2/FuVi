#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

namespace App {

	class GridNumber
	{
	public:
		GridNumber(double number, int precision, const sf::Font& font);
		void SetPositionWithinBounds(sf::Vector2f pos, const sf::FloatRect& bounds, bool isYAxis = true);
		operator sf::Text() { return text; }
	private:
		sf::Text text;
		float textWidth{};
	};

	class Grid
	{
	public:
		Grid(sf::RenderWindow* window);

		void Update();
		void OnEvent(sf::Event& event);
		void Draw();
	private:
		void UpdateGraphOffset();
		float GetGridCellSize();
	private:
		sf::RenderWindow* window;
		float width{}, height{};

		sf::VertexArray lines{ sf::Lines };
		std::vector<sf::Text> numbers;
		sf::Font font;

		static constexpr float baseUnit = 80;
		float pixelsPerUnit = baseUnit;
		double zoomFactor = 1;

		sf::Vector2i offset;

		bool canDragGraph = false;

		friend class FuVi;
	};

}

#include "Visualizer.h"

#include <iostream>

// TODO: Remove unused static libs

namespace App {

	Visualizer::Visualizer(sf::RenderWindow* renderWindow)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(Width, Height), "Function Visualizer",
			sf::Style::Default, sf::ContextSettings(0, 0, 4));
		window->setVerticalSyncEnabled(true);

		// TODO: Take functions by user input
		functions.emplace_back(sf::Color::Red, [](float x) { return x; });
		functions.emplace_back(sf::Color::Red, [](float x) { return x * x; });
		functions.emplace_back(sf::Color::Blue, [](float x) { return x * x * x; });
		functions.emplace_back(sf::Color::Green, [](float x) { return std::sinf(x); });
		functions.emplace_back(sf::Color::Magenta, [](float x) { return std::cosf(x); });
	}

	static bool draw = true;

	void Visualizer::Update(float ts)
	{
		if (!draw)
			return;

		window->clear();

		constexpr float pixelsPerUnit = 40;

		for (auto& f : functions)
		{
			for (int drawX = 0; drawX < Width; drawX++)
			{
				float x = (drawX - Width / 2.f) / pixelsPerUnit;
				float y = f.Function(x);

				float drawY = Height - (y * pixelsPerUnit + Height / 2.f);

				f.vertices[drawX] = { {(float)drawX, drawY}, f.Color };
			}

			window->draw(f.vertices, Width, sf::LineStrip);
		}

		window->display();

		draw = false;
	}

	void Visualizer::OnEvent(sf::Event& event)
	{
		switch (event.type)
		{
		case sf::Event::Closed:
		{
			window->close();
			break;
		}
		case sf::Event::Resized:
		{
			draw = true;
			break;
		}
		}
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Visualizer>();
	return 0;
}

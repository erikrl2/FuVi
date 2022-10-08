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

		// DEBUG
		functions.emplace_back(sf::Color::Red, [](float x) { return x * x; });
	}

	static bool draw = true;

	void Visualizer::Update(float ts)
	{
		if (!draw)
			return;

		window->clear();

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
		case sf::Event::MouseWheelScrolled:
		{
			static int zoom = 39;
			zoom += (int)event.mouseWheelScroll.delta;
			pixelsPerUnit = (float)std::pow(1.1f, zoom);
			break;
		}
		case sf::Event::Closed:
		{
			window->close();
			break;
		}
		}

		if (event.type & (sf::Event::Resized | sf::Event::MouseWheelScrolled))
			draw = true;
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Visualizer>();
	return 0;
}

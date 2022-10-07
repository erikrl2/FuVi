#include "Visualizer.h"

#include <iostream>

// TODO: Remove unused static libs

namespace App {

	Visualizer::Visualizer(sf::RenderWindow* renderWindow)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(windowSize.x, windowSize.y), "Graph-Visualizer",
			sf::Style::Default, sf::ContextSettings(0, 0, 4));
		window->setVerticalSyncEnabled(true);

		// TODO: Switch to using vertices instead of image/texture/sprite
		image.create(windowSize.x, windowSize.y, sf::Color::White);
		texture.loadFromImage(image);
		sprite.setTexture(texture, true);

		functions.emplace_back(sf::Color::Red, [](float x) { return x; });
		functions.emplace_back(sf::Color::Red, [](float x) { return x * x; });
		functions.emplace_back(sf::Color::Blue, [](float x) { return x * x * x; });
		functions.emplace_back(sf::Color::Green, [](float x) { return std::sinf(x); });
		functions.emplace_back(sf::Color::Magenta, [](float x) { return std::cosf(x); });
	}

	void Visualizer::Update(float ts)
	{
		constexpr float pixelsPerUnit = 40;

		// TODO: Only recreate on change
		image.create(image.getSize().x, image.getSize().y, sf::Color::White);

		for (auto& f : functions)
		{
			for (uint32_t drawX = 0; drawX < windowSize.x; drawX++)
			{
				float x = (drawX - windowSize.x / 2.f) / pixelsPerUnit;
				float y = f.Function(x);

				uint32_t drawY = (uint32_t)(windowSize.y - (y * pixelsPerUnit + windowSize.y / 2.f));

				if (drawY < 0 || drawY >= windowSize.y)
					continue;

				image.setPixel(drawX, drawY, f.Color);
			}
		}

		texture.update(image);

		Draw();
	}

	void Visualizer::Draw()
	{
		window->clear();
		window->draw(sprite);
		window->display();
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
		}
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Visualizer>();
	return 0;
}

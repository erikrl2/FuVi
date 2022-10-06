#include "Game.h"

namespace App {

	Visualizer::Visualizer(sf::RenderWindow* renderWindow)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(size.x, size.y), "Graph-Visualizer");
		window->setVerticalSyncEnabled(true);

		sf::Image image;
		image.create(size.x, size.y);
		texture.loadFromImage(image);
		sprite.setTexture(texture, true);
	}

	void Visualizer::Update(sf::Time ts)
	{
		// TODO: Replace image with vertices?

		sf::Image image;
		image.create(size.x, size.y);

		for (uint32_t x = 0; x < size.x; x++)
		{
			uint32_t y = (uint32_t)(x * x);
			if (y < size.y)
			{
				image.setPixel(x, y, sf::Color::White);
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
		if (event.type == sf::Event::Closed)
			window->close();
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Visualizer>();
	return 0;
}

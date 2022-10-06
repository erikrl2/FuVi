#include "Game.h"

namespace App {

	Game::Game(sf::RenderWindow* renderWindow, const std::string& title)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(200, 200), title);

		shape.setRadius(100);
		shape.setFillColor(sf::Color::Green);
	}

	void Game::Update(sf::Time ts)
	{
		window->clear();
		window->draw(shape);
		window->display();
	}

	void Game::OnEvent(sf::Event& event)
	{
		if (event.type == sf::Event::Closed)
			window->close();
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Game>(argv[0]);

	return 0;
}

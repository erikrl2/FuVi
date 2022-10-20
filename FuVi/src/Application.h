#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

class Application
{
public:
	virtual ~Application() = default;

	virtual void Update(sf::Time ts) = 0;
	virtual void Draw() = 0;
	virtual void OnEvent(sf::Event& event) = 0;
public:
	template<class App, class... Args>
	static void Launch(Args... args)
	{
		static_assert(std::is_base_of<Application, App>::value, "App is not a subclass of 'Application'");

		sf::RenderWindow window;

		App app(&window, std::forward<Args>(args)...);

		sf::Clock clock;

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
				app.OnEvent(event);

			app.Update(clock.restart());
			app.Draw();
		}
	}
};

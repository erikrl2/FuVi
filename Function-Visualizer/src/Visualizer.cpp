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

		if (font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
			inputText.setFont(font);
		else
			std::cout << "Font could not be loaded\n";
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

		window->draw(inputText);

		window->display();

		draw = false;
	}

	void Visualizer::HandleInput()
	{
		// TODO: Display function and text in same color
		// TODO: Ability to display multiple functions at once

		if (input.find('x') != input.npos)
		{
			std::string astr = input.substr(0, input.find_first_of('x'));
			float af = 0;

			if (astr.empty())
				af = 1;
			else if (Utils::IsDigit(astr))
				af = std::stof(astr);
			else
				return;

			functions.emplace_back(sf::Color::White, [=](float x) { return af * x; });
		}
		else
			functions.clear();
	}

	void Visualizer::OnEvent(sf::Event& event)
	{
		switch (event.type)
		{
		case sf::Event::TextEntered:
		{
			auto character = event.text.unicode;
			if (character == 8) // 8 = Backspace
			{
				if (!input.empty())
					input.erase(input.size() - 1);
			}
			else if (character == 27) // 27 = Esc
			{
				if (!input.empty())
					input.clear();
			}
			else if (character < 128 && input.size() < 32)
				input += character;
			else
				break;

			inputText.setString(input);

			HandleInput();
			break;
		}
		case sf::Event::Closed:
		{
			window->close();
			break;
		}
		}

		if (event.type == sf::Event::TextEntered ||
			event.type == sf::Event::Resized)
			draw = true;
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Visualizer>();
	return 0;
}

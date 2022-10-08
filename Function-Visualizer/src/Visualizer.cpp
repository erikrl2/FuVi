#include "Visualizer.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <iostream>

// TODO: Remove unused static libs

namespace App {

	Visualizer::Visualizer(sf::RenderWindow* renderWindow)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(Width, Height), "Function Visualizer",
			sf::Style::Default, sf::ContextSettings(0, 0, 4));
		window->setVerticalSyncEnabled(true);

		ImGui::SFML::Init(*window);
		ImGui::GetIO().IniFilename = nullptr;

		functions.emplace_back(sf::Color::Red, [](float x) { return x * x; });
	}

	Visualizer::~Visualizer()
	{
		ImGui::SFML::Shutdown(*window);
	}

	static bool recalculate = true;

	void Visualizer::Update(sf::Time ts)
	{
		ImGui::SFML::Update(*window, ts);

		UpdateImGui(ts);

		if (!recalculate)
			return;

		for (auto& f : functions)
		{
			for (int drawX = 0; drawX < Width; drawX++)
			{
				float x = (drawX - Width / 2.f) / pixelsPerUnit;
				float y = f.Function(x);

				float drawY = Height - (y * pixelsPerUnit + Height / 2.f);

				f.vertices[drawX] = { {(float)drawX, drawY}, f.Color };
			}
		}

		recalculate = false;
	}

	void Visualizer::UpdateImGui(sf::Time ts)
	{
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ 200, (float)window->getSize().y });
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGui::Begin("Functions", 0, flags);

		static char fbuf[32]{};
		ImGui::InputText("f(x)", fbuf, sizeof(fbuf));

		ImGui::End();

		// DEBUG
		ImGui::SetNextWindowPos({ window->getSize().x - 100.f, 50 });
		ImGui::Begin("Debug");
		ImGui::Text("%.0f FPS", 1 / ts.asSeconds());
		ImGui::End();
	}

	void Visualizer::Draw()
	{
		window->clear();

		for (auto& f : functions)
			window->draw(f.vertices, Width, sf::LinesStrip);

		ImGui::SFML::Render(*window);

		window->display();
	}

	void Visualizer::OnEvent(sf::Event& event)
	{
		ImGui::SFML::ProcessEvent(event);

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
			recalculate = true;
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Visualizer>();
	return 0;
}

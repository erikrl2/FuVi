#include "Visualizer.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <iostream>

// TODO: Remove unused static libs

#define FOFX(function) [](float x) -> float { return function; }

namespace App {

	Visualizer::Visualizer(sf::RenderWindow* renderWindow)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(Width, Height), "Function Visualizer",
			sf::Style::Default, sf::ContextSettings(0, 0, 4));
		window->setVerticalSyncEnabled(true);

		ImGui::SFML::Init(*window);
		ImGui::GetIO().IniFilename = nullptr;

		// DEBUG
		functions.emplace_back(FOFX(x * x), sf::Color::Red);
		functions.emplace_back(FOFX(x * x * x), sf::Color::Green);
		functions.emplace_back(FOFX(sinf(x)), sf::Color::Blue);
	}

	Visualizer::~Visualizer()
	{
		ImGui::SFML::Shutdown(*window);
	}

	//static bool recalculate = true;

	void Visualizer::Update(sf::Time ts)
	{
		ImGui::SFML::Update(*window, ts);

		UpdateImGui(ts);

		//if (!recalculate)
		//	return;

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

		//recalculate = false;
	}

	void Visualizer::UpdateImGui(sf::Time ts)
	{
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ 330, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { .5f, .5f });
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]);
		ImGui::Begin("Functions", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		int indexToRemove = -1;
		for (int i = 0; i < functions.size(); i++)
		{
			auto& f = functions[i];
			ImGui::PushID(i);

			ImVec4 col = f.Color;
			if (ImGui::ColorEdit3("##color", &col.x, ImGuiColorEditFlags_NoInputs))
				f.Color = col;

			ImGui::SameLine();

			if (ImGui::InputText("##input", f.Buffer, sizeof(f.Buffer)))
			{
				// TODO: Interprete buffer
				// If buffer contains valid function, update f.Function
			}

			ImGui::SameLine();

			if (ImGui::Button("Del"))
				indexToRemove = i;

			ImGui::PopID();
		}
		if (indexToRemove != -1)
			functions.erase(functions.begin() + indexToRemove);

		DisplayNewPrompt();

		ImGui::End();

		// DEBUG
		ImGui::SetNextWindowPos({ window->getSize().x - 100.f, 30 });
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

		//if (event.type == sf::Event::Resized ||
		//	event.type == sf::Event::MouseWheelScrolled)
		//	recalculate = true;
	}

	void Visualizer::DisplayNewPrompt()
	{
		static ImVec4 col{ 1, 1, 1, 1 };
		ImGui::ColorEdit3("##Color", &col.x, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		static char buf[32]{};
		if (ImGui::InputText("##f(x)", buf, sizeof(buf)))
		{
			// TODO: Interprete buffer
			// If buffer contains valid function, push back function
		}
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Visualizer>();
	return 0;
}

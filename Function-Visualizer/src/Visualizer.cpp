#include "pch.h"

#include "Visualizer.h"

#include <iostream>

// TODO: Remove unused static libs

namespace App {

	Visualizer::Visualizer(sf::RenderWindow* renderWindow)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(Width, Height), "Function Visualizer",
			sf::Style::Default, sf::ContextSettings(0, 0, 4));
#if NDEBUG
		window->setVerticalSyncEnabled(true);
#endif

		ImGui::SFML::Init(*window);
		ImGui::GetIO().IniFilename = nullptr;

		sf::Color gridColor(255, 255, 255, 63);
		grid[0] = { sf::Vertex{{0, Height / 2.f}, gridColor} };
		grid[1] = { sf::Vertex{{Width, Height / 2.f}, gridColor} };
		grid[2] = { sf::Vertex{{Width / 2.f, 0}, gridColor} };
		grid[3] = { sf::Vertex{{Width / 2.f, Height}, gridColor} };
	}

	Visualizer::~Visualizer()
	{
		ImGui::SFML::Shutdown(*window);
	}

	static bool redraw = false;

	void Visualizer::Update(sf::Time ts)
	{
		ImGui::SFML::Update(*window, ts);

		UpdateImGui(ts);

		if (!redraw)
			return;
		redraw = false;

		// TODO: Move graph with mouse

		for (auto& fData : functions)
		{
			for (int drawX = 0; drawX < Width; drawX++)
			{
				float x = (drawX - Width / 2.f) / pixelsPerUnit;
				float y = fData->Function(x);

				float drawY = Height - (y * pixelsPerUnit + Height / 2.f);

				fData->Vertices[drawX] = { {(float)drawX, drawY}, fData->Color };
			}
		}
	}

	void Visualizer::UpdateImGui(sf::Time ts)
	{
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ 300, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { .5f, .5f });
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]);
		ImGui::Begin("Functions", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		int indexToRemove = -1;
		for (int i = 0; i < functions.size(); i++)
		{
			auto& fData = functions[i];
			ImGui::PushID(i);

			ImVec4 col = fData->Color;
			if (ImGui::ColorEdit3("##color", &col.x, ImGuiColorEditFlags_NoInputs))
			{
				fData->Color = col;
				redraw = true;
			}

			ImGui::SameLine();

			if (ImGui::InputText("##input", fData->Buffer, sizeof(fData->Buffer)))
			{
				exprtk::parser<float> parser;
				parser.compile(fData->Buffer, fData->Expression);
				redraw = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Del"))
				indexToRemove = i;

			ImGui::PopID();
		}
		if (indexToRemove != -1)
		{
			functions.erase(functions.begin() + indexToRemove);
			redraw = true;
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
		ImGui::Text("Add new:");
		ImGui::SameLine();

		static char exprCString[32]{};
		bool enter = false;

		ImGui::SetNextItemWidth(120);
		if (ImGui::InputText("##f(x)", exprCString, sizeof(exprCString), ImGuiInputTextFlags_EnterReturnsTrue))
			enter = true;

		ImGui::SameLine();

		if (ImGui::Button("Enter"))
			enter = true;

		if (enter)
		{
			exprtk::symbol_table<float> symbolTable;
			exprtk::expression<float> expression;
			exprtk::parser<float> parser;

			std::shared_ptr<FunctionData> fData = std::make_shared<FunctionData>();

			symbolTable.add_variable("x", fData->X);
			symbolTable.add_constants();
			expression.register_symbol_table(symbolTable);
			if (parser.compile(exprCString, expression))
			{
				fData->Expression = expression;

				strncpy_s(fData->Buffer, sizeof(fData->Buffer), exprCString, strlen(exprCString));
				memset(exprCString, 0, sizeof(exprCString));

				functions.push_back(fData);
			}

			redraw = true;
		}

		ImGui::End();

#ifdef DEBUG
		ImGui::SetNextWindowPos({ window->getSize().x - 100.f, 30 });
		ImGui::Begin("Debug", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);
		ImGui::Text("%.0f FPS", 1 / ts.asSeconds());
		ImGui::End();
#endif
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

		if (event.type == sf::Event::Resized ||
			event.type == sf::Event::MouseWheelScrolled)
			redraw = true;
	}

	void Visualizer::Draw()
	{
		window->clear();

		window->draw(grid, 4, sf::Lines);

		for (auto& fData : functions)
			window->draw(fData->Vertices, Width, sf::LinesStrip);

		ImGui::SFML::Render(*window);

		window->display();
	}

}

int main(int argc, char** argv)
{
	Application::Launch<App::Visualizer>();
	return 0;
}

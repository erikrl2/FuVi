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

		for (int i = 0; i < 4; i++)
			grid[i].color = { 255, 255, 255, 63 };
	}

	Visualizer::~Visualizer()
	{
		ImGui::SFML::Shutdown(*window);
	}

	void Visualizer::Update(sf::Time ts)
	{
		ImGui::SFML::Update(*window, ts);

		UpdateImGui(ts);
		UpdateGraphOffset();

		for (auto& fData : functions)
		{
			for (int drawX = 0; drawX < Width; drawX++)
			{
				float x = (drawX - Width / 2.f) / pixelsPerUnit;
				x -= graphOffset.x / pixelsPerUnit;

				float y = fData->Function(x);
				y -= graphOffset.y / pixelsPerUnit;

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
			}

			ImGui::SameLine();

			if (ImGui::InputText("##input", fData->Buffer, sizeof(fData->Buffer)))
			{
				exprtk::parser<float> parser;
				parser.compile(fData->Buffer, fData->Expression);
			}

			ImGui::SameLine();

			if (ImGui::Button("Del"))
				indexToRemove = i;

			ImGui::PopID();
		}
		if (indexToRemove != -1)
			functions.erase(functions.begin() + indexToRemove);

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
		}

		canDragGraph = !ImGui::IsWindowHovered();

		ImGui::End();

#ifdef DEBUG
		ImGui::SetNextWindowPos({ 10, window->getSize().y - 100.f });
		ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoDecoration);
		ImGui::Text("%.0f FPS", 1 / ts.asSeconds());
		ImGui::End();
#endif
	}

	void Visualizer::UpdateGraphOffset()
	{
		static bool dragging = false;
		static sf::Vector2i lastPos;

		if (canDragGraph && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
			if (dragging) graphOffset += mousePos - lastPos;
			lastPos = mousePos;
			dragging = true;
		}
		else
		{
			dragging = false;
		}

		grid[0].position = { 0, Height / 2.f + graphOffset.y };
		grid[1].position = { Width, Height / 2.f + graphOffset.y };
		grid[2].position = { Width / 2.f + graphOffset.x, 0 };
		grid[3].position = { Width / 2.f + graphOffset.x, Height };
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

	void Visualizer::OnEvent(sf::Event& event)
	{
		ImGui::SFML::ProcessEvent(event);

		switch (event.type)
		{
		case sf::Event::MouseWheelScrolled:
		{
			static float zoom = 38.704f; // 1.1^38.704 = 40
			zoom += event.mouseWheelScroll.delta;
			pixelsPerUnit = powf(1.1f, zoom);
			break;
		}
		case sf::Event::Resized:
		{
			// TODO
			break;
		}
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

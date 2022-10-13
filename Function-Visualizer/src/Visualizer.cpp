#include "Visualizer.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <iostream>

// TODO: Remove unused static libs

namespace App {

	Visualizer::Visualizer(sf::RenderWindow* renderWindow)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(width, height), "Function Visualizer",
			sf::Style::Default, sf::ContextSettings(0, 0, 4));
#if NDEBUG
		window->setVerticalSyncEnabled(true);
#endif

		ImGui::SFML::Init(*window);
		ImGui::GetIO().IniFilename = nullptr;
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
		UpdateFunctions();
		UpdateGridLines();
	}

	void Visualizer::UpdateImGui(sf::Time ts)
	{
#ifdef DEBUG
		ImGui::Begin("Debug");
		ImGui::Text("%.0f FPS", 1 / ts.asSeconds());
		size_t vertexCount = 0;
		for (auto& fData : functions)
			vertexCount += fData.Vertices.getVertexCount();
		ImGui::Text("Vertices: %d", vertexCount);
		ImGui::End();
#endif

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { .5f, .5f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0, 0, 0, .9f });

		bool open = ImGui::Begin("Functions", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(2);

		canDragGraph = !(ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive());

		if (!open)
		{
			ImGui::End();
			return;
		}

		ImGui::Text("Add new:");

		ImGui::SameLine();

		static char exprCString[32]{};
		bool enterNewFunction = false;

		ImGui::SetNextItemWidth(150);
		if (ImGui::InputText("##f(x)", exprCString, sizeof(exprCString), ImGuiInputTextFlags_EnterReturnsTrue))
			enterNewFunction = true;

		ImGui::SameLine();

		if (ImGui::Button("Enter"))
			enterNewFunction = true;

		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

		int indexToRemove = -1;
		for (int i = 0; i < functions.size(); i++)
		{
			auto& fData = functions[i];
			ImGui::PushID(i);

			ImGui::Text("%c(x) =", 'A' + (i + 5) % 26);

			ImGui::SameLine();
			ImGui::SetNextItemWidth(150);

			if (ImGui::InputText("##input", fData.Buffer, sizeof(fData.Buffer)))
			{
				exprtk::parser<float> parser;
				parser.compile(fData.Buffer, fData.Expression);
			}

			ImGui::SameLine();

			ImVec4 col = fData.Color;
			if (ImGui::ColorEdit3("##color", &col.x, ImGuiColorEditFlags_NoInputs))
			{
				fData.Color = col;
			}

			ImGui::SameLine();

			if (ImGui::Button("Del"))
				indexToRemove = i;

			ImGui::PopID();
		}

		ImGui::End();

		if (indexToRemove != -1)
			functions.erase(functions.begin() + indexToRemove);

		if (enterNewFunction)
		{
			exprtk::symbol_table<float> symbolTable;
			exprtk::expression<float> expression;
			exprtk::parser<float> parser;

			FunctionData fData;
			symbolTable.add_variable("x", *fData.X);
			symbolTable.add_constants();
			expression.register_symbol_table(symbolTable);
			if (parser.compile(exprCString, expression))
			{
				fData.Expression = expression;
				fData.Vertices.resize(width);

				strncpy_s(fData.Buffer, sizeof(fData.Buffer), exprCString, strlen(exprCString));
				memset(exprCString, 0, sizeof(exprCString));

				functions.push_back(std::move(fData));
			}
		}
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
	}

	void Visualizer::UpdateFunctions()
	{
		for (auto& fData : functions)
		{
			if (fData.Vertices.getVertexCount() != width)
				fData.Vertices.resize(width);

			for (int drawX = 0; drawX < width; drawX++)
			{
				float& x = *fData.X;
				x = (drawX - width / 2.f) / pixelsPerUnit;
				x -= graphOffset.x / pixelsPerUnit;

				float y = fData.Expression.value();
				y -= graphOffset.y / pixelsPerUnit;

				float drawY = height - (y * pixelsPerUnit + height / 2.f);

				fData.Vertices[drawX] = { {(float)drawX, drawY}, fData.Color };
			}
		}
	}

	void Visualizer::UpdateGridLines()
	{
		int rows = 4;
		int cols = 4;
		int numLines = rows + cols - 2;
		float w = (float)width;
		float h = (float)height;
		float rowH = h / rows;
		float colW = w / cols;

		grid.resize((size_t)2 * numLines);

		// Rows
		for (int i = 0; i < rows - 1; i++) {
			int r = i + 1;
			float rowY = rowH * r;
			grid[i * 2].position = { 0, rowY + graphOffset.y };
			grid[i * 2 + 1].position = { w, rowY + graphOffset.y };
		}

		// Columns
		for (int i = rows - 1; i < numLines; i++) {
			int c = i - rows + 2;
			float colX = colW * c;
			grid[i * 2].position = { colX + graphOffset.x, 0 };
			grid[i * 2 + 1].position = { colX + graphOffset.x, h };
		}

		for (int i = 0; i < grid.getVertexCount(); i++)
			grid[i].color.a = 60;
	}

	void Visualizer::Draw()
	{
		window->clear();

		window->draw(grid);

		for (auto& fData : functions)
			window->draw(fData.Vertices);

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
			if (canDragGraph)
			{
				static float zoom = 38.704f; // 1.1^38.704 = 40
				zoom += event.mouseWheelScroll.delta;
				pixelsPerUnit = powf(1.1f, zoom);
			}
			break;
		}
		case sf::Event::Resized:
		{
			width = event.size.width;
			height = event.size.height;
			window->setView(sf::View(sf::FloatRect(0, 0, (float)width, (float)height)));
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

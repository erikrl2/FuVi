#include "FuVi.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <iostream>

#ifdef NDEBUG
#include <Windows.h>
#define MAIN() wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE previousInstance, _In_ PWSTR arguments, _In_ int commandShow)
#else
#define MAIN() main(int argc, char** argv)
#endif

namespace App {

	FuVi::FuVi(sf::RenderWindow* renderWindow)
		: window(renderWindow), grid(renderWindow)
	{
		window->create(sf::VideoMode(width, height), "FuVi - Graphing Calculator",
			sf::Style::Default, sf::ContextSettings(0, 0, 4));
#if NDEBUG
		window->setVerticalSyncEnabled(true);
#endif

		ImGui::SFML::Init(*window);
		ImGui::GetIO().IniFilename = nullptr;
	}

	FuVi::~FuVi()
	{
		ImGui::SFML::Shutdown(*window);
	}

	void FuVi::Update(sf::Time ts)
	{
		ImGui::SFML::Update(*window, ts);
		UpdateImGui(ts);
		grid.Update();
		UpdateFunctions();
	}

	void FuVi::UpdateImGui(sf::Time ts)
	{
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { .5f, .5f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0, 0, 0, .9f });

		bool open = ImGui::Begin("Functions", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(2);

		grid.canDragGraph = grid.canDragGraph && !(ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive());

		if (!open)
		{
			ImGui::End();
			return;
		}

		ImGui::Text("%c(x) =", 'A' + (functions.size() + 5) % 26);

		ImGui::SameLine();

		static char exprCString[32]{};
		bool enterNewFunction = false;

		ImGui::SetNextItemWidth(150);
		if (ImGui::InputText("##function", exprCString, sizeof(exprCString), ImGuiInputTextFlags_EnterReturnsTrue))
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

#ifdef DEBUG
		ImGui::Begin("Debug", 0, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("%.0f FPS", 1 / ts.asSeconds());
		size_t vertexCount = grid.lines.getVertexCount();
		for (auto& fData : functions)
			vertexCount += fData.Vertices.getVertexCount();
		ImGui::Text("Vertices: %d", vertexCount);
		ImGui::Text("PixelsPerUnit: %f", grid.pixelsPerUnit);
		ImGui::Text("ZoomFactor: %f", grid.zoomFactor);
		ImGui::Text("GraphOffset: [%d, %d]", grid.offset.x, grid.offset.y);
		ImGui::End();
#endif
	}

	void FuVi::UpdateFunctions()
	{
		for (auto& fData : functions)
		{
			if (fData.Vertices.getVertexCount() != width)
				fData.Vertices.resize(width);

			for (int drawX = 0; drawX < width; drawX++)
			{
				float& x = *fData.X;
				x = (drawX - width / 2.f) / grid.pixelsPerUnit;
				x -= grid.offset.x / grid.pixelsPerUnit;

				float y = fData.Expression.value();
				y -= grid.offset.y / grid.pixelsPerUnit;

				float drawY = height - (y * grid.pixelsPerUnit + height / 2.f);

				fData.Vertices[drawX] = { {(float)drawX, drawY}, fData.Color };
			}
		}
	}

	void FuVi::Draw()
	{
		window->clear();

		grid.Draw();

		for (auto& fData : functions)
			window->draw(fData.Vertices);

		ImGui::SFML::Render(*window);

		window->display();
	}

	void FuVi::OnEvent(sf::Event& event)
	{
		ImGui::SFML::ProcessEvent(event);

		grid.OnEvent(event);

		switch (event.type)
		{
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

int MAIN()
{
	Application::Launch<App::FuVi>();
	return 0;
}

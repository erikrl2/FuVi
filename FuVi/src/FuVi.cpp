#include "FuVi.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <iostream>
#include <format>

#ifdef NDEBUG
#include <Windows.h>
#define MAIN() wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE previousInstance, _In_ PWSTR arguments, _In_ int commandShow)
#else
#define MAIN() main(int argc, char** argv)
#endif

namespace App {

	Visualizer::Visualizer(sf::RenderWindow* renderWindow)
		: window(renderWindow)
	{
		window->create(sf::VideoMode(width, height), "FuVi - Graphing Calculator",
			sf::Style::Default, sf::ContextSettings(0, 0, 4));
#if NDEBUG
		window->setVerticalSyncEnabled(true);
#endif

		ImGui::SFML::Init(*window);
		ImGui::GetIO().IniFilename = nullptr;

		font.loadFromFile("assets/fonts/OpenSans/OpenSans-Medium.ttf");
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
		UpdateGrid();
	}

	void Visualizer::UpdateImGui(sf::Time ts)
	{
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { .5f, .5f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0, 0, 0, .9f });

		bool open = ImGui::Begin("Functions", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(2);

		canDragGraph = canDragGraph && !(ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive());

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
		size_t vertexCount = gridLines.getVertexCount();
		for (auto& fData : functions)
			vertexCount += fData.Vertices.getVertexCount();
		ImGui::Text("Vertices: %d", vertexCount);
		ImGui::Text("PixelsPerUnit: %f", pixelsPerUnit);
		ImGui::Text("ZoomFactor: %f", zoomFactor);
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
			canDragGraph = dragging = false;
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

	void Visualizer::UpdateGrid()
	{
		float w = (float)width;
		float h = (float)height;
		sf::Vector2i center(int(w / 2), int(h / 2));
		center += graphOffset;

		float cellSize = GetGridCellSize();

		int rowLines = (int)ceilf(h / cellSize);
		int colLines = (int)ceilf(w / cellSize);
		if (rowLines % 2 == 0) rowLines++;
		if (colLines % 2 == 0) colLines++;
		int rowLinesHalf = rowLines >> 1;
		int colLinesHalf = colLines >> 1;

		float rowYStart = center.y - cellSize * rowLinesHalf;
		float colXStart = center.x - cellSize * colLinesHalf;
		int cellOffsetY = (int)roundf(graphOffset.y / cellSize);
		int cellOffsetX = (int)roundf(graphOffset.x / cellSize);

		int numberIndex = 0;
		int rowNumberCount = rowLinesHalf;
		int colNumberCount = colLinesHalf;
		if ((rowLinesHalf + cellOffsetY) % 2) rowNumberCount--;
		if ((colLinesHalf + cellOffsetX) % 2) colNumberCount--;
		if (abs(cellOffsetY) > rowLinesHalf) rowNumberCount++;
		if (abs(cellOffsetX) > colLinesHalf) colNumberCount++;

		gridLines.resize((size_t)(rowLines * 2 + colLines * 2));
		gridNumbers.resize((size_t)rowNumberCount + colNumberCount + 1);

		for (int i = 0; i < rowLines; i++)
		{
			int gridIndex = i * 2;
			float rowY = rowYStart + cellSize * (i - cellOffsetY);

			gridLines[(size_t)gridIndex].position = { 0, rowY };
			gridLines[(size_t)gridIndex + 1].position = { w, rowY };

			int gridNumber = rowLinesHalf - (i - cellOffsetY);

			sf::Uint8 alpha = (int)rowY == center.y ? 64 : gridNumber % 2 == 0 ? 32 : 16;
			gridLines[(size_t)gridIndex].color.a = alpha;
			gridLines[(size_t)gridIndex + 1].color.a = alpha;

			if (gridNumber % 2 == 0 && gridNumber != 0)
			{
				int precision = zoomFactor < 1 ? (int)-log2(zoomFactor) : 0;
				GridNumber number((gridNumber >> 1) * zoomFactor, precision, font);
				number.SetPositionWithinBounds({ w / 2 + graphOffset.x, rowY }, { 10, 10, w - 10, h - 10 });
				gridNumbers[numberIndex++] = number;
			}
		}

		for (int i = 0; i < colLines; i++)
		{
			int gridIndex = rowLines * 2 + i * 2;
			float colX = colXStart + cellSize * (i - cellOffsetX);

			gridLines[(size_t)gridIndex].position = { colX, 0 };
			gridLines[(size_t)gridIndex + 1].position = { colX, h };

			int gridNumber = (colLinesHalf - (i - cellOffsetX)) * -1;

			sf::Uint8 alpha = (int)colX == center.x ? 64 : gridNumber % 2 == 0 ? 32 : 16;
			gridLines[(size_t)gridIndex].color.a = alpha;
			gridLines[(size_t)gridIndex + 1].color.a = alpha;

			if (gridNumber % 2 == 0 && gridNumber != 0)
			{
				int precision = zoomFactor < 1 ? (int)-log2(zoomFactor) : 0;
				GridNumber number((gridNumber >> 1) * zoomFactor, precision, font);
				number.SetPositionWithinBounds({ colX, h / 2 + graphOffset.y }, { 10, 10, w - 10, h - 10 }, false);
				gridNumbers[numberIndex++] = number;
			}
		}
		sf::Text number("0", font, 14);
		number.setPosition({ w / 2 + graphOffset.x - 8, h / 2 + graphOffset.y });
		gridNumbers[numberIndex] = number;
	}

	float Visualizer::GetGridCellSize()
	{
		float unitCellSize = pixelsPerUnit;
		uint64_t factor = 1;

		while (pixelsPerUnit < baseUnit / factor)
		{
			factor <<= 1;
			unitCellSize = pixelsPerUnit * factor;
		}

		if (factor > 1)
		{
			zoomFactor = (double)factor;
			return unitCellSize / 2;
		}

		while (pixelsPerUnit >= baseUnit * (factor << 1))
		{
			factor <<= 1;
			unitCellSize = pixelsPerUnit / factor;
		}

		zoomFactor = 1.0 / factor;

		return unitCellSize / 2;
	}

	void Visualizer::Draw()
	{
		window->clear();

		window->draw(gridLines);

		for (auto& text : gridNumbers)
			window->draw(text);

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
		case sf::Event::MouseButtonPressed:
		{
			auto mouse = event.mouseButton;
			if (mouse.button == sf::Mouse::Left)
			{
				canDragGraph = mouse.x >= 0 && mouse.x < width
					&& mouse.y >= 0 && mouse.y < height;
			}
			break;
		}
		case sf::Event::MouseWheelScrolled:
		{
			static float zoom = logf(baseUnit) / logf(1.1f);
			float dir = event.mouseWheelScroll.delta;

			if (pixelsPerUnit < baseUnit / ((uint64_t)1 << 62) && dir < 0) break;
			if (pixelsPerUnit >= baseUnit * ((uint64_t)1 << 62) && dir > 0) break;

			zoom += dir;
			pixelsPerUnit = powf(1.1f, zoom);
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

int MAIN()
{
	Application::Launch<App::Visualizer>();
	return 0;
}

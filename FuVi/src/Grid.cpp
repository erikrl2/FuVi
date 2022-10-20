#include "Grid.h"

namespace App {

	GridNumber::GridNumber(double number, int precision, const sf::Font& font)
		: Text("", font, 14)
	{
		char format[] = "%.0f";
		format[2] = '0' + std::max(0, std::min(7, precision));
		char numberString[32]{};
		sprintf_s(numberString, sizeof(numberString), format, number);
		Text.setString(numberString);

		textWidth = strnlen_s(numberString, sizeof(numberString)) * 8.f;
	}

	void GridNumber::SetPositionWithinBounds(sf::Vector2f pos, const sf::FloatRect& bounds, bool isYAxis)
	{
		float x = isYAxis ? std::max(bounds.left, std::min(bounds.width - textWidth, pos.x - textWidth)) : pos.x - textWidth / 2;
		float y = !isYAxis ? std::max(bounds.top, std::min(bounds.height - 16, pos.y)) : pos.y - 8;
		Text.setPosition(x, y);
	}

	Grid::Grid(sf::RenderWindow* window)
		: window(window)
	{
		font.loadFromFile("assets/fonts/OpenSans/OpenSans-Medium.ttf");
	}

	void Grid::Update()
	{
		width = (float)window->getSize().x;
		height = (float)window->getSize().y;

		UpdateGraphOffset();

		sf::Vector2i center(int(width / 2), int(height / 2));
		center += offset;

		float cellSize = GetGridCellSize();

		int rowLines = (int)ceilf(height / cellSize);
		int colLines = (int)ceilf(width / cellSize);
		if (rowLines % 2 == 0) rowLines++;
		if (colLines % 2 == 0) colLines++;
		int rowLinesHalf = rowLines >> 1;
		int colLinesHalf = colLines >> 1;

		float rowYStart = center.y - cellSize * rowLinesHalf;
		float colXStart = center.x - cellSize * colLinesHalf;
		int cellOffsetY = (int)roundf(offset.y / cellSize);
		int cellOffsetX = (int)roundf(offset.x / cellSize);

		int numberIndex = 0;
		int rowNumberCount = rowLinesHalf;
		int colNumberCount = colLinesHalf;
		if ((rowLinesHalf + cellOffsetY) % 2) rowNumberCount--;
		if ((colLinesHalf + cellOffsetX) % 2) colNumberCount--;
		if (abs(cellOffsetY) > rowLinesHalf) rowNumberCount++;
		if (abs(cellOffsetX) > colLinesHalf) colNumberCount++;

		lines.resize((size_t)(rowLines * 2 + colLines * 2));
		numbers.resize((size_t)rowNumberCount + colNumberCount + 1);

		for (int i = 0; i < rowLines; i++)
		{
			int gridIndex = i * 2;
			float rowY = rowYStart + cellSize * (i - cellOffsetY);

			lines[(size_t)gridIndex].position = { 0, rowY };
			lines[(size_t)gridIndex + 1].position = { width, rowY };

			int gridNumber = rowLinesHalf - (i - cellOffsetY);

			sf::Uint8 alpha = (int)rowY == center.y ? 64 : gridNumber % 2 == 0 ? 32 : 16;
			lines[(size_t)gridIndex].color.a = alpha;
			lines[(size_t)gridIndex + 1].color.a = alpha;

			if (gridNumber % 2 == 0 && gridNumber != 0)
			{
				int precision = zoomFactor < 1 ? (int)-log2(zoomFactor) : 0;
				GridNumber number((gridNumber >> 1) * zoomFactor, precision, font);
				number.SetPositionWithinBounds({ width / 2 + offset.x, rowY }, { 10, 10, width - 10, height - 10 }, true);
				numbers[numberIndex++] = number.Text;
			}
		}

		for (int i = 0; i < colLines; i++)
		{
			int gridIndex = rowLines * 2 + i * 2;
			float colX = colXStart + cellSize * (i - cellOffsetX);

			lines[(size_t)gridIndex].position = { colX, 0 };
			lines[(size_t)gridIndex + 1].position = { colX, height };

			int gridNumber = (colLinesHalf - (i - cellOffsetX)) * -1;

			sf::Uint8 alpha = (int)colX == center.x ? 64 : gridNumber % 2 == 0 ? 32 : 16;
			lines[(size_t)gridIndex].color.a = alpha;
			lines[(size_t)gridIndex + 1].color.a = alpha;

			if (gridNumber % 2 == 0 && gridNumber != 0)
			{
				int precision = zoomFactor < 1 ? (int)-log2(zoomFactor) : 0;
				GridNumber number((gridNumber >> 1) * zoomFactor, precision, font);
				number.SetPositionWithinBounds({ colX, height / 2 + offset.y }, { 10, 10, width - 10, height - 10 }, false);
				numbers[numberIndex++] = number.Text;
			}
		}
		sf::Text number("0", font, 14);
		number.setPosition({ width / 2 + offset.x - 8, height / 2 + offset.y });
		numbers[numberIndex] = number;

	}

	void Grid::UpdateGraphOffset()
	{
		static bool dragging = false;
		static sf::Vector2i lastPos;

		if (canDragGraph && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
			if (dragging) offset += mousePos - lastPos;
			lastPos = mousePos;
			dragging = true;
		}
		else
		{
			canDragGraph = dragging = false;
		}
	}

	float Grid::GetGridCellSize()
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

	void Grid::OnEvent(sf::Event& event)
	{
		switch (event.type)
		{
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
		}
	}

	void Grid::Draw()
	{
		window->draw(lines);

		for (auto& text : numbers)
			window->draw(text);
	}

}

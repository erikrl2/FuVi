#pragma once

#include "Application.h"

#include <functional>
#include <vector>

namespace App {

	constexpr uint32_t Width = 1280;
	constexpr uint32_t Height = 720;

	struct FunctionData
	{
		sf::Color Color;
		std::function<float(float)> Function;
		sf::Vertex vertices[Width]{};

		FunctionData(sf::Color color, const auto& func)
			: Color(color), Function(func) {}
	};

	class Visualizer : public Application
	{
	public:
		Visualizer(sf::RenderWindow* renderWindow);
		~Visualizer() override;

		void Update(sf::Time ts) override;
		void Draw() override;
		void OnEvent(sf::Event& event) override;

		void UpdateImGui(sf::Time ts);
	private:
		sf::RenderWindow* window = nullptr;

		std::vector<FunctionData> functions;

		float pixelsPerUnit = 40;
	};

	//namespace Utils {

	//	inline bool IsDigit(const std::string& s)
	//	{
	//		return !s.empty() && std::find_if(s.begin(), s.end(),
	//			[](unsigned char c) { return std::isdigit(c); }) != s.end();
	//	}

	//}

}


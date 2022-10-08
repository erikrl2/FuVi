#pragma once

#include "Application.h"

#include <functional>
#include <vector>

namespace App {

	constexpr uint32_t Width = 1280;
	constexpr uint32_t Height = 720;

	struct FunctionData
	{
		std::function<float(float)> Function;
		sf::Color Color;

		sf::Vertex vertices[Width]{};

		char Buffer[32]{};

		FunctionData() = default;
		FunctionData(const auto& func, sf::Color color)
			: Function(func), Color(color) {}
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

		void DisplayNewPrompt();
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


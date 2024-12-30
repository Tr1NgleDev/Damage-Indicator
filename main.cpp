//#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>
#include <fstream>

using namespace fdm;

// Initialize the DLLMain
initDLL

glm::vec4 color{ 1.0f, 0.0f, 0.0f, 0.4f };
float animDur = 0.5f;
std::string configPath;

$hook(void, Player, init, const glm::ivec4& spawnPoint)
{
	original(self, spawnPoint);
	self->damageTime = -animDur * 10.f;
}

$hook(void, StateGame, render, StateManager& s)
{
	original(self, s);

	Player& player = self->player;
	QuadRenderer& qr = self->qr;

	double time = glfwGetTime();

	if (time - player.damageTime < animDur)
	{
		glDepthMask(0);
		glDisable(GL_DEPTH_TEST);

		int w, h;
		glfwGetWindowSize(s.window, &w, &h);

		qr.setQuadRendererMode(GL_TRIANGLES); // <------ Tr1Ngle ReFeReNcE?????????!?!??!?!??!?1/?!?!/1/?!/1//!?/1/?!/?!
		
		// animation time
		float a = 1.f - (time - player.damageTime) / animDur;

		glm::vec4 colorA = color * a;
		glm::vec4 colorB = color;
		colorB.a = 0;

		// left side
		qr.setColor({ colorB, colorB, colorA, colorA });
		//qr.setColor(1, 0, 0, 0.5f);
		qr.setPos(0, 0, w / 2, h);
		qr.render();

		// top side
		qr.setColor({ colorB, colorA, colorA, colorB });
		//qr.setColor(0, 1, 0, 0.5f);
		qr.setPos(0, 0, w, h / 2);
		qr.render();

		// right side
		qr.setColor({ colorA, colorA, colorB, colorB });
		//qr.setColor(0, 0, 1, 0.5f);
		qr.setPos(w / 2, 0, w / 2, h);
		qr.render();

		// bottom side
		qr.setColor({ colorA, colorB, colorB, colorA });
		//qr.setColor(1, 0, 1, 0.5f);
		qr.setPos(0, h / 2, w, h / 2);
		qr.render();

		glEnable(GL_DEPTH_TEST);
		glDepthMask(1);
	}
}

void updateConfig(const std::string& path, const nlohmann::json& j)
{
	std::ofstream configFileO(path);
	if (configFileO.is_open())
	{
		configFileO << j.dump(4);
		configFileO.close();
	}
}

inline static int getY(gui::Element* element)
{
	// im comparing typeid name strings instead of using dynamic_cast because typeids of 4dminer and typeids of 4dm.h are different
	if (0 == strcmp(typeid(*element).name(), "class gui::Button"))
		return ((gui::Button*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class gui::CheckBox"))
		return ((gui::CheckBox*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class gui::Image"))
		return ((gui::Image*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class gui::Slider"))
		return ((gui::Slider*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class gui::Text"))
		return ((gui::Text*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class gui::TextInput"))
		return ((gui::TextInput*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class fdm::gui::Button"))
		return ((gui::Button*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class fdm::gui::CheckBox"))
		return ((gui::CheckBox*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class fdm::gui::Image"))
		return ((gui::Image*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class fdm::gui::Slider"))
		return ((gui::Slider*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class fdm::gui::Text"))
		return ((gui::Text*)element)->yOffset;
	else if (0 == strcmp(typeid(*element).name(), "class fdm::gui::TextInput"))
		return ((gui::TextInput*)element)->yOffset;
	return 0;
}

static bool initializedSettings = false;
$hook(void, StateSettings, init, StateManager& s)
{
	initializedSettings = false;
	original(self, s);
}

$hook(void, StateSettings, render, StateManager& s)
{
	original(self, s);

	if (!initializedSettings)
	{
		int lowestY = 0;
		for (auto& e : self->mainContentBox.elements)
		{
			if (e == &self->secretButton) // skip the secret button
				continue;

			lowestY = std::max(getY(e), lowestY);
		}
		int oldLowest = lowestY;

		static gui::Text title{};
		title.setText("Damage-Indicator Options:");
		title.alignX(gui::ALIGN_CENTER_X);
		title.offsetY(lowestY += 100);
		title.size = 2;

		static gui::Slider rSlider{};
		rSlider.value = floorf(color.r * 100);
		rSlider.setText(std::format("R: {:.2f}", color.r));
		rSlider.alignX(gui::ALIGN_CENTER_X);
		rSlider.offsetY(lowestY += 100);
		rSlider.range = 100;
		rSlider.width = rSlider.range + 500; // 600
		rSlider.user = &rSlider;
		rSlider.callback = [](void* user, int value)
			{
				color.r = value / 100.f;
				((gui::Slider*)user)->setText(std::format("R: {:.2f}", color.r));
				updateConfig(configPath, { { "color", m4::vec4ToJson(color)}, { "animDur", animDur } });
			};
		static gui::Slider gSlider{};
		gSlider.value = floorf(color.g * 100);
		gSlider.setText(std::format("G: {:.2f}", color.g));
		gSlider.alignX(gui::ALIGN_CENTER_X);
		gSlider.offsetY(lowestY += 100);
		gSlider.range = 100;
		gSlider.width = gSlider.range + 500; // 600
		gSlider.user = &gSlider;
		gSlider.callback = [](void* user, int value)
			{
				color.g = value / 100.f;
				((gui::Slider*)user)->setText(std::format("G: {:.2f}", color.g));
				updateConfig(configPath, { { "color", m4::vec4ToJson(color)}, { "animDur", animDur } });
			};
		static gui::Slider bSlider{};
		bSlider.value = floorf(color.g * 100);
		bSlider.setText(std::format("B: {:.2f}", color.b));
		bSlider.alignX(gui::ALIGN_CENTER_X);
		bSlider.offsetY(lowestY += 100);
		bSlider.range = 100;
		bSlider.width = bSlider.range + 500; // 600
		bSlider.user = &bSlider;
		bSlider.callback = [](void* user, int value)
			{
				color.b = value / 100.f;
				((gui::Slider*)user)->setText(std::format("B: {:.2f}", color.b));
				updateConfig(configPath, { { "color", m4::vec4ToJson(color)}, { "animDur", animDur } });
			};
		static gui::Slider aSlider{};
		aSlider.value = floorf(color.a * 100);
		aSlider.setText(std::format("A: {:.2f}", color.a));
		aSlider.alignX(gui::ALIGN_CENTER_X);
		aSlider.offsetY(lowestY += 100);
		aSlider.range = 100;
		aSlider.width = aSlider.range + 500; // 600
		aSlider.user = &aSlider;
		aSlider.callback = [](void* user, int value)
			{
				color.a = value / 100.f;
				((gui::Slider*)user)->setText(std::format("A: {:.2f}", color.a));
				updateConfig(configPath, { { "color", m4::vec4ToJson(color)}, { "animDur", animDur } });
			};
		static gui::Slider durSlider{};
		durSlider.value = floorf(animDur * 100);
		durSlider.setText(std::format("Duration: {:.2f}s", animDur));
		durSlider.alignX(gui::ALIGN_CENTER_X);
		durSlider.offsetY(lowestY += 100);
		durSlider.range = 100;
		durSlider.width = durSlider.range + 500; // 600
		durSlider.user = &durSlider;
		durSlider.callback = [](void* user, int value)
			{
				animDur = value / 100.f;
				((gui::Slider*)user)->setText(std::format("Duration: {:.2f}s", animDur));
				updateConfig(configPath, { { "color", m4::vec4ToJson(color)}, { "animDur", animDur }});
			};

		self->mainContentBox.addElement(&title);
		self->mainContentBox.addElement(&rSlider);
		self->mainContentBox.addElement(&gSlider);
		self->mainContentBox.addElement(&bSlider);
		self->mainContentBox.addElement(&aSlider);
		self->mainContentBox.addElement(&durSlider);

		self->secretButton.yOffset += lowestY - oldLowest;
		self->mainContentBox.scrollH += lowestY - oldLowest;

		initializedSettings = true;
	}
}

$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	// initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();

	// the config

	configPath = std::format("{}/config.json", fdm::getModPath(fdm::modID));

	nlohmann::json configJson
	{
		{ "color", m4::vec4ToJson(color) },
		{ "animDur", animDur }
	};

	if (!std::filesystem::exists(configPath))
	{
		updateConfig(configPath, configJson);
	}
	else
	{
		std::ifstream configFileI(configPath);
		if (configFileI.is_open())
		{
			configJson = nlohmann::json::parse(configFileI);
			configFileI.close();
		}
	}

	if (!configJson.contains("color"))
	{
		configJson["color"] = m4::vec4ToJson(color);
		updateConfig(configPath, configJson);
	}
	if (!configJson.contains("animDur") || !configJson["animDur"].is_number_float())
	{
		configJson["animDur"] = animDur;
		updateConfig(configPath, configJson);
	}

	color = m4::vec4FromJson(configJson["color"]);
	animDur = configJson["animDur"];
}
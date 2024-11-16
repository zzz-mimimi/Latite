#pragma once
#include "sdk/Util.h"
#include "util/LMath.h"

namespace SDK {
	class ClientInstance {
	public:
		MVCLASS_FIELD(class MinecraftGame*, minecraftGame, 0xD0, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xA8, 0xA8);
		MVCLASS_FIELD(class Minecraft*, minecraft, 0xD8, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xC0, 0xC0);
		MVCLASS_FIELD(class LevelRenderer*, levelRenderer, 0xE8, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xD0, 0xD0);
		MVCLASS_FIELD(class ClientInputHandler*, inputHandler, 0x118, 0x110, 0x110, 0x110, 0x110, 0x110, 0x100, 0x100);
		MVCLASS_FIELD(struct Vec2, cursorPos, 0x4D0, 0x4C8, 0x4C8, 0x4C8, 0x4C8, 0x498, 0x458, 0x458);

		static ClientInstance* get();
		class BlockSource* getRegion();
		class LocalPlayer* getLocalPlayer();
		class GuiData* getGuiData();
		class Options* getOptions();

		void grabCursor();
		void releaseCursor();
	private:
		static ClientInstance* instance;
	};
}
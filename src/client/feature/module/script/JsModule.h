#pragma once
#include "client/script/feature/JsEvented.h"
#include "client/feature/module/Module.h"
#include "util/ChakraUtil.h"

class JsModule : public Module, public JsEvented {
public:
	JsModule(std::string const& name, std::string const& displayName, 
		std::string const& desc, int key);

	~JsModule() {
		JS::JsRelease(object, nullptr);
	}

	void onEnable() override;
	void onDisable() override;
	bool shouldHoldToToggle() override;
	JsValueRef object = JS_INVALID_REFERENCE;
private:
	std::string displayName;

	JsContextRef ctx = JS_INVALID_REFERENCE;
};
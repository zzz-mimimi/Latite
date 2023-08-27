#include "GameScriptingObject.h"
#include "../JsScript.h"
#include "client/script/class/impl/JsVec2.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/game/MinecraftGame.h"
#include "sdk/common/client/renderer/game/LevelRenderer.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/world/Minecraft.h"
#include "client/Latite.h"
#include "client/script/ScriptManager.h"
#include "util/Logger.h"
#include "sdk/common/network/packet/TextPacket.h"
#include "util/XorString.h"
#include "../class/impl/game/JsEntityClass.h"

void GameScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	this->createWorldObject();
	Chakra::DefineFunc(object, getMousePosCallback, L"getMousePos");
	Chakra::DefineFunc(object, isInUICallback, L"isInUI");
	Chakra::DefineFunc(object, playSoundUI, L"playSoundUI");
	Chakra::DefineFunc(object, sendChatCallback, L"sendChatMessage");
	Chakra::DefineFunc(object, getWorldCallback, L"getWorld", this);
}

void GameScriptingObject::createWorldObject() {
	JS::JsCreateObject(&worldObj);
	Chakra::DefineFunc(worldObj, worldGetEntList, L"getEntityList", this);
}

JsValueRef GameScriptingObject::worldGetEntList(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		Chakra::ThrowError(L"World is not allowed to be used here");
		return Chakra::GetUndefined();
	}

	JsScript* scr;
	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JS::JsGetContextData(ctx, reinterpret_cast<void**>(&scr));

	if (!Latite::getScriptManager().hasPermission(scr, ScriptManager::Permission::Operator)) {
		Chakra::ThrowError(util::StrToWStr(XOR_STRING("No permission to use getEntityList here")));
		return JS_INVALID_REFERENCE;
	}

	auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
	JsValueRef array;
	auto entList = lvl->getRuntimeActorList();
	unsigned sz = static_cast<unsigned>(entList.size());
	JS::JsCreateArray(sz, &array);
	unsigned idx = 0;
	for (auto& ent : entList) {
		JsValueRef db;
		JS::JsDoubleToNumber(static_cast<double>(idx), &db);
		JsScript* script;
		JsContextRef ctx;
		JS::JsGetCurrentContext(&ctx);
		JS::JsGetContextData(ctx, reinterpret_cast<void**>(&script));

		auto entc = script->getClass<JsEntityClass>();
		JS::JsSetIndexedProperty(array, db, entc->construct(new JsEntity(ent->getRuntimeID()), true));
		idx++;
		Chakra::Release(db);
	}
	return array;
}

JsValueRef GameScriptingObject::worldGetEntCount(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return JsValueRef();
}

JsValueRef GameScriptingObject::getMousePosCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsScript* script;
	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JS::JsGetContextData(ctx, reinterpret_cast<void**>(&script));

	auto vec2 = script->getClass<JsVec2>();
	if (vec2) {
		return vec2->construct(SDK::ClientInstance::get()->cursorPos);
	}

	// could not find Vector2
	__debugbreak();
	return Chakra::GetUndefined();
}

JsValueRef GameScriptingObject::getWorldCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!SDK::ClientInstance::get()->minecraft->getLevel()) return Chakra::GetNull();
	JsContextRef ct;
	JS::JsGetCurrentContext(&ct);
	return reinterpret_cast<GameScriptingObject*>(callbackState)->worldObj;
}

JsValueRef GameScriptingObject::isInUICallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed() ? Chakra::GetTrue() : Chakra::GetFalse();
}

JsValueRef GameScriptingObject::sendChatCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsValueType::JsString} })) return JS_INVALID_REFERENCE;

	JsScript* script;
	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JS::JsGetContextData(ctx, reinterpret_cast<void**>(&script));
	
	if (Latite::getScriptManager().hasPermission(script, ScriptManager::Permission::SendChat)) {
		auto lp = SDK::ClientInstance::get()->getLocalPlayer();
		if (lp) {
			SDK::TextPacket tp{};
			String s{};
			s.setString(util::WStrToStr(Chakra::GetString(arguments[1])).c_str());
			tp.chat(s);
			lp->packetSender->sendToServer(tp);
		}

		return Chakra::GetUndefined();
	}
	Chakra::ThrowError(util::StrToWStr(XOR_STRING("Permission denied to use sendChatMessage")));
	return Chakra::GetUndefined();
}

JsValueRef GameScriptingObject::playSoundUI(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsNumber}, {arguments[3], JsNumber} })) return JS_INVALID_REFERENCE;

	std::string soundName = util::WStrToStr(Chakra::GetString(arguments[1]));
	float volume = static_cast<float>(Chakra::GetNumber(arguments[2]));
	float pitch = static_cast<float>(Chakra::GetNumber(arguments[3]));

	auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
	if (lvl) {
		lvl->playSoundEvent(soundName, SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer()->getOrigin(), pitch);
	}
	return Chakra::GetUndefined();
}

#include "pch.h"
#include "Zoom.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"

Zoom::Zoom() : Module("Zoom", "Zoom", "Zooms like OptiFine", GAME, nokeybind) {
	addSetting("zoomKey", "Zoom Key", "The key to press to zoom", this->zoomKey);
	addSliderSetting("modifier", "Modifier", "How far to zoom", this->modifier, FloatValue(1.f), FloatValue(50.f), FloatValue(1.f));
	addSetting("animation", "Animation", "Whether to have a zoom animation or not", hasAnim);
	addSliderSetting("animationSpeed", "Speed", "The speed of the animation", animSpeed, FloatValue(1.f), FloatValue(5.f), FloatValue(1.f), "animation"_istrue);
	addSetting("cinematic", "Cinematic Camera", "Enable cinematic camera while Zoom is on", this->cinematicCam);
	addSetting("hideHand", "Hide Hand", "Whether or not to hide the hand when zooming.", this->hideHand);
	addSetting("dpiAdjust", "Adjust DPI", "Adjust the sensitivity while zooming.", this->dpiAdjust);

	listen<RenderLevelEvent>((EventListenerFunc)&Zoom::onRenderLevel);
	listen<KeyUpdateEvent>((EventListenerFunc)&Zoom::onKeyUpdate);
	listen<CinematicCameraEvent>((EventListenerFunc)&Zoom::onCinematicCamera);
	listen<HideHandEvent>((EventListenerFunc)&Zoom::onHideHand);
	listen<SensitivityEvent>((EventListenerFunc)&Zoom::onSensitivity);
}

void Zoom::onRenderLevel(Event& evGeneric) {
	auto& ev = reinterpret_cast<RenderLevelEvent&>(evGeneric);

	modifyTo = shouldZoom ? std::get<FloatValue>(modifier).value : 1.f;

	// partial ticks
	float alpha = Latite::getRenderer().getDeltaTime();
	float lr = modifyTo;
	if (std::get<BoolValue>(hasAnim)) lr = std::lerp(activeModifier, modifyTo, alpha * std::get<FloatValue>(animSpeed) / 10.f);
	activeModifier = lr;

	float& fx = ev.getLevelRenderer()->getLevelRendererPlayer()->getFovX();
	float& fy = ev.getLevelRenderer()->getLevelRendererPlayer()->getFovY();

	fx *= activeModifier;
	fy *= activeModifier;
}

void Zoom::onKeyUpdate(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	if (ev.inUI()) return;
	if (ev.getKey() == std::get<KeyValue>(this->zoomKey)) {
		this->shouldZoom = ev.isDown();
	}
}

void Zoom::onCinematicCamera(Event& evGeneric) {
	auto& ev = reinterpret_cast<CinematicCameraEvent&>(evGeneric);
	if (shouldZoom && std::get<BoolValue>(this->cinematicCam)) {
		ev.setValue(true);
	}
}

void Zoom::onHideHand(Event& evG) {
	auto& ev = reinterpret_cast<HideHandEvent&>(evG);
	if (shouldZoom && std::get<BoolValue>(this->hideHand)) {
		ev.getValue() = true;
	}
}

void Zoom::onSensitivity(Event& evG) {
	auto& ev = reinterpret_cast<SensitivityEvent&>(evG);
	if (shouldZoom && std::get<BoolValue>(this->dpiAdjust)) {
		ev.getValue() = ev.getValue() * (1 / std::get<FloatValue>(modifier));
	}
}
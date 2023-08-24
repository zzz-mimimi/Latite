#pragma once
#include "../../Module.h"
#include "client/event/impl/RenderLevelEvent.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/CinematicCameraEvent.h"
#include "api/feature/setting/Setting.h"

class Zoom : public Module {
public:

	Zoom();
	~Zoom() = default;

	void onRenderLevel(Event& ev);
	void onKeyUpdate(Event& ev);
	void onCinematicCamera(Event& ev);
private:
	ValueType zoomKey = KeyValue('C');
	ValueType modifier = FloatValue(15.f);
	ValueType hasAnim = BoolValue(false);
	ValueType animSpeed = FloatValue(2.f);
	ValueType cinematicCam = BoolValue(true);

	float activeModifier = 1.f;
	float modifyTo = 1.f;
	bool shouldZoom = false;
};
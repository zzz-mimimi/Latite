#include "pch.h"
#include "Notifications.h"
#include <client/render/Renderer.h>
#include <client/Latite.h>

Notifications::Notifications() {
	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&Notifications::onRender, 10);
}

void Notifications::onRender(Event& evG) {
	auto& ev = reinterpret_cast<RenderOverlayEvent&>(evG);

	float transition = 300;
	float stay = 2000;

	D2DUtil dc;
	D2D1_SIZE_F ssize = Latite::getRenderer().getScreenSize();
	d2d::Rect ss = d2d::Rect(0, 0, ssize.width, ssize.height);

	const d2d::Color bgCol = d2d::Color::RGB(7, 7, 7).asAlpha(0.80f);
	const d2d::Color outlineCol = d2d::Color::RGB(0, 0, 0).asAlpha(0.28f);
	const d2d::Color textCol = d2d::Colors::WHITE;

	if (!toasts.empty()) {
		auto now = std::chrono::system_clock::now();

		auto& toast = toasts.front();
		auto dur = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - toast.createTime).count());
		float transitionIn = std::max(0.F, transition - dur);
		float maxDur = stay + transition * 2;
		float transitionOut = static_cast<float>(stay + transition * 2) - transition;
		float transl = 1.f;
		if (transitionIn > 0) {
			// in
			transl = 1 - (transitionIn / transition);
		}
		else if (transitionOut <= dur) {
			// out
			transl = 1 - ((dur - transitionOut) / transition);
		}
		float opacity = transl;

		const float fontSize = 30.f;
		const Renderer::FontSelection font = Renderer::FontSelection::SegoeLight;

		auto textSize = dc.getTextSize(toast.message, font, fontSize);

		auto rec = dc.getTextRect(toast.message, font, fontSize, 15.f);
		dc.fillRoundedRectangle(rec, bgCol.asAlpha(opacity));
		dc.drawRoundedRectangle(rec, outlineCol.asAlpha(opacity), rec.getHeight() / 2.f, 3);

		dc.drawText(rec, toast.message, textCol.asAlpha(opacity), font, fontSize, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		if (dur > maxDur) {
			toasts.pop();
		};
	}

}
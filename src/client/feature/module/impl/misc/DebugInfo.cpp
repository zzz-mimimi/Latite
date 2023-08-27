#include "DebugInfo.h"
#include "Windows.h"

#include "client/Latite.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/RenderOverlayEvent.h"

#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/world/Minecraft.h"
#include "sdk/common/world/level/Dimension.h"
#include "sdk/signature/storage.h"

DebugInfo::DebugInfo() : Module("DebugInfo", "Debug Info", "See some craaazy info (send help)", GAME, this->debugInfoKey) {
    listen<RenderOverlayEvent>((EventListenerFunc)&DebugInfo::onRenderOverlay);
}

namespace {
    std::string getMinecraftVersion() {
        return std::format("Latite Client {}, Minecraft {}", Latite::get().version, Latite::get().gameVersion);
    }
    std::string getFPS() {
        return std::format("{} fps", Latite::get().getTimings().getFPS());
    }
    std::string getDimension() {
        return std::format("Dimension: {}", SDK::ClientInstance::get()->getLocalPlayer()->dimension->dimensionName);
    }
    std::string getCoordinates() {
        Vec3 position = SDK::ClientInstance::get()->getLocalPlayer()->getPos();
        return std::format("XYZ Coordinates: {:.3f} / {:.3f} / {:.3f}", position.x, position.y, position.z);
    }
    std::string getVelocity() {
        Vec3 velocity = SDK::ClientInstance::get()->getLocalPlayer()->getVelocity();
        return std::format("XYZ Velocity: {:.3f} / {:.3f} / {:.3f}", velocity.x, velocity.y, velocity.z);
    }
    std::string getRotation() {
        Vec2 rotation = SDK::ClientInstance::get()->getLocalPlayer()->getRot();
        return std::format("XY Rotation: {:.3f} / {:.3f} | Magnitude: {}", rotation.x, rotation.y, rotation.magnitude());
    }
    // TODO: block info, tps info, tick speed info, biome info, days ran on server.


    std::string getMemUsage() {
        // Get the total available memory
        MEMORYSTATUSEX memStatus;
        memStatus.dwLength = sizeof(memStatus);
        GlobalMemoryStatusEx(&memStatus);

        // Convert memory usage values to GB
        double usedMemoryGB = static_cast<double>(memStatus.ullTotalPhys - memStatus.ullAvailPhys) / (1024 * 1024 * 1024); // Convert to GB
        double totalMemoryGB = static_cast<double>(memStatus.ullTotalPhys) / (1024 * 1024 * 1024); // Convert to GB

        // Calculate the ratio of used memory to total memory
        double memoryUsageRatio = usedMemoryGB / totalMemoryGB;

        return std::format("Memory Usage: {:.2f} GB / {:.2f} GB", usedMemoryGB, totalMemoryGB);
    }
    std::string getGpuInfo() {
        return std::format("GPU: {}", reinterpret_cast<const char*>(Signatures::GpuInfo.result));
    }
    std::string getCpuInfo() {
        return std::format("CPU: {}", util::GetProcessorInfo());
    }
}

void DebugInfo::onRenderOverlay(Event& evG) {
    RenderOverlayEvent& ev = reinterpret_cast<RenderOverlayEvent&>(evG);
    DXContext dc;

    auto [width, height] = Latite::getRenderer().getScreenSize();
    d2d::Rect rect = { 0.f, 0.f, width, height };
    const std::wstring topLeftDebugInfo = util::StrToWStr(std::format("{}\n{}\n\n{}\n{}\n{}\n{}",
        getMinecraftVersion(),
        getFPS(),
        getDimension(),
        getCoordinates(),
        getVelocity(),
        getRotation()));
    const std::wstring topRightDebugInfo = util::StrToWStr(std::format("{}\n{}\n{}",
        getMemUsage(),
        getGpuInfo(),
        getCpuInfo()
        ));

    dc.drawText(rect, topLeftDebugInfo, d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular,
        28, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    dc.drawText(rect, topRightDebugInfo, d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular,
        28, DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}
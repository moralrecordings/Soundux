#pragma once
#include "../../helper/audio/audio.hpp"
#include "../../helper/threads/processing.hpp"
#include "../../ui/ui.hpp"
#include "../config/config.hpp"
#include "../hotkeys/hotkeys.hpp"
#include "objects.hpp"
#include <memory>

namespace Soundux
{
    namespace Globals
    {
        inline Objects::Data gData;
        inline Objects::Audio gAudio;
        inline Objects::Config gConfig;
        inline Objects::Hotkeys gHotKeys;
        inline Objects::Settings gSettings;
        inline std::unique_ptr<Objects::Window> gGui;
        inline Objects::ProcessingQueue<std::uintptr_t> gQueue;

        /* Allows for fast & easy sound access, is populated on start up */
        inline std::shared_mutex gSoundsMutex;
        inline std::map<std::uint32_t, Objects::Sound> gSounds;
    } // namespace Globals
} // namespace Soundux
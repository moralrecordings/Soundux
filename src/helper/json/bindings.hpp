#pragma once
#include <core/global/globals.hpp>
#include <core/hotkeys/keys.hpp>
#include <helper/audio/windows/winsound.hpp>
#include <helper/version/check.hpp>
#include <nlohmann/json.hpp>

namespace Soundux
{
    namespace traits
    {
        template <typename T> struct is_optional
        {
          private:
            static std::uint8_t test(...);
            template <typename O> static auto test(std::optional<O> *) -> std::uint16_t;

          public:
            static const bool value = sizeof(test(reinterpret_cast<std::decay_t<T> *>(0))) == sizeof(std::uint16_t);
        };
    } // namespace traits
} // namespace Soundux

namespace nlohmann
{
    template <typename T> struct adl_serializer<std::optional<T>>
    {
        static void to_json(json &j, const std::optional<T> &obj)
        {
            if (obj)
            {
                j = *obj;
            }
            else
            {
                j = nullptr;
            }
        }
        static void from_json(const json &j, const std::optional<T> &obj)
        {
            if (!j.is_null())
            {
                obj = j.get<T>();
            }
        }
    }; // namespace nlohmann
    template <> struct adl_serializer<Soundux::Objects::Key>
    {
        static void to_json(json &j, const Soundux::Objects::Key &obj)
        {
            j = {{"key", obj.key}, {"type", obj.type}};
        }
        static void from_json(const json &j, Soundux::Objects::Key &obj)
        {
            if (j.find("type") != j.end())
            {
                j.at("key").get_to(obj.key);
                j.at("type").get_to(obj.type);
            }
            else
            {
                j.get_to(obj.key);
                obj.type = Soundux::Enums::KeyType::Keyboard;
            }
        }
    }; // namespace nlohmann
    template <> struct adl_serializer<Soundux::Objects::Sound>
    {
        static void to_json(json &j, const Soundux::Objects::Sound &obj)
        {
            j = {
                {"name", obj.name},
                {"hotkeys", obj.hotkeys},
                {"hotkeySequence",
                 Soundux::Globals::gHotKeys->getKeySequence(obj.hotkeys)}, //* For frontend and config readability
                {"id", obj.id},
                {"path", obj.path},
                {"isFavorite", obj.isFavorite},
                {"localVolume", obj.localVolume},
                {"remoteVolume", obj.remoteVolume},
                {"modifiedDate", obj.modifiedDate},
            };
        }
        static void from_json(const json &j, Soundux::Objects::Sound &obj)
        {
            j.at("name").get_to(obj.name);
            j.at("hotkeys").get_to(obj.hotkeys);
            j.at("id").get_to(obj.id);
            j.at("path").get_to(obj.path);
            j.at("modifiedDate").get_to(obj.modifiedDate);
            if (j.find("isFavorite") != j.end())
            {
                j.at("isFavorite").get_to(obj.isFavorite);
            }
            if (j.find("localVolume") != j.end())
            {
                if (j.at("localVolume").is_number())
                {
                    obj.localVolume = j.at("localVolume").get<int>();
                }
            }
            if (j.find("remoteVolume") != j.end())
            {
                if (j.at("remoteVolume").is_number())
                {
                    obj.remoteVolume = j.at("remoteVolume").get<int>();
                }
            }
        }
    };
    template <> struct adl_serializer<Soundux::Objects::AudioDevice>
    {
        static void to_json(json &j, const Soundux::Objects::AudioDevice &obj)
        {
            j = {{"name", obj.name}, {"isDefault", obj.isDefault}};
        }
        static void from_json(const json &j, Soundux::Objects::AudioDevice &obj)
        {
            j.at("name").get_to(obj.name);
            j.at("isDefault").get_to(obj.isDefault);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::PlayingSound>
    {
        static void to_json(json &j, const Soundux::Objects::PlayingSound &obj)
        {
            j = {
                {"sound", obj.sound},           {"id", obj.id},
                {"length", obj.length},         {"paused", obj.paused.load()},
                {"lengthInMs", obj.lengthInMs}, {"repeat", obj.repeat.load()},
                {"readFrames", obj.readFrames}, {"readInMs", obj.readInMs.load()},
            };
        }
        static void from_json(const json &j, Soundux::Objects::PlayingSound &obj)
        {
            j.at("id").get_to(obj.id);
            j.at("sound").get_to(obj.sound);
            j.at("length").get_to(obj.length);
            j.at("readFrames").get_to(obj.readFrames);
            j.at("lengthInMs").get_to(obj.lengthInMs);

            obj.paused.store(j.at("paused").get<bool>());
            obj.repeat.store(j.at("repeat").get<bool>());
            obj.readInMs.store(j.at("readInMs").get<std::uint64_t>());
        }
    };
    template <> struct adl_serializer<Soundux::Objects::Settings>
    {
        static void to_json(json &j, const Soundux::Objects::Settings &obj)
        {
            j = {
                {"theme", obj.theme},
                {"outputs", obj.outputs},
                {"viewMode", obj.viewMode},
                {"language", obj.language},
                {"stopHotkey", obj.stopHotkey},
                {"syncVolumes", obj.syncVolumes},
                {"selectedTab", obj.selectedTab},
                {"localVolume", obj.localVolume},
                {"remoteVolume", obj.remoteVolume},
                {"audioBackend", obj.audioBackend},
                {"deleteToTrash", obj.deleteToTrash},
                {"pushToTalkKeys", obj.pushToTalkKeys},
                {"tabHotkeysOnly", obj.tabHotkeysOnly},
                {"minimizeToTray", obj.minimizeToTray},
                {"localVolumeKnob", obj.localVolumeKnob},
                {"remoteVolumeKnob", obj.remoteVolumeKnob},
                {"allowOverlapping", obj.allowOverlapping},
                {"muteDuringPlayback", obj.muteDuringPlayback},
                {"useAsDefaultDevice", obj.useAsDefaultDevice},
                {"allowMultipleOutputs", obj.allowMultipleOutputs},
            };
        }

        template <typename T> static void get_to_safe(const json &j, const std::string &key, T &member) noexcept
        {
            if (j.find(key) != j.end())
            {
                if constexpr (Soundux::traits::is_optional<T>::value)
                {
                    if (j.at(key).type_name() == nlohmann::basic_json(typename T::value_type{}).type_name())
                    {
                        if (!j.at(key).is_null())
                        {
                            member = j.at(key).get<typename T::value_type>();
                        }
                    }
                }
                else
                {
                    if (j.at(key).type_name() == nlohmann::basic_json(T{}).type_name())
                    {
                        j.at(key).get_to(member);
                    }
                }
            }
        }

        static void from_json(const json &j, Soundux::Objects::Settings &obj)
        {
            get_to_safe(j, "theme", obj.theme);
            get_to_safe(j, "outputs", obj.outputs);
            get_to_safe(j, "language", obj.language);
            get_to_safe(j, "viewMode", obj.viewMode);
            get_to_safe(j, "stopHotkey", obj.stopHotkey);
            get_to_safe(j, "localVolume", obj.localVolume);
            get_to_safe(j, "selectedTab", obj.selectedTab);
            get_to_safe(j, "syncVolumes", obj.syncVolumes);
            get_to_safe(j, "audioBackend", obj.audioBackend);
            get_to_safe(j, "remoteVolume", obj.remoteVolume);
            get_to_safe(j, "deleteToTrash", obj.deleteToTrash);
            get_to_safe(j, "pushToTalkKeys", obj.pushToTalkKeys);
            get_to_safe(j, "minimizeToTray", obj.minimizeToTray);
            get_to_safe(j, "tabHotkeysOnly", obj.tabHotkeysOnly);
            get_to_safe(j, "localVolumeKnob", obj.localVolumeKnob);
            get_to_safe(j, "remoteVolumeKnob", obj.remoteVolumeKnob);
            get_to_safe(j, "allowOverlapping", obj.allowOverlapping);
            get_to_safe(j, "useAsDefaultDevice", obj.useAsDefaultDevice);
            get_to_safe(j, "muteDuringPlayback", obj.muteDuringPlayback);
            get_to_safe(j, "allowMultipleOutputs", obj.allowMultipleOutputs);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::Tab>
    {
        static void to_json(json &j, const Soundux::Objects::Tab &obj)
        {
            j = {{"id", obj.id},
                 {"name", obj.name},
                 {"path", obj.path},
                 {"sounds", obj.sounds},
                 {"sortMode", obj.sortMode}};
        }
        static void from_json(const json &j, Soundux::Objects::Tab &obj)
        {
            j.at("id").get_to(obj.id);
            j.at("name").get_to(obj.name);
            j.at("path").get_to(obj.path);
            j.at("sounds").get_to(obj.sounds);

            if (j.find("sortMode") != j.end())
            {
                j.at("sortMode").get_to(obj.sortMode);
            }
        }
    };
    template <> struct adl_serializer<Soundux::Objects::Data>
    {
        static void to_json(json &j, const Soundux::Objects::Data &obj)
        {
            j = {{"height", obj.height},
                 {"width", obj.width},
                 {"tabs", obj.tabs},
                 {"soundIdCounter", obj.soundIdCounter}};
        }
        static void from_json(const json &j, Soundux::Objects::Data &obj)
        {
            j.at("soundIdCounter").get_to(obj.soundIdCounter);
            j.at("height").get_to(obj.height);
            j.at("width").get_to(obj.width);
            j.at("tabs").get_to(obj.tabs);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::Config>
    {
        static void to_json(json &j, const Soundux::Objects::Config &obj)
        {
            j = {{"data", obj.data}, {"settings", obj.settings}};
        }
        static void from_json(const json &j, Soundux::Objects::Config &obj)
        {
            j.at("data").get_to(obj.data);
            j.at("settings").get_to(obj.settings);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::VersionStatus>
    {
        static void to_json(json &j, const Soundux::Objects::VersionStatus &obj)
        {
            j = {{"current", obj.current}, {"latest", obj.latest}, {"outdated", obj.outdated}};
        }
        static void from_json(const json &j, Soundux::Objects::VersionStatus &obj)
        {
            j.at("latest").get_to(obj.latest);
            j.at("current").get_to(obj.current);
            j.at("outdated").get_to(obj.outdated);
        }
    };
#if defined(__linux__)
    template <> struct adl_serializer<std::shared_ptr<Soundux::Objects::IconRecordingApp>>
    {
        static void to_json(json &j, const std::shared_ptr<Soundux::Objects::IconRecordingApp> &obj)
        {
            j = {
                {"name", obj->name},
                {"appIcon", obj->appIcon},
                {"application", obj->application},
            };
        }
        static void from_json(const json &j, std::shared_ptr<Soundux::Objects::IconRecordingApp> &obj)
        {
            if (obj)
            {
                j.at("name").get_to(obj->name);
                j.at("appIcon").get_to(obj->appIcon);
                j.at("application").get_to(obj->application);
            }
        }
    };
    template <> struct adl_serializer<std::shared_ptr<Soundux::Objects::IconPlaybackApp>>
    {
        static void to_json(json &j, const std::shared_ptr<Soundux::Objects::IconPlaybackApp> &obj)
        {
            j = {
                {"name", obj->name},
                {"appIcon", obj->appIcon},
                {"application", obj->application},
            };
        }
        static void from_json(const json &j, std::shared_ptr<Soundux::Objects::IconPlaybackApp> &obj)
        {
            if (obj)
            {
                j.at("name").get_to(obj->name);
                j.at("appIcon").get_to(obj->appIcon);
                j.at("application").get_to(obj->application);
            }
        }
    };
#elif defined(_WIN32)
    template <> struct adl_serializer<Soundux::Objects::RecordingDevice>
    {
        static void to_json(json &j, const Soundux::Objects::RecordingDevice &obj)
        {
            j = {
                {"name", obj.getName()},
                {"guid", obj.getGUID()},
            };
        }
    };
#endif
} // namespace nlohmann
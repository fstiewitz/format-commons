#ifndef FORMAT_COMMONS_AUDIO_X_MIDI_HPP
#define FORMAT_COMMONS_AUDIO_X_MIDI_HPP

#include <format.hpp>
#include <utility>

namespace format::audio::x_midi {

    struct empty_sysex_message : public std::exception {
    };

    template<int V> using IntegralConstant = Constant<std::integral_constant<uint8_t, V>>;

    template<typename T, int S, int E>
    auto PackedValue(T in) {
        return Packed < T > {in}.template get<S, E>();
    }

    enum variables {
        STATUS_BYTE
    };

    enum message_type_t {
        NOTEOFF = 0b1000,
        NOTEON,
        POLYPHONICKEYPRESSURE,
        CONTROLCHANGE,
        PROGRAMCHANGE,
        CHANNELPRESSURE,
        PITCHWHEELCHANGE,
        SYSTEMMESSAGE
    };

    constexpr auto make_status_byte(unsigned type, unsigned channel) {
        return type << 4u | channel & 15u;
    }

    struct note_off_t {
        uint8_t key{0};
        uint8_t velocity{0};

        note_off_t(uint8_t k, uint8_t v) : key(k), velocity(v) {}

        note_off_t() = default;

        bool operator==(const note_off_t &other) const {
            return key == other.key && velocity == other.velocity;
        }
    };

    struct note_on_t {
        uint8_t key;
        uint8_t velocity;

        note_on_t(uint8_t k, uint8_t v) : key(k), velocity(v) {}

        note_on_t() = default;

        bool operator==(const note_on_t &other) const {
            return key == other.key && velocity == other.velocity;
        }
    };

    struct polyphonic_key_pressure_t {
        uint8_t key;
        uint8_t velocity;

        polyphonic_key_pressure_t(uint8_t k, uint8_t v) : key(k), velocity(v) {}

        polyphonic_key_pressure_t() = default;
    };

    struct control_change_t {
        uint8_t controller;
        uint8_t value;

        control_change_t(uint8_t c, uint8_t v) : controller(c), value(v) {}

        control_change_t() = default;

        bool operator==(const control_change_t &other) const {
            return controller == other.controller && value == other.value;
        }
    };

    struct program_change_t {
        uint8_t program_number;

        explicit program_change_t(uint8_t p) : program_number(p) {}

        program_change_t() = default;

        bool operator==(const program_change_t &other) const {
            return program_number == other.program_number;
        }
    };

    struct channel_pressure_t {
        uint8_t pressure;

        explicit channel_pressure_t(uint8_t p) : pressure(p) {}

        channel_pressure_t() = default;
    };

    struct pitch_wheel_change_t {
        uint16_t pitch_wheel;

        explicit pitch_wheel_change_t(uint8_t l, uint8_t m) : pitch_wheel((m << 8u) | l) {}

        pitch_wheel_change_t() = default;

        [[nodiscard]] uint8_t lsb() const {
            return pitch_wheel & 0xffu;
        }

        [[nodiscard]] uint8_t msb() const {
            return pitch_wheel >> 8u;
        }
    };

    struct song_position_pointer_t {
        uint16_t song_position;

        explicit song_position_pointer_t(uint8_t l, uint8_t m) : song_position((m << 8u) | l) {}

        song_position_pointer_t() = default;

        [[nodiscard]] uint8_t lsb() const {
            return song_position & 0xffu;
        }

        [[nodiscard]] uint8_t msb() const {
            return song_position >> 8u;
        }
    };

    struct song_select_t {
        uint8_t song_select;

        explicit song_select_t(uint8_t s) : song_select(s) {}

        song_select_t() = default;
    };

    struct sysex_message_t {
        uint8_t id {0};
        std::string message;

        sysex_message_t(uint8_t id, std::string r): id(id), message(std::move(r)) {}
        sysex_message_t(std::initializer_list<uint8_t> l): id(*l.begin()), message(l.begin() + 1, l.end()) {}

        sysex_message_t() = default;

        bool operator==(const sysex_message_t &other) const {
            return id == other.id && message == other.message;
        }
    };

    using system_message_t = std::variant<sysex_message_t, song_position_pointer_t, song_select_t>;

    struct midi_message_t {
        uint8_t status{};
        std::variant<note_off_t, note_on_t, polyphonic_key_pressure_t, control_change_t, program_change_t, channel_pressure_t, pitch_wheel_change_t, std::variant<sysex_message_t, song_position_pointer_t, song_select_t>> message{};

        template<typename T>
        midi_message_t(uint8_t s, T &&v): status(s), message(std::forward<T>(v)) {}

        midi_message_t() = default;
    };

    using NoteOff = Structure <note_off_t, O<offsetof(note_off_t, key), Sc < uint8_t>>, O<offsetof(note_off_t,
                                                                                                   velocity), Sc<uint8_t>>>;
    using NoteOn = Structure <note_on_t, O<offsetof(note_on_t, key), Sc < uint8_t>>, O<offsetof(note_on_t,
                                                                                                velocity), Sc<uint8_t>>>;
    using PolyphonicKeyPressure = Structure <polyphonic_key_pressure_t, O<offsetof(polyphonic_key_pressure_t, key),
            Sc < uint8_t>>, O<offsetof(polyphonic_key_pressure_t, velocity), Sc<uint8_t>>>;
    using ControlChange = Structure <control_change_t, O<offsetof(control_change_t, controller),
            Sc < uint8_t>>, O<offsetof(control_change_t, value), Sc<uint8_t>>>;
    using ProgramChange = Structure <program_change_t, O<offsetof(program_change_t, program_number), Sc < uint8_t>>>;
    using ChannelPressure = Structure <channel_pressure_t, O<offsetof(channel_pressure_t, pressure), Sc < uint8_t>>>;
    using PitchWheelChange = Structure <pitch_wheel_change_t, Acc<&pitch_wheel_change_t::lsb,
            Sc < uint8_t>>, Acc<&pitch_wheel_change_t::msb, Sc<uint8_t>>>;
    using SongPositionPointer = Structure <song_position_pointer_t, Acc<&song_position_pointer_t::lsb,
            Sc < uint8_t>>, Acc<&song_position_pointer_t::msb, Sc<uint8_t>>>;
    using SongSelect = Structure <song_select_t, O<offsetof(song_select_t, song_select), Sc < uint8_t>>>;

    auto vectorToSysEx(const std::vector<uint8_t> &input) {
        std::string ret{};
        if (input.empty()) throw empty_sysex_message{};
        for (auto it = input.cbegin() + 1; it != input.cend(); ++it) {
            uint8_t c = *it;
            if (c & 128u) continue; // ignore real-time in sysex (for now)
            ret.push_back(c);
        }
        return sysex_message_t{static_cast<uint8_t>(input[0]), ret};
    }

    auto sysExToVector(const sysex_message_t &in) {
        std::vector<uint8_t> out{};
        out.resize(1 + in.message.size());
        out[0] = in.id;
        memcpy(out.data() + 1, in.message.data(), in.message.size());
        return out;
    }

    using SystemExclusiveMessage = Map<&vectorToSysEx, &sysExToVector, TerminatedArray<Sc < uint8_t>, 0b11110111>>;

    using SystemMessage = Switch <Call<&PackedValue<uint8_t, 0, 3>, Get < STATUS_BYTE>>,
    Case <IntegralConstant<0b0000>, SystemExclusiveMessage>,
    Case <IntegralConstant<0b0010>, SongPositionPointer>,
    Case <IntegralConstant<0b0011>, SongSelect>>;

    using StatusByte = Copy <STATUS_BYTE, Sc<uint8_t>>;
    using RemainingMidiMessage =
    Switch <Call<&PackedValue<uint8_t, 4, 7>, Get < STATUS_BYTE>>,
    Case <IntegralConstant<NOTEOFF>, NoteOff>,
    Case <IntegralConstant<NOTEON>, NoteOn>,
    Case <IntegralConstant<POLYPHONICKEYPRESSURE>, PolyphonicKeyPressure>,
    Case <IntegralConstant<CONTROLCHANGE>, ControlChange>,
    Case <IntegralConstant<PROGRAMCHANGE>, ProgramChange>,
    Case <IntegralConstant<CHANNELPRESSURE>, ChannelPressure>,
    Case <IntegralConstant<PITCHWHEELCHANGE>, PitchWheelChange>,
    Case <IntegralConstant<SYSTEMMESSAGE>, SystemMessage>>;

    using MidiMessage = Structure <midi_message_t, O<offsetof(midi_message_t, status), StatusByte>, O<offsetof(
            midi_message_t, message), RemainingMidiMessage>>;

    enum controller_t {
        BANK_SELECT_MSB = 0x00,
        MODULATION_WHEEL_MSB,
        BREATH_CONTROL_MSB,
        UNDEFINED_MSB_4,
        FOOT_CONTROLLER_MSB,
        PORTAMENTO_TIME_MSB,
        DATA_ENTRY_MSB,
        CHANNEL_VOLUME_FORMERLY_MAIN_VOLUME_MSB,
        BALANCE_MSB,
        UNDEFINED_MSB_10,
        PAN_MSB,
        EXPRESSION_CONTROLLER_MSB,
        EFFECT_CONTROL_1_MSB,
        EFFECT_CONTROL_2_MSB,
        UNDEFINED_MSB_15,
        UNDEFINED_MSB_16,
        GENERAL_PURPOSE_CONTROLLER_1_MSB,
        GENERAL_PURPOSE_CONTROLLER_2_MSB,
        GENERAL_PURPOSE_CONTROLLER_3_MSB,
        GENERAL_PURPOSE_CONTROLLER_4_MSB,
        UNDEFINED_MSB_21,
        UNDEFINED_MSB_22,
        UNDEFINED_MSB_23,
        UNDEFINED_MSB_24,
        UNDEFINED_MSB_25,
        UNDEFINED_MSB_26,
        UNDEFINED_MSB_27,
        UNDEFINED_MSB_28,
        UNDEFINED_MSB_29,
        UNDEFINED_MSB_30,
        UNDEFINED_MSB_31,
        UNDEFINED_MSB_32,
        BANK_SELECT_LSB,
        MODULATION_WHEEL_LSB,
        BREATH_CONTROL_LSB,
        UNDEFINED_LSB_36,
        FOOT_CONTROLLER_LSB,
        PORTAMENTO_TIME_LSB,
        DATA_ENTRY_LSB,
        CHANNEL_VOLUME_FORMERLY_MAIN_VOLUME_LSB,
        BALANCE_LSB,
        UNDEFINED_LSB_42,
        PAN_LSB,
        EXPRESSION_CONTROLLER_LSB,
        EFFECT_CONTROL_1_LSB,
        EFFECT_CONTROL_2_LSB,
        UNDEFINED_LSB_47,
        UNDEFINED_LSB_48,
        GENERAL_PURPOSE_CONTROLLER_1_LSB,
        GENERAL_PURPOSE_CONTROLLER_2_LSB,
        GENERAL_PURPOSE_CONTROLLER_3_LSB,
        GENERAL_PURPOSE_CONTROLLER_4_LSB,
        UNDEFINED_LSB_53,
        UNDEFINED_LSB_54,
        UNDEFINED_LSB_55,
        UNDEFINED_LSB_56,
        UNDEFINED_LSB_57,
        UNDEFINED_LSB_58,
        UNDEFINED_LSB_59,
        UNDEFINED_LSB_60,
        UNDEFINED_LSB_61,
        UNDEFINED_LSB_62,
        UNDEFINED_LSB_63,
        UNDEFINED_LSB_64,
        DAMPER_PEDAL_ON_OFF_SUSTAIN,
        PORTAMENTO_ON_OFF,
        SUSTENUTO_ON_OFF,
        SOFT_PEDAL_ON_OFF,
        LEGATO_FOOTSWITCH,
        HOLD_2,
        SOUND_CONTROLLER_1_SOUND_VARIATION_LSB,
        SOUND_CONTROLLER_2_TIMBRE_LSB,
        SOUND_CONTROLLER_3_RELEASE_TIME_LSB,
        SOUND_CONTROLLER_4_ATTACK_TIME_LSB,
        SOUND_CONTROLLER_5_BRIGHTNESS_LSB,
        SOUND_CONTROLLER_6_LSB,
        SOUND_CONTROLLER_7_LSB,
        SOUND_CONTROLLER_8_LSB,
        SOUND_CONTROLLER_9_LSB,
        SOUND_CONTROLLER_10_LSB,
        GENERAL_PURPOSE_CONTROLLER_5_LSB,
        GENERAL_PURPOSE_CONTROLLER_6_LSB,
        GENERAL_PURPOSE_CONTROLLER_7_LSB,
        GENERAL_PURPOSE_CONTROLLER_8_LSB,
        PORTAMENTO_CONTROL,
        UNDEFINED_LSB_86,
        UNDEFINED_LSB_87,
        UNDEFINED_LSB_88,
        UNDEFINED_LSB_89,
        UNDEFINED_LSB_90,
        UNDEFINED_LSB_91,
        EFFECTS_1_DEPTH_LSB,
        EFFECTS_2_DEPTH_LSB,
        EFFECTS_3_DEPTH_LSB,
        EFFECTS_4_DEPTH_LSB,
        EFFECTS_5_DEPTH_LSB,
        DATA_ENTRY_PLUS_1,
        DATA_ENTRY_MINUS_1,
        NON_REGISTERED_PARAMETER_NUMBER_LSB,
        NON_REGISTERED_PARAMETER_NUMBER_MSB,
        REGISTERED_PARAMETER_NUMBER_LSB,
        REGISTERED_PARAMETER_NUMBER_MSB,
        UNDEFINED_103,
        UNDEFINED_104,
        UNDEFINED_105,
        UNDEFINED_106,
        UNDEFINED_107,
        UNDEFINED_108,
        UNDEFINED_109,
        UNDEFINED_110,
        UNDEFINED_111,
        UNDEFINED_112,
        UNDEFINED_113,
        UNDEFINED_114,
        UNDEFINED_115,
        UNDEFINED_116,
        UNDEFINED_117,
        UNDEFINED_118,
        UNDEFINED_119,
        UNDEFINED_120,
        ALL_SOUND_OFF,
        RESET_ALL_CONTROLLERS,
        LOCAL_CONTROL_ON_OFF,
        ALL_NOTES_OFF,
        OMNI_MODE_OFF_ALL_NOTES_OFF,
        OMNI_MODE_ON_ALL_NOTES_OFF,
        POLY_MODE_ON_OFF_ALL_NOTES_OFF,
    };

    struct controller_state {
        uint16_t states[128];

        uint16_t get(uint8_t message_type) const {
            if (message_type >= 32u && message_type <= 63u) return states[message_type - 32u];
            else return states[message_type];
        }

        void apply(uint8_t message_type, uint8_t value) {
            if (message_type <= 31u) {
                states[message_type] = states[message_type] & 0xFFu | value << 8u;
            } else if (message_type <= 63u) {
                states[message_type - 32u] = states[message_type - 32u] & 0xFF00u | value;
            } else if (message_type <= 69u) {
                states[message_type] = value >= 64u;
            } else if (message_type == 122) {
                states[message_type] = value >= 127u;
            } else {
                states[message_type] = value;
            }
        }
    };

}

#endif //FORMAT_COMMONS_AUDIO_X_MIDI_HPP

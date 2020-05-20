#ifndef FORMAT_COMMONS_AUDIO_X_MIDI_HPP
#define FORMAT_COMMONS_AUDIO_X_MIDI_HPP

#include <format.hpp>

namespace format::audio::x_midi {

struct empty_sysex_message: public std::exception {};

    template<int V> using IntegralConstant = Constant<std::integral_constant<uint8_t, V>>;

    template<typename T, int S, int E>
    auto PackedValue(T in) {
        return Packed<T>{in}.template get<S, E>();
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
        uint8_t key {0};
        uint8_t velocity {0};

        note_off_t(uint8_t k, uint8_t v): key(k), velocity(v) {}
        note_off_t() = default;
        bool operator==(const note_off_t &other) const {
            return key == other.key && velocity == other.velocity;
        }
    };

    struct note_on_t {
        uint8_t key;
        uint8_t velocity;

        note_on_t(uint8_t k, uint8_t v): key(k), velocity(v) {}
        note_on_t() = default;
        bool operator==(const note_on_t &other) const {
            return key == other.key && velocity == other.velocity;
        }
    };

    struct polyphonic_key_pressure_t {
        uint8_t key;
        uint8_t velocity;

        polyphonic_key_pressure_t(uint8_t k, uint8_t v): key(k), velocity(v) {}
        polyphonic_key_pressure_t() = default;
    };

    struct control_change_t {
        uint8_t controller;
        uint8_t value;

        control_change_t(uint8_t c, uint8_t v): controller(c), value(v) {}
        control_change_t() = default;
    };

    struct program_change_t {
        uint8_t program_number;

        explicit program_change_t(uint8_t p): program_number(p) {}
        program_change_t() = default;
    };

    struct channel_pressure_t {
        uint8_t pressure;

        explicit channel_pressure_t(uint8_t p): pressure(p) {}
        channel_pressure_t() = default;
    };

    struct pitch_wheel_change_t {
        uint16_t pitch_wheel;

        explicit pitch_wheel_change_t(uint8_t l, uint8_t m): pitch_wheel((m << 8u) | l) {}
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

        explicit song_position_pointer_t(uint8_t l, uint8_t m): song_position((m << 8u) | l) {}
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

        explicit song_select_t(uint8_t s): song_select(s) {}
        song_select_t() = default;
    };

    struct sysex_message_t {
        uint8_t id;
        std::string message;
    };

    struct midi_message_t {
        uint8_t status {};
        std::variant<note_off_t, note_on_t, polyphonic_key_pressure_t, control_change_t, program_change_t, channel_pressure_t, pitch_wheel_change_t, std::variant<sysex_message_t, song_position_pointer_t, song_select_t>> message {};

        template<typename T>
        midi_message_t(uint8_t s, T &&v): status(s), message(std::forward<T>(v)) {}
        midi_message_t() = default;
    };

    using NoteOff = Structure<note_off_t, O<offsetof(note_off_t, key), Sc<uint8_t>>, O<offsetof(note_off_t, velocity), Sc<uint8_t>>>;
    using NoteOn = Structure<note_on_t, O<offsetof(note_on_t, key), Sc<uint8_t>>, O<offsetof(note_on_t, velocity), Sc<uint8_t>>>;
    using PolyphonicKeyPressure = Structure<polyphonic_key_pressure_t, O<offsetof(polyphonic_key_pressure_t, key), Sc<uint8_t>>, O<offsetof(polyphonic_key_pressure_t, velocity), Sc<uint8_t>>>;
    using ControlChange = Structure<control_change_t, O<offsetof(control_change_t, controller), Sc<uint8_t>>, O<offsetof(control_change_t, value), Sc<uint8_t>>>;
    using ProgramChange = Structure<program_change_t, O<offsetof(program_change_t, program_number), Sc<uint8_t>>>;
    using ChannelPressure = Structure<channel_pressure_t, O<offsetof(channel_pressure_t, pressure), Sc<uint8_t>>>;
    using PitchWheelChange = Structure<pitch_wheel_change_t, Acc<&pitch_wheel_change_t::lsb, Sc<uint8_t>>, Acc<&pitch_wheel_change_t::msb, Sc<uint8_t>>>;
    using SongPositionPointer = Structure<song_position_pointer_t, Acc<&song_position_pointer_t::lsb, Sc<uint8_t>>, Acc<&song_position_pointer_t::msb, Sc<uint8_t>>>;
    using SongSelect = Structure<song_select_t, O<offsetof(song_select_t, song_select), Sc<uint8_t>>>;

    auto vectorToSysEx(const std::vector<uint8_t> &input) {
        std::string ret {};
        if(input.empty()) throw empty_sysex_message {};
        for(auto it = input.cbegin() + 1; it != input.cend(); ++it) {
            uint8_t c = *it;
            if(c & 128u) continue; // ignore real-time in sysex (for now)
            ret.push_back(c);
        }
        return sysex_message_t {static_cast<uint8_t>(input[0]), ret};
    }

    auto sysExToVector(const sysex_message_t &in) {
        std::vector<uint8_t> out {};
        out.resize(1 + in.message.size());
        out[0] = in.id;
        memcpy(out.data() + 1, in.message.data(), in.message.size());
        return out;
    }

    using SystemExclusiveMessage = Map<&vectorToSysEx, &sysExToVector, TerminatedArray<Sc<uint8_t>, 0b11110111>>;

    using SystemMessage = Switch<Call<&PackedValue<uint8_t, 0, 3>, Get<STATUS_BYTE>>,
    Case<IntegralConstant<0b0000>, SystemExclusiveMessage>,
    Case<IntegralConstant<0b0010>, SongPositionPointer>,
    Case<IntegralConstant<0b0011>, SongSelect>>;

    using StatusByte = Copy <STATUS_BYTE, Sc<uint8_t>>;
    using RemainingMidiMessage =
    Switch<Call<&PackedValue<uint8_t, 4, 7>, Get<STATUS_BYTE>>,
    Case <IntegralConstant<NOTEOFF>, NoteOff>,
    Case <IntegralConstant<NOTEON>, NoteOn>,
    Case <IntegralConstant<POLYPHONICKEYPRESSURE>, PolyphonicKeyPressure>,
    Case <IntegralConstant<CONTROLCHANGE>, ControlChange>,
    Case <IntegralConstant<PROGRAMCHANGE>, ProgramChange>,
    Case <IntegralConstant<CHANNELPRESSURE>, ChannelPressure>,
    Case <IntegralConstant<PITCHWHEELCHANGE>, PitchWheelChange>,
    Case <IntegralConstant<SYSTEMMESSAGE>, SystemMessage>>;

    using MidiMessage = Structure<midi_message_t, O<offsetof(midi_message_t, status), StatusByte>, O<offsetof(midi_message_t, message), RemainingMidiMessage>>;

}

#endif //FORMAT_COMMONS_AUDIO_X_MIDI_HPP

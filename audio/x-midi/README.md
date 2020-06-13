## format-commons/audio/x-midi

Implementation of a MIDI message reader/writer using [format.hpp](https://github.com/fstiewitz/format.hpp).

### Usage

Add `format.hpp` to your include paths.

```c++
#include <format.hpp>
#include <format-commons/audio/x-midi.hpp>

using namespace format;
using namespace format::audio::x_midi;

int main() {
    while(!std::cin.eof()) {
        midi_message_t message;

        try {
            Format<MidiMessage>::reader(std::cin).read(message);
            const auto type = status_get_type(message.status);

            if(type == NOTEON) {
                const auto channel = status_get_channel(message.status);
                auto noteon = std::get<note_on_t>(message.message);
                std::cout << note_to_str_c_major(noteon.key) << std::endl;
                // ...
            } else if(type == NOTEOFF) {
                const auto channel = status_get_channel(message.status);
                auto noteoff = std::get<note_off_t>(message.message);
                std::cout << note_to_str_c_major(noteoff.key) << std::endl;
                // ...
            }
        } catch(binary_eof &) {}
    }
}
```

### Types

The main structure, `midi_message_t` stores a status byte and an `std::variant` of all possible message types. These types are:

    note_off_t(key, velocity)
    note_on_t(key, velocity)
    polyphonic_key_pressure_t(key, velocity)
    control_change_t(controller, value)
    program_change_t(program_number)
    channel_pressure_t(pressure)
    pitch_wheel_change_t(pitch_wheel)
    system_message_t

`system_message_t` is an `std::variant` of:

    sysex_message_t(id, message)
    song_position_pointer_t(song_position)
    song_select_t(song_select)

### Utilities

For reading and writing the status byte:

    make_status_byte(type, channel)
    status_get_type(status_byte)
    status_get_channel(status_byte)

For controller messages:

    enum controller_t
    const char* controller_names[]

Additionally, a `struct controller_state` can be used to keep track of controller changes:

```c++
struct controller_state {
    // value for composites is stored in the MSB slot (e.g. BANK_SELECT_MSB)
    uint16_t get(uint8_t message_type) const;
    // has to be called manually
    void apply(uint8_t message_type, uint8_t value);
    // called for every message. value represents "full" value (MSB + LSB, if necessary)
    virtual void controller_changed(controller_t controller, uint16_t value);
};
```

For notes:

    note_to_str_c_major(note)

Instruments:

    enum gm_instrument_family
    gm_instrument_family_from_program(program)
    enum gm_instruments
    const char* gm_instrument_names[]
    enum gm_percussion_key_map
    const char* gm_percussion_key_map_names[]
    gm_percussion_from_note(note)

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

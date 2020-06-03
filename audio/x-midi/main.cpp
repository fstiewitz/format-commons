#include <format.hpp>
#include <format-commons/audio/x-midi.hpp>

using namespace format;
using namespace format::audio::x_midi;

#define NOTEOFFFORMAT               "key off                 (channel %i)"
#define NOTEOFFZEROFORMAT           "key off (velocity = 0)  (channel %i)"
#define NOTEONFORMAT                "key on                  (channel %i)"
#define POLYPHONICKEYPRESSUREFORMAT "polyphonic key pressure (channel %i)"
#define CONTROLCHANGEFORMAT         "control change          (channel %i)"
#define PROGRAMCHANGEFORMAT         "program change          (channel %i)"
#define CHANNELPRESSUREFORMAT       "channel pressure format (channel %i)"
#define PITCHWHEELCHANGEFORMAT      "pitch wheel change      (channel %i)"
#define SYSEXFORMAT                 "sysex message           (id      %02x)"
#define SONGPOSITIONFORMAT          "song position                       "
#define SONGSELECTFORMAT            "song select                         "
#define UNDEFINEDFORMAT             "undefined                           "

int main() {
    using F = Format<MidiMessage>;
    midi_message_t message;

    try {
        while (!std::cin.eof()) {
            F::reader(std::cin).read(message);
            const auto channel = status_get_channel(message.status);
            const auto type = status_get_type(message.status);
            if (channel == 10 && type == NOTEOFF) {
                auto d = std::get<note_off_t>(message.message);
                printf(NOTEOFFFORMAT, channel);
                printf(": percussion %s vel %u\n", gm_percussion_key_map_names[gm_percussion_from_note(d.key)], d.velocity);
            } else if (channel == 10 && type == NOTEON) {
                auto d = std::get<note_off_t>(message.message);
                if(d.velocity == 0) printf(NOTEOFFZEROFORMAT, channel);
                else printf(NOTEONFORMAT, channel);
                printf(": percussion %s vel %u\n", gm_percussion_key_map_names[gm_percussion_from_note(d.key)], d.velocity);
            } else if (type == NOTEOFF) {
                auto d = std::get<note_off_t>(message.message);
                printf(NOTEOFFFORMAT, channel);
                printf(": %s vel %u\n", note_to_str_c_major(d.key).c_str(), d.velocity);
            } else if (type == NOTEON) {
                auto d = std::get<note_on_t>(message.message);
                if(d.velocity == 0) printf(NOTEOFFZEROFORMAT, channel);
                else printf(NOTEONFORMAT, channel);
                printf(": %s vel %u\n", note_to_str_c_major(d.key).c_str(), d.velocity);
            } else if (type == POLYPHONICKEYPRESSURE) {
                auto d = std::get<polyphonic_key_pressure_t>(message.message);
                printf(POLYPHONICKEYPRESSUREFORMAT, channel);
                printf(": %s vel %u\n", note_to_str_c_major(d.key).c_str(), d.velocity);
            } else if (type == CONTROLCHANGE) {
                auto d = std::get<control_change_t>(message.message);
                printf(CONTROLCHANGEFORMAT, channel);
                if (d.controller < 128) {
                    if(d.controller >= 64 && d.controller <= 69) {
                        if(d.value < 63) printf(": %s %s\n", controller_names[d.controller], "off");
                        else if(d.value > 64) printf(": %s %s\n", controller_names[d.controller], "on");
                        else printf(": %s %u\n", controller_names[d.controller], d.value);
                    } else if(d.controller == 122) {
                        if(d.controller == 0) printf(": %s %s\n", controller_names[d.controller], "off");
                        else if(d.controller == 0) printf(": %s %s\n", controller_names[d.controller], "on");
                        else printf(": %s %s\n", controller_names[d.controller], "true (non-standard value)");
                    } else if(d.controller == 84) {
                        printf(": %s key %s\n", controller_names[d.controller], note_to_str_c_major(d.value).c_str());
                    } else {
                        printf(": %s val %u\n", controller_names[d.controller], d.value);
                    }
                } else {
                    printf(": undefined controller %u\n", d.controller);
                }
            } else if (type == PROGRAMCHANGE) {
                auto d = std::get<program_change_t>(message.message);
                printf(PROGRAMCHANGEFORMAT, channel);
                if(d.program_number < 128) {
                    printf(": %s\n", gm_instrument_names[d.program_number]);
                } else {
                    printf(": undefined instrument %u\n", d.program_number);
                }
            } else if (type == CHANNELPRESSURE) {
                auto d = std::get<channel_pressure_t>(message.message);
                printf(CHANNELPRESSUREFORMAT, channel);
                printf(": vel %u\n", d.pressure);
            } else if (type == PITCHWHEELCHANGE) {
                auto d = std::get<pitch_wheel_change_t>(message.message);
                printf(PITCHWHEELCHANGEFORMAT, channel);
                printf(": val %u\n", d.pitch_wheel);
            } else if (type == SYSTEMMESSAGE) {
                auto d = std::get<system_message_t>(message.message);
                sysex_message_t sysex_m;
                switch(channel) {
                    case SYSEX_MESSAGE:
                        sysex_m = std::get<sysex_message_t>(d);
                        printf(SYSEXFORMAT, sysex_m.id);
                        printf(":");
                        for(uint8_t b : sysex_m.message) {
                            printf(" %02x", b);
                        }
                        printf("\n");
                        break;
                    case SONG_POSITION_POINTER:
                        printf(SONGPOSITIONFORMAT);
                        printf(": beats %u\n", std::get<song_position_pointer_t>(d).song_position);
                        break;
                    case SONG_SELECT:
                        printf(SONGSELECTFORMAT);
                        printf(": selection %u\n", std::get<song_select_t>(d).song_select);
                        break;
                    case TUNE_REQUEST:
                        printf("tune request\n");
                        break;
                    case END_OF_EXCLUSIVE:
                        printf("end of exclusive\n");
                        break;
                    case CONTINUE:
                        printf("continue\n");
                        break;
                    case STOP:
                        printf("stop\n");
                        break;
                    case ACTIVE_SENSING:
                        printf("active sensing\n");
                        break;
                    case RESET:
                        printf("reset\n");
                        break;
                    default:
                        printf(UNDEFINEDFORMAT);
                        printf("\n");
                        break;
                }
            }
        }
    } catch (no_matching_case &c) {
        std::cout << "unknown input (no_matching_case): " << std::to_string(std::any_cast<uint8_t>(c._val))
                  << std::endl;
    } catch (binary_eof &) {
        std::cout << "input stream closed" << std::endl;
    }
}

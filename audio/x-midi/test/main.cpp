#include <format.hpp>
#include <format-commons/audio/x-midi.hpp>

#include <fstream>
#include <sstream>
#include <cassert>

using namespace format;
using namespace format::audio::x_midi;

#define TEST(x) fprintf(stderr, "Test %2i: %s\n", tc++, x)

template<typename T>
auto get_file(T &&p) {
    return std::ifstream(std::string("fixtures/") + std::forward<T>(p) + ".syx", std::ios_base::in | std::ios_base::binary);
}

int main(int argc, char** argv) {
    int tc = 1;
    using F = Format<MidiMessage>;

    TEST("Single Key Press");
    {
        midi_message_t message;
        std::stringbuf fd;
        get_file("test0") >> &fd;

        std::stringstream sd;
        sd.str(fd.str());
        F::reader(sd).read(message);

        assert(message.status == make_status_byte(NOTEON, 0));
        assert(std::holds_alternative<note_on_t>(message.message));
        assert(std::get<note_on_t>(message.message) == note_on_t(59u, 58u));

        sd.clear();
        sd.str("");
        F::writer(sd).write(message);

        assert(sd.str() == fd.str());
    }
    TEST("Single Key Release");
    {
        midi_message_t message;
        std::stringbuf fd;
        get_file("test1") >> &fd;

        std::stringstream sd;
        sd.str(fd.str());
        F::reader(sd).read(message);

        assert(message.status == make_status_byte(NOTEOFF, 0));
        assert(std::holds_alternative<note_off_t>(message.message));
        assert(std::get<note_off_t>(message.message) == note_off_t(69u, 0u));

        sd.clear();
        sd.str("");
        F::writer(sd).write(message);

        assert(sd.str() == fd.str());
    }
    TEST("Pedal Press");
    {
        midi_message_t message;
        std::stringbuf fd;
        get_file("test2") >> &fd;

        std::stringstream sd;
        sd.str(fd.str());
        F::reader(sd).read(message);

        assert(message.status == make_status_byte(CONTROLCHANGE, 0));
        assert(std::holds_alternative<control_change_t>(message.message));
        assert(std::get<control_change_t>(message.message) == control_change_t(DAMPER_PEDAL_ON_OFF_SUSTAIN, 127u));

        sd.clear();
        sd.str("");
        F::writer(sd).write(message);

        assert(sd.str() == fd.str());
    }
    TEST("Pedal Release");
    {
        midi_message_t message;
        std::stringbuf fd;
        get_file("test3") >> &fd;

        std::stringstream sd;
        sd.str(fd.str());
        F::reader(sd).read(message);

        assert(message.status == make_status_byte(CONTROLCHANGE, 0));
        assert(std::holds_alternative<control_change_t>(message.message));
        assert(std::get<control_change_t>(message.message) == control_change_t(DAMPER_PEDAL_ON_OFF_SUSTAIN, 0u));

        sd.clear();
        sd.str("");
        F::writer(sd).write(message);

        assert(sd.str() == fd.str());
    }
    TEST("Voice Change (pre-recorded from own piano)");
    {
        midi_message_t message;
        std::stringbuf fd;
        get_file("test4") >> &fd;

        std::stringstream sd;
        sd.str(fd.str());

        F::reader(sd).read(message);
        assert(message.status == make_status_byte(CONTROLCHANGE, 0));
        assert(std::holds_alternative<control_change_t>(message.message));
        assert(std::get<control_change_t>(message.message) == control_change_t(BANK_SELECT_MSB, 0u));

        F::reader(sd).read(message);
        assert(message.status == make_status_byte(CONTROLCHANGE, 0));
        assert(std::holds_alternative<control_change_t>(message.message));
        assert(std::get<control_change_t>(message.message) == control_change_t(BANK_SELECT_LSB, 122u));

        F::reader(sd).read(message);
        assert(message.status == make_status_byte(PROGRAMCHANGE, 0));
        assert(std::holds_alternative<program_change_t>(message.message));
        assert(std::get<program_change_t>(message.message) == program_change_t(5u));

        // according to my manual, this sets the reverb type
        F::reader(sd).read(message);
        assert(message.status == make_status_byte(SYSTEMMESSAGE, 0));
        assert(std::holds_alternative<system_message_t>(message.message));
        assert(std::holds_alternative<sysex_message_t>(std::get<system_message_t>(message.message)));
        assert(std::get<sysex_message_t>(std::get<system_message_t>(message.message)) == sysex_message_t({0x43, 0x10, 0x4c, 0x02, 0x01, 0x00, 0x03, 0x10}));

        F::reader(sd).read(message);
        assert(message.status == make_status_byte(CONTROLCHANGE, 0));
        assert(std::holds_alternative<control_change_t>(message.message));
        assert(std::get<control_change_t>(message.message) == control_change_t(EFFECTS_1_DEPTH_LSB, 20u));

        // according to my manual, this sets the chorus type
        F::reader(sd).read(message);
        assert(message.status == make_status_byte(SYSTEMMESSAGE, 0));
        assert(std::holds_alternative<system_message_t>(message.message));
        assert(std::holds_alternative<sysex_message_t>(std::get<system_message_t>(message.message)));
        assert(std::get<sysex_message_t>(std::get<system_message_t>(message.message)) == sysex_message_t({0x43, 0x10, 0x4c, 0x02, 0x01, 0x20, 0x41, 0x08}));

        F::reader(sd).read(message);
        assert(message.status == make_status_byte(CONTROLCHANGE, 0));
        assert(std::holds_alternative<control_change_t>(message.message));
        assert(std::get<control_change_t>(message.message) == control_change_t(EFFECTS_3_DEPTH_LSB, 25u));
    }
    return 0;
}

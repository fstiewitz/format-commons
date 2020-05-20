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
    return 0;
}

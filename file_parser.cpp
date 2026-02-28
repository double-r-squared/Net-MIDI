#include "file_parser.h"
#include <fstream>
#include <iostream>
#include <cmath>

std::vector<ParsedEvent> FileParser::parseFile(
    const std::string& input_file,
    uint8_t octave_base,
    uint32_t duration_ticks
) {
    std::vector<ParsedEvent> events;

    std::ifstream file(input_file, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Couldn't open file " << input_file << '\n';
        return events;
    }

    std::cout << "Parsing file: " << input_file << '\n';

    uint8_t byte_value;
    int byte_count = 0;

    // Read file byte by byte
    while (file.read(reinterpret_cast<char*>(&byte_value), sizeof(uint8_t))) {
        byte_count++;

        ParsedEvent event;
        event.raw_value = byte_value;
        event.midi_note = rawToMidiNote(byte_value, octave_base);
        event.velocity = rawToVelocity(byte_value);
        event.duration_ticks = duration_ticks;

        events.push_back(event);

        // Debug: print every 100th byte
        if (byte_count % 100 == 0) {
            std::cout << "Processed " << byte_count << " bytes...\n";
        }
    }

    file.close();

    std::cout << "Parsing complete. Extracted " << events.size() << " MIDI events.\n";

    // Print some sample events
    if (!events.empty()) {
        std::cout << "\nSample events (first 10):\n";
        for (size_t i = 0; i < std::min(size_t(10), events.size()); ++i) {
            const auto& e = events[i];
            std::cout << "  Byte[" << i << "]: raw=" << (int)e.raw_value
                      << " -> note=" << (int)e.midi_note
                      << " vel=" << (int)e.velocity << '\n';
        }
    }

    return events;
}

uint8_t FileParser::rawToMidiNote(uint8_t raw_value, uint8_t octave_base) {
    // Use modular arithmetic to map byte (0-255) to one octave (12 semitones)
    // octave_base + (value % 12) keeps notes within one octave
    uint8_t midi_note = octave_base + (raw_value % 12);
    
    // Ensure result is valid MIDI range
    if (midi_note > 127) {
        midi_note = 127;
    }
    
    return midi_note;
}

uint8_t FileParser::rawToVelocity(uint8_t raw_value) {
    // Map byte value (0-255) to velocity range (1-127)
    // Avoid 0 (silent) so notes are always audible
    uint8_t velocity = 1 + (raw_value / 2);  // 0-255 -> 1-127
    return velocity;
}

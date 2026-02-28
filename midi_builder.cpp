#include "midi_builder.h"
#include <fstream>
#include <iostream>
#include <algorithm>

void MidiBuilder::writeVarLength(uint32_t value) {
    std::vector<uint8_t> bytes;
    bytes.push_back(value & 0x7F);
    value >>= 7;
    while (value) {
        bytes.insert(bytes.begin(), (value & 0x7F) | 0x80);
        value >>= 7;
    }
    for (uint8_t b : bytes) {
        trackData.push_back(b);
    }
}

void MidiBuilder::write32(uint32_t value) {
    trackData.push_back((value >> 24) & 0xFF);
    trackData.push_back((value >> 16) & 0xFF);
    trackData.push_back((value >> 8) & 0xFF);
    trackData.push_back(value & 0xFF);
}

void MidiBuilder::write16(uint16_t value) {
    trackData.push_back((value >> 8) & 0xFF);
    trackData.push_back(value & 0xFF);
}

void MidiBuilder::setTempo(uint32_t deltaTime, uint32_t microsecondsPerBeat) {
    writeVarLength(deltaTime);
    trackData.push_back(0xFF);  // Meta event
    trackData.push_back(0x51);  // Tempo
    trackData.push_back(0x03);  // Length
    trackData.push_back((microsecondsPerBeat >> 16) & 0xFF);
    trackData.push_back((microsecondsPerBeat >> 8) & 0xFF);
    trackData.push_back(microsecondsPerBeat & 0xFF);
}

void MidiBuilder::noteOn(uint32_t deltaTime, uint8_t note, uint8_t velocity) {
    writeVarLength(deltaTime);
    trackData.push_back(0x90);  // Note On, channel 0
    trackData.push_back(note);
    trackData.push_back(velocity);
}

void MidiBuilder::noteOff(uint32_t deltaTime, uint8_t note) {
    writeVarLength(deltaTime);
    trackData.push_back(0x80);  // Note Off, channel 0
    trackData.push_back(note);
    trackData.push_back(0x40);  // Default velocity
}

void MidiBuilder::buildFromEvents(const std::vector<ParsedEvent>& events, uint32_t tempo) {
    // Reset track data for fresh build
    trackData.clear();

    // Set initial tempo
    setTempo(0, tempo);

    std::cout << "Building MIDI from " << events.size() << " events...\n";

    // Convert each parsed event into MIDI note on/off
    for (const auto& event : events) {
        // Note on with 0 delta time (start immediately)
        noteOn(0, event.midi_note, event.velocity);
        // Note off after duration
        noteOff(event.duration_ticks, event.midi_note);
    }

    std::cout << "MIDI track built. Track data size: " << trackData.size() << " bytes\n";
}

void MidiBuilder::write(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);

    // Write header chunk
    file.write("MThd", 4);

    uint8_t header[6] = {
        0x00, 0x00,  // Format 0 (single track)
        0x00, 0x01,  // 1 track
        0x00, 0x80   // 128 ticks per quarter note
    };
    
    uint8_t headerLen[4] = {0x00, 0x00, 0x00, 0x06};
    file.write((char*)headerLen, 4);
    file.write((char*)header, 6);

    // Write track chunk
    file.write("MTrk", 4);

    uint32_t trackLength = trackData.size() + 4;  // +4 for end of track
    uint8_t lenBytes[4] = {
        static_cast<uint8_t>((trackLength >> 24) & 0xFF),
        static_cast<uint8_t>((trackLength >> 16) & 0xFF),
        static_cast<uint8_t>((trackLength >> 8) & 0xFF),
        static_cast<uint8_t>(trackLength & 0xFF)
    };
    file.write((char*)lenBytes, 4);
    file.write((char*)trackData.data(), trackData.size());

    // End of track
    file.put(0x00);  // Delta time
    file.put(0xFF);  // Meta event
    file.put(0x2F);  // End of track
    file.put(0x00);  // Length

    file.close();

    std::cout << "MIDI file written to: " << filename << '\n';
}

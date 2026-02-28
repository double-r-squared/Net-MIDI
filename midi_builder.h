#ifndef MIDI_BUILDER_H
#define MIDI_BUILDER_H

#include "file_parser.h"
#include <string>
#include <vector>
#include <cstdint>

// ============================================================================
// MidiBuilder: Constructs MIDI files from parsed events
// Responsible for: writing valid MIDI format from event data
// ============================================================================

class MidiBuilder {
private:
    std::vector<uint8_t> trackData;

    void writeVarLength(uint32_t value);
    void write32(uint32_t value);
    void write16(uint16_t value);

public:
    /**
     * Set the tempo of the MIDI file
     * 
     * Parameters:
     *   - deltaTime: ticks when tempo change occurs
     *   - microsecondsPerBeat: tempo in microseconds per quarter note
     *                          120 BPM = 500000 (common default)
     */
    void setTempo(uint32_t deltaTime, uint32_t microsecondsPerBeat);

    /**
     * Add a note-on event
     * 
     * Parameters:
     *   - deltaTime: ticks until this event
     *   - note: MIDI note number (0-127)
     *   - velocity: how loud (0-127)
     */
    void noteOn(uint32_t deltaTime, uint8_t note, uint8_t velocity);

    /**
     * Add a note-off event
     * 
     * Parameters:
     *   - deltaTime: ticks until note ends (duration)
     *   - note: MIDI note number (must match noteOn)
     */
    void noteOff(uint32_t deltaTime, uint8_t note);

    /**
     * Build MIDI from parsed events
     * 
     * Parameters:
     *   - events: Vector of parsed MIDI events from FileParser
     *   - tempo: Tempo in microseconds per beat (default 500000 = 120 BPM)
     */
    void buildFromEvents(const std::vector<ParsedEvent>& events, uint32_t tempo = 500000);

    /**
     * Write the constructed MIDI file to disk
     * 
     * Parameters:
     *   - filename: Output path (e.g., "output.mid")
     */
    void write(const std::string& filename);
};

#endif

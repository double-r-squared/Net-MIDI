#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <string>
#include <vector>
#include <cstdint>

// ============================================================================
// FileParser: Handles parsing of captured network traffic
// Responsible for: reading binary packet data and converting to usable format
// 
// Strategy: Treat binary data as raw bytes, convert each byte to MIDI value
// This doesn't care about the binary packet format - just extracts values
// ============================================================================

struct ParsedEvent {
    uint8_t raw_value;      // Original byte value from file (0-255)
    uint8_t midi_note;      // MIDI note (0-127) derived from raw value
    uint32_t duration_ticks; // Duration in MIDI ticks (default 128 = quarter note)
    uint8_t velocity;       // MIDI velocity (0-127)
};

class FileParser {
public:
    /**
     * Parse binary file and convert to MIDI events
     * 
     * Strategy: 
     *   - Read each byte from file as a separate value
     *   - Apply modular arithmetic to convert to valid MIDI range
     *   - Create a MIDI note for each byte
     * 
     * Parameters:
     *   - input_file: Path to captured packet file
     *   - octave_base: Base MIDI note for octave (default 48 = C3)
     *   - duration_ticks: Duration for each note (default 128 = quarter note)
     * 
     * Returns: Vector of ParsedEvent objects
     */
    static std::vector<ParsedEvent> parseFile(
        const std::string& input_file,
        uint8_t octave_base = 48,  // C3
        uint32_t duration_ticks = 128
    );

    /**
     * Convert raw byte value to MIDI note using modular arithmetic
     * 
     * Formula: midi_note = octave_base + (raw_value % 12)
     * This ensures all notes fall within one octave
     * 
     * Parameters:
     *   - raw_value: Byte value from file (0-255)
     *   - octave_base: Base MIDI note (default 48 = C3)
     * 
     * Returns: Valid MIDI note (0-127)
     */
    static uint8_t rawToMidiNote(uint8_t raw_value, uint8_t octave_base = 48);

    /**
     * Calculate velocity from raw byte value
     * Spread values across full velocity range (1-127)
     * 
     * Parameters:
     *   - raw_value: Byte value from file (0-255)
     * 
     * Returns: MIDI velocity (1-127)
     */
    static uint8_t rawToVelocity(uint8_t raw_value);
};

#endif

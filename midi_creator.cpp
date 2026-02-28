#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <sstream>

// ============================================================================
// MIDI NOTE REFERENCE (0-127, where 60 is Middle C)
// ============================================================================
// C0=12, C1=24, C2=36, C3=48, C4=60(Middle C), C5=72, C6=84, C7=96, C8=108
// 
// Input Parameters for MIDI creation:
// 
// TEMPO:
//   - microseconds per beat (default: 500000 = 120 BPM)
//   - Formula: microseconds = 60,000,000 / BPM
//   - Example: 120 BPM = 500000, 60 BPM = 1000000
//
// TICKS PER BEAT:
//   - Resolution of the timing (typically 96 or 128)
//   - Higher = more precise timing
//   - Used for deltaTime: quarter note = ticks_per_beat
//
// NOTE VALUE (0-127):
//   - 60 = Middle C
//   - Each number represents one semitone
//   - 12 semitones = 1 octave
//
// VELOCITY (0-127):
//   - Volume/intensity of the note
//   - 0 = silent, 100 = standard, 127 = maximum
//
// DELTA TIME:
//   - Duration in ticks between events
//   - quarter note = ticks_per_beat (typically 128)
//   - eighth note = ticks_per_beat / 2 (64)
//   - sixteenth note = ticks_per_beat / 4 (32)
//
// ============================================================================

class MidiFile {
private:
    std::vector<uint8_t> trackData;
    
    // Write a variable-length quantity (used for timing)
    void writeVarLength(uint32_t value) {
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
    
    // Write a 32-bit big-endian integer
    void write32(uint32_t value) {
        trackData.push_back((value >> 24) & 0xFF);
        trackData.push_back((value >> 16) & 0xFF);
        trackData.push_back((value >> 8) & 0xFF);
        trackData.push_back(value & 0xFF);
    }
    
    // Write a 16-bit big-endian integer
    void write16(uint16_t value) {
        trackData.push_back((value >> 8) & 0xFF);
        trackData.push_back(value & 0xFF);
    }
    
public:
    // Add a note-on event
    // Parameters:
    //   deltaTime: ticks until this event (0 = immediately after last event)
    //   note:      MIDI note number (0-127, where 60 = Middle C)
    //   velocity:  how loud the note is (0-127, where 100 is standard)
    void noteOn(uint32_t deltaTime, uint8_t note, uint8_t velocity) {
        writeVarLength(deltaTime);
        trackData.push_back(0x90);  // Note On, channel 0
        trackData.push_back(note);
        trackData.push_back(velocity);
    }
    
    // Add a note-off event (stops the note)
    // Parameters:
    //   deltaTime: ticks until this event (duration of the note)
    //   note:      MIDI note number (must match the noteOn)
    void noteOff(uint32_t deltaTime, uint8_t note) {
        writeVarLength(deltaTime);
        trackData.push_back(0x80);  // Note Off, channel 0
        trackData.push_back(note);
        trackData.push_back(0x40);  // Default velocity
    }
    
    // Set the tempo of the MIDI file
    // Parameters:
    //   deltaTime:            ticks when this tempo change occurs
    //   microsecondsPerBeat:  tempo in microseconds per quarter note
    //                         Common values:
    //                         - 250000 = 240 BPM (fast)
    //                         - 500000 = 120 BPM (medium/standard)
    //                         - 1000000 = 60 BPM (slow)
    void setTempo(uint32_t deltaTime, uint32_t microsecondsPerBeat) {
        writeVarLength(deltaTime);
        trackData.push_back(0xFF);  // Meta event
        trackData.push_back(0x51);  // Tempo
        trackData.push_back(0x03);  // Length
        trackData.push_back((microsecondsPerBeat >> 16) & 0xFF);
        trackData.push_back((microsecondsPerBeat >> 8) & 0xFF);
        trackData.push_back(microsecondsPerBeat & 0xFF);
    }
    
    // Write the MIDI file
    // Parameters:
    //   filename: path to output .mid file (e.g., "output.mid")
    void write(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        
        // Write header
        file.write("MThd", 4);
        uint32_t headerLength = 6;
        write32(headerLength);
        file.write((char*)trackData.data() - 6, 6);  // Write header data
        
        // Actually, let's simplify - write header properly
        file.seekp(0);
        file.write("MThd", 4);
        
        uint8_t header[6] = {
            0x00, 0x00,  // Format 0
            0x00, 0x01,  // 1 track
            0x00, 0x80   // 128 ticks per quarter note
        };
        file.write((char*)header, 6);
        
        // Write track chunk
        file.write("MTrk", 4);
        uint32_t trackLength = trackData.size() + 4;  // +4 for end of track
        uint8_t lenBytes[4] = {
            (trackLength >> 24) & 0xFF,
            (trackLength >> 16) & 0xFF,
            (trackLength >> 8) & 0xFF,
            trackLength & 0xFF
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
};

int main() {
    MidiFile midi;
    
    // ========================================================================
    // EXAMPLE: Playing a C major scale
    // ========================================================================
    
    // Set tempo to 500000 microseconds per beat (120 BPM)
    // This is a good starting point - adjust for faster/slower playback
    midi.setTempo(0, 500000);
    
    // C major scale notes and their MIDI values
    // Format: {MIDI_NOTE_NUMBER, DURATION_IN_TICKS}
    //
    // MIDI note numbers: 60=C4, 62=D4, 64=E4, 65=F4, 67=G4, 69=A4, 71=B4, 72=C5
    // Ticks: 128 = quarter note, 64 = eighth note, 32 = sixteenth note
    
    struct Note {
        uint8_t note;      // MIDI note value (0-127)
        uint32_t duration; // Delta time in ticks (how long to play)
    };
    
    std::vector<Note> scale = {
        {60, 128},  // C4, quarter note
        {62, 128},  // D4, quarter note
        {64, 128},  // E4, quarter note
        {65, 128},  // F4, quarter note
        {67, 128},  // G4, quarter note
        {69, 128},  // A4, quarter note
        {71, 128},  // B4, quarter note
        {72, 128},  // C5, quarter note
    };
    
    // Play each note in the scale
    for (const auto& n : scale) {
        midi.noteOn(0, n.note, 100);      // Start note (velocity=100)
        midi.noteOff(n.duration, n.note); // End note after duration
    }
    
    midi.write("output.mid");
    
    // ========================================================================
    // TO READ FROM A TEXT FILE IN THE FUTURE:
    // ========================================================================
    // 
    // Format your text file like:
    //   tempo 500000
    //   note 60 128 100
    //   note 62 128 100
    //   note 64 128 100
    //   ... (more notes)
    //
    // Then modify the code to:
    //   1. Open and read the text file line by line
    //   2. Parse each parameter (note number, duration, velocity)
    //   3. Apply modular arithmetic to transform the values:
    //      - note % 12 to stay within one octave
    //      - duration % 256 to normalize timing
    //      - Or create custom formulas based on your needs
    //   4. Call noteOn/noteOff with the processed values
    //
    // Example with modular arithmetic:
    //   input_note = 60;
    //   actual_note = 48 + (input_note % 12);  // Stay in range C3-B3
    //   midi.noteOn(0, actual_note, velocity);
    // 
    // ========================================================================
    
    return 0;
}

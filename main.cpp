#include "user_interface.h"
#include "network_capture.h"
#include "file_parser.h"
#include "midi_builder.h"
#include <iostream>
#include <cstdint>

// ============================================================================
// MAIN: Orchestration logic
// Responsible for: coordinating the workflow between modules
// 
// Workflow:
// 1. Show UI welcome and get user parameters
// 2. Capture network packets using NetworkCapture
// 3. Parse captured file using FileParser
// 4. Build MIDI using MidiBuilder
// 5. Display completion message
// ============================================================================

int main() {
    try {
        // ====== PHASE 1: USER INTERFACE ======
        UserInterface::showWelcome();

        std::string device = UserInterface::promptForDevice();
        std::string filter = UserInterface::promptForFilter();
        int packet_limit = UserInterface::promptForPacketLimit();
        uint32_t tempo = UserInterface::promptForTempo();
        uint8_t octave_base = UserInterface::promptForOctaveBase();

        // ====== PHASE 2: NETWORK CAPTURE ======
        UserInterface::showCaptureInfo(device, filter, packet_limit);

        std::string captured_file = "captured_traffic.bin";
        bool capture_success = NetworkCapture::capturePackets(
            device,
            filter,
            captured_file,
            packet_limit,
            1000  // 1 second timeout
        );

        if (!capture_success) {
            std::cerr << "Error: Packet capture failed. Exiting.\n";
            return 1;
        }

        // ====== PHASE 3: FILE PARSING ======
        UserInterface::showParsingInfo(captured_file);

        std::vector<ParsedEvent> events = FileParser::parseFile(
            captured_file,
            octave_base,
            128  // Quarter note duration in ticks
        );

        if (events.empty()) {
            std::cerr << "Error: No events parsed from file. Exiting.\n";
            return 1;
        }

        // ====== PHASE 4: MIDI BUILDING ======
        std::cout << "\n─── Building MIDI ───\n";

        MidiBuilder midi;
        midi.buildFromEvents(events, tempo);

        // ====== PHASE 5: WRITE OUTPUT ======
        std::string output_file = "output.mid";
        midi.write(output_file);

        UserInterface::showCompletion(output_file);

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
}

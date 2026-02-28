#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <string>
#include <cstdint>

// ============================================================================
// UserInterface: Handles all terminal I/O with the user
// Responsible for: prompts, user input, status messages
// ============================================================================

class UserInterface {
public:
    /**
     * Display welcome banner and instructions
     */
    static void showWelcome();

    /**
     * Prompt user for network interface
     * Returns: device name (e.g., "eth0", "tailscale0")
     */
    static std::string promptForDevice();

    /**
     * Prompt user for packet capture filter
     * Returns: BPF filter expression (e.g., "ip", "tcp", "port 8080")
     */
    static std::string promptForFilter();

    /**
     * Prompt user for number of packets to capture
     * Returns: packet limit (0 = infinite, or positive number)
     */
    static int promptForPacketLimit();

    /**
     * Prompt user for MIDI tempo in BPM
     * Returns: tempo in microseconds per beat
     */
    static uint32_t promptForTempo();

    /**
     * Prompt user for base MIDI octave
     * Returns: base MIDI note (e.g., 48 = C3, 60 = C4)
     */
    static uint8_t promptForOctaveBase();

    /**
     * Display capture progress information
     */
    static void showCaptureInfo(const std::string& device, const std::string& filter, int packet_limit);

    /**
     * Display parsing progress
     */
    static void showParsingInfo(const std::string& input_file);

    /**
     * Display final status and output file location
     */
    static void showCompletion(const std::string& output_file);
};

#endif

#include "user_interface.h"
#include <iostream>
#include <string>
#include <limits>

void UserInterface::showWelcome() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              Net-MIDI: Network-to-Music Generator              ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║  Convert network traffic into MIDI music using modular math    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

std::string UserInterface::promptForDevice() {
    std::cout << "Enter network interface (e.g., eth0, wlan0, tailscale0): ";
    std::string device;
    std::getline(std::cin, device);
    
    if (device.empty()) {
        std::cout << "Using default: eth0\n";
        return "eth0";
    }
    return device;
}

std::string UserInterface::promptForFilter() {
    std::cout << "\nEnter BPF filter expression (default 'ip'):\n";
    std::cout << "  Examples: ip, tcp, udp, port 80, 'tcp and port 443'\n";
    std::cout << "  Filter: ";
    
    std::string filter;
    std::getline(std::cin, filter);
    
    if (filter.empty()) {
        std::cout << "Using default: ip\n";
        return "ip";
    }
    return filter;
}

int UserInterface::promptForPacketLimit() {
    std::cout << "\nEnter packet capture limit (0 = infinite, or number):\n";
    std::cout << "  Limit: ";
    
    int limit;
    if (!(std::cin >> limit)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Using 100 packets.\n";
        return 100;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (limit < 0) limit = 0;
    return limit;
}

uint32_t UserInterface::promptForTempo() {
    std::cout << "\nEnter tempo in BPM (default 120):\n";
    std::cout << "  Tempo: ";
    
    int bpm;
    if (!(std::cin >> bpm)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Using 120 BPM.\n";
        return 500000;  // 120 BPM
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (bpm <= 0) bpm = 120;
    
    // Convert BPM to microseconds per beat
    uint32_t microseconds = 60000000 / bpm;
    std::cout << "Set to " << bpm << " BPM (" << microseconds << " μs/beat)\n";
    return microseconds;
}

uint8_t UserInterface::promptForOctaveBase() {
    std::cout << "\nEnter base MIDI note (default 48 = C3):\n";
    std::cout << "  MIDI note values: 36=C2, 48=C3, 60=C4, 72=C5\n";
    std::cout << "  Base note: ";
    
    int note;
    if (!(std::cin >> note)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Using 48 (C3).\n";
        return 48;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (note < 0) note = 0;
    if (note > 127) note = 127;
    
    std::cout << "Set to " << note << '\n';
    return (uint8_t)note;
}

void UserInterface::showCaptureInfo(const std::string& device, const std::string& filter, int packet_limit) {
    std::cout << "\n─── Capture Configuration ───\n";
    std::cout << "Device:       " << device << '\n';
    std::cout << "Filter:       " << filter << '\n';
    std::cout << "Packet limit: " << (packet_limit == 0 ? "infinite" : std::to_string(packet_limit)) << '\n';
    std::cout << "\nPress Ctrl+C to stop capture when ready...\n";
}

void UserInterface::showParsingInfo(const std::string& input_file) {
    std::cout << "\n─── Parsing Configuration ───\n";
    std::cout << "Input file:   " << input_file << '\n';
}

void UserInterface::showCompletion(const std::string& output_file) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    ✓ Success! MIDI Generated                   ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║  Output: " << output_file << '\n';
    std::cout << "║  Open in your favorite MIDI player or DAW to listen!           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

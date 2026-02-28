#ifndef NETWORK_CAPTURE_H
#define NETWORK_CAPTURE_H

#include <string>
#include <cstdint>

// ============================================================================
// NetworkCapture: Handles all network I/O
// Responsible for: capturing packets from network interface
// ============================================================================

class NetworkCapture {
public:
    /**
     * Capture network packets from specified interface
     * 
     * Parameters:
     *   - device: Network interface name (e.g., "eth0", "tailscale0")
     *   - filter_expression: BPF filter (e.g., "ip", "tcp", "port 8080")
     *   - output_file: Path to save captured packets (binary format)
     *   - packet_limit: Max packets to capture (0 = infinite)
     *   - timeout_ms: Packet timeout in milliseconds
     * 
     * Returns: true if successful, false if error occurred
     */
    static bool capturePackets(
        const std::string& device,
        const std::string& filter_expression,
        const std::string& output_file,
        int packet_limit = 0,
        int timeout_ms = 1000
    );

    /**
     * Get list of available network interfaces
     * Useful for user selection
     */
    static std::string getDefaultDevice();
};

#endif

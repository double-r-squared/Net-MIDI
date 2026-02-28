#include "network_capture.h"
#include <pcap.h>
#include <fstream>
#include <iostream>
#include <cstring>

// Callback function for pcap_loop
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    std::ofstream *output_file = (std::ofstream*)user_data;
    output_file->write((char*)packet, pkthdr->len);
}

bool NetworkCapture::capturePackets(
    const std::string& device,
    const std::string& filter_expression,
    const std::string& output_file,
    int packet_limit,
    int timeout_ms
) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program fp;
    bpf_u_int32 mask, net;

    // Open the network interface
    handle = pcap_open_live(device.c_str(), BUFSIZ, 1, timeout_ms, errbuf);
    if (handle == nullptr) {
        std::cerr << "Error: Couldn't open device " << device << ": " << errbuf << '\n';
        return false;
    }

    // Get network number and mask associated with capture device
    if (pcap_lookupnet(device.c_str(), &net, &mask, errbuf) == -1) {
        std::cerr << "Warning: Couldn't get netmask for device " << device << '\n';
        net = 0;
        mask = 0;
    }

    // Compile the filter expression
    if (pcap_compile(handle, &fp, filter_expression.c_str(), 0, net) == -1) {
        std::cerr << "Error: Couldn't parse filter " << filter_expression << ": "
                  << pcap_geterr(handle) << '\n';
        pcap_close(handle);
        return false;
    }

    // Apply the compiled filter
    if (pcap_setfilter(handle, &fp) == -1) {
        std::cerr << "Error: Couldn't install filter " << filter_expression << ": "
                  << pcap_geterr(handle) << '\n';
        pcap_freecode(&fp);
        pcap_close(handle);
        return false;
    }

    // Open the output file
    std::ofstream out_file(output_file, std::ios::binary);
    if (!out_file.is_open()) {
        std::cerr << "Error: Couldn't open output file " << output_file << '\n';
        pcap_freecode(&fp);
        pcap_close(handle);
        return false;
    }

    std::cout << "Starting packet capture on " << device << " with filter: " 
              << filter_expression << '\n';
    std::cout << "Packet limit: " << (packet_limit == 0 ? "infinite" : std::to_string(packet_limit)) << '\n';

    // Capture packets in a loop
    int result = pcap_loop(handle, packet_limit, packet_handler, (u_char*)&out_file);

    // Cleanup
    out_file.close();
    pcap_freecode(&fp);
    pcap_close(handle);

    if (result == -1) {
        std::cerr << "Error during packet capture\n";
        return false;
    }

    std::cout << "Packet capture complete. Captured " << result << " packets.\n";
    std::cout << "Data saved to: " << output_file << '\n';
    return true;
}

std::string NetworkCapture::getDefaultDevice() {
    char errbuf[PCAP_ERRBUF_SIZE];
    char *dev = pcap_lookupdev(errbuf);
    if (dev == nullptr) {
        std::cerr << "Couldn't find default device: " << errbuf << '\n';
        return "eth0";  // fallback
    }
    return std::string(dev);
}

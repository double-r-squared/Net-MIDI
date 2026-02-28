#include <pcap.h>
#include <fstream>
#include <iostream>

using namespace std;

void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    ofstream *output_file = (ofstream*)user_data;
    output_file->write((char*)packet, pkthdr->len);
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program fp;
    char filter_exp[] = "ip";  // You can modify this filter expression to capture specific traffic
    const char *dev = "tailscale0";  // Change this to your network interface

    bpf_u_int32 mask;
    bpf_u_int32 net;

    // Open the network interface
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        cerr << "Couldn't open device eth0: " << errbuf << '\n';
        return(2);
    }

    // Get network number and mask associated with capture device
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        net = 0;
        mask = 0;
    }

    // Compile the filter expression
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        cerr << "Couldn't parse filter " << filter_exp << ": "
                  << pcap_geterr(handle) << '\n';
        return(2);
    }

    // Apply the compiled filter
    if (pcap_setfilter(handle, &fp) == -1) {
        cerr << "Couldn't install filter " << filter_exp << ": "
                  << pcap_geterr(handle) << '\n';
        return(2);
    }

    // Open the output file
    ofstream output_file("captured_traffic.txt", std::ios::binary);

    // Capture packets in a loop
    pcap_loop(handle, 0, packet_handler, (u_char*)&output_file);

    // Cleanup
    pcap_freecode(&fp);
    pcap_close(handle);

    return(0);
}
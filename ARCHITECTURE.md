# Net-MIDI: Network-to-Music Generator

Convert network packet traffic into MIDI music using modular arithmetic.

## Architecture Overview

The project is organized into **5 independent modules** with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────┐
│                    MAIN (Orchestration)                     │
│  - Coordinates workflow between modules                     │
│  - Handles error checking                                   │
└─────────────────────────────────────────────────────────────┘
          ↓           ↓           ↓           ↓
    ┌─────────┐  ┌──────────┐  ┌──────────┐  ┌──────────────┐
    │  USER   │  │ NETWORK  │  │   FILE   │  │    MIDI      │
    │   I/O   │  │ CAPTURE  │  │  PARSER  │  │   BUILDER    │
    └─────────┘  └──────────┘  └──────────┘  └──────────────┘
    Terminal I/O  Packet Sniff  Binary Parse  MIDI Creation

```

### Module Responsibilities

#### 1. **UserInterface** (`user_interface.h/cpp`)
- **Responsibility:** All terminal I/O with the user
- **Handles:** Prompts, input validation, status messages, welcome banner
- **Example Usage:**
  ```cpp
  std::string device = UserInterface::promptForDevice();
  uint32_t tempo = UserInterface::promptForTempo();
  ```

#### 2. **NetworkCapture** (`network_capture.h/cpp`)
- **Responsibility:** Network packet capture using libpcap
- **Handles:** Device selection, filter compilation, packet collection
- **Parameters:**
  - `device`: Network interface name (e.g., "eth0", "tailscale0")
  - `filter_expression`: BPF filter (e.g., "ip", "tcp", "port 80")
  - `packet_limit`: Maximum packets to capture (0 = infinite)
  - `timeout_ms`: Packet read timeout
- **Example Usage:**
  ```cpp
  bool success = NetworkCapture::capturePackets(
      "eth0",           // device
      "ip",             // filter
      "captured.bin",   // output file
      1000,             // max packets
      1000              // timeout ms
  );
  ```

#### 3. **FileParser** (`file_parser.h/cpp`)
- **Responsibility:** Convert binary packet data to MIDI events
- **Strategy:** Treats binary data as raw byte values, no format assumptions
- **Conversion Formula:**
  - **MIDI Note:** `octave_base + (raw_byte % 12)`
    - Uses modular arithmetic to constrain to one octave
    - Default octave_base = 48 (C3)
  - **Velocity:** `1 + (raw_byte / 2)`
    - Spreads byte values (0-255) across velocity range (1-127)
- **Example Usage:**
  ```cpp
  auto events = FileParser::parseFile(
      "captured.bin",  // input file
      48,              // octave base (C3)
      128              // duration per note in ticks
  );
  ```

#### 4. **MidiBuilder** (`midi_builder.h/cpp`)
- **Responsibility:** Construct valid MIDI files from parsed events
- **Handles:** MIDI format encoding, note timing, tempo setting
- **Example Usage:**
  ```cpp
  MidiBuilder midi;
  midi.buildFromEvents(events, 500000);  // tempo in μs/beat
  midi.write("output.mid");
  ```

#### 5. **Main** (`main_new.cpp`)
- **Responsibility:** Orchestrate the complete workflow
- **Workflow:**
  1. Get parameters from user via UserInterface
  2. Capture packets via NetworkCapture
  3. Parse file via FileParser
  4. Build MIDI via MidiBuilder
  5. Display completion

## Building

```bash
# Build the project
make

# Run
./net-midi

# Clean artifacts
make clean

# Rebuild from scratch
make rebuild
```

## Workflow Example

```
./net-midi
┌─ Prompts user for:
│  - Network interface (eth0)
│  - Filter expression (ip)
│  - Packet limit (1000)
│  - Tempo (120 BPM)
│  - Base octave (48)
│
├─ Captures 1000 packets on eth0 to "captured_traffic.bin"
│
├─ Parses binary file:
│  - Reads each byte from captured packets
│  - Converts to MIDI note using modular arithmetic
│  - Creates MIDI event with velocity
│
├─ Builds MIDI track from events
│
└─ Writes "output.mid" playable in any MIDI player
```

## Key Design Decisions

### 1. **Binary Agnostic Parsing**
The FileParser doesn't care about the actual packet format (TCP headers, IP fields, etc.). It simply:
- Reads raw bytes sequentially
- Applies modular arithmetic to convert to MIDI values
- Creates one MIDI note per byte

**Why?** This approach works with any binary data and is simple to understand and modify.

### 2. **Modular Arithmetic for Note Range**
```cpp
midi_note = octave_base + (raw_byte % 12)
```
- `raw_byte % 12` maps any value (0-255) to 0-11 semitones
- Adding `octave_base` keeps notes within one octave
- Ensures all notes are musically coherent

### 3. **Clear Separation of Concerns**
- Each module has ONE responsibility
- Modules don't depend on each other (main coordinates)
- Easy to test, modify, or replace individual modules

## Extending the Code

### To add a custom note generator:
1. Create a new function in `FileParser` that implements your algorithm
2. Call it from `main.cpp` instead of `parseFile()`

**Example:** Generate notes based on packet byte patterns:
```cpp
static std::vector<ParsedEvent> parseByPattern(const std::string& input_file) {
    // Read pairs of bytes
    // First byte = note
    // Second byte = duration
    // ...
}
```

### To add MIDI effects:
1. Add methods to `MidiBuilder` (e.g., `addControlChange()`, `setPan()`)
2. Call from `main.cpp` after building events

### To add different input sources:
1. Create a new input module (e.g., `audio_capture.h`)
2. Return same `ParsedEvent` vector format
3. Main automatically feeds to MidiBuilder

## MIDI Parameter Reference

### Note Values (0-127)
- C0=12, C1=24, C2=36, C3=48
- **C4=60 (Middle C)**, C5=72, C6=84, C7=96, C8=108

### Tempo Formula
```
microseconds_per_beat = 60,000,000 / BPM
Examples:
  60 BPM  = 1,000,000 μs/beat
  120 BPM = 500,000 μs/beat (standard)
  240 BPM = 250,000 μs/beat
```

### Note Duration (in ticks)
- Assumes 128 ticks per quarter note
- Quarter note = 128 ticks
- Eighth note = 64 ticks
- Sixteenth note = 32 ticks

## License

[Your License Here]

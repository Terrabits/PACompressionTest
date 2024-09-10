# Power Supply Development Notes

For a "dumb mode" (non-list-mode) power supply.

This may not work for pulsed measurements. Pulsed measurements may require a power supply (or DMM) that supports list mode, and corresponding hardware triggers.

## Channel Settings

For a "dumb mode" (single point) power supply, the following settings are required:

- Manual Sweep (`vna.channel(n).manualSweepOn()`)
- Sweep Count (`vna.channel(n).setSweepCount(1)`)
- Chopped Driving Mode (`vna.channel(n).setDrivingMode(VnaChannel::DrivingMode::Chopped)`)
- Trigger Source: Manual (`vna.channel(n).trigger().setSource(VnaTrigger::Source::Manual)`)
- Trigger Sequence: Point (`vna.channel(n).trigger().setSequence(VnaTrigger::Sequence::Point)`)

## Measurement Sequence

### Start Sweep

- `vna.channel(n).startSweep()`

### Measure Point

This sequence should be executed for each point.

- `vna.channel(n).trigger().sendManualTrigger()`
- `powerSupply->voltage_V()`
- `powerSupply->current_A()`

## Development Work

### Channel

```c++
class VnaChannel {

public:

  enum /*class*/ DrivingMode {
    Alternating,
    Chopped,
    Auto
  }

  DrivingMode drivingMode();

  void setDrivingMode(DrivingMode mode);
}
```

### Trigger Settings

Need to add this functionality to `RsaToolbox/Instruments/Vna/VnaTrigger[.h|.cpp]`:

```c++
class VnaTrigger {

public:

  enum /*class*/ Sequence {
    Sweep,
    Segment,
    Point,
    PartialPoint
  };

  Sequence sequence();

  void setSequence(Sequence sequence);
}
```

# Power Supply Algorithm

Must be point-by-point since NGE does not have "list mode", VNA cannot trigger by point.

```c++
// power in sweep values
QVector<double> powersIn_dBm = linspace(
  startPowerIn_dBm,
  stopPowerIn_dBm,
  powerInPoints
);

// frequency sweep values
QVector<double> frequencies_Hz = linspace(
  startFrequency_Hz,
  stopFrequency_Hz,
  frequencyPoints
);


// initialize VNA

// manual sweeps
vna.manualSweepOn();

// copy channel
vna.channel(measurementSettings.channel()).select();
uint channelIndex  = vna.createChannel();
VnaChannel channel = vna.channel(channelIndex);
channel.setSParameterGroup(
  measurementSettings.inputPort(),
  measurementSettings.outputPort()
);

// single point sweep
channel.setSweepType(VnaChannel::SweepType::LinearFrequencySweep);
channel.setPoints(1);

// use linear frequency sweep type
VnaLinearSweep sweep = channel.linearSweep();

// measure a1 with generator on port 1
VnaTrace aInTrace = vna.createTrace();
QString parameter = "a%1(D%2)";
parameter = parameter.arg(inputPort).arg(inputPort);
aInTrace.setParameter(parameter);
aInTrace.setFormat(VnaTrace::Format::LogMagnitude);


// perform sweeps
for (int iPower = 0; i < powerInPoints; i_power++) {
  for (int iFreq = 0; i < frequencyPoints; i++) {

    // power in, frequency
    const double powerIn_dBm  = powersIn_dBm[iPower];
    const double frequency_Hz = frequencies_Hz[iFreq];

    // set power in, frequency
    sweep.setPower(powerIn_dBm);
    sweep.setStartFrequency(frequency_Hz);

    // sweep vna
    channel.startSweep();
    channel.pause();

    // let power supply settle?
    // TODO: wait for NGE?

    measuredPowersIn_dBm[iPower][iFreq] = aInTrace.y_formatted()[0];
    data     [iPower][iFreq] = sweep.readSParameterGroup()[0];  returns array?
    current_A[iPower][iFreq] = powerSupply.current_A();
    voltage_V[iPower][iFreq] = powerSupply.voltage_V();
  }
}
```

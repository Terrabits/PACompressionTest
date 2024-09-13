#include "ProcessTrace.h"


// RsaToolbox
#include "General.h"
#include "Helpers.h"
using namespace RsaToolbox;


// Qt
#include <QDebug>


ProcessTrace::ProcessTrace(TraceSettings *settings, MeasurementData *data, Vna *vna, uint defaultDiagram) :
    _settings(settings),
    _data(data),
    _vna(vna),
    _diagram(defaultDiagram),
    _memoryTraceName(settings->name)
{
    // valid trace?
    if (!_settings->isValid()) {
      return;
    }

    _channelName = "_" + _memoryTraceName;
    _dataTraceName = _channelName;

    // retrieve data?
    if (!retrieveData()) {
        return;
    }

    // display on
    _vna->settings().updateDisplay();
    _vna->settings().displayOn();

    // trace already exists?
    if (isPreexistingTrace()) {
        updateChannel();
        updateTrace();
        return;
    }

    // new trace
    createChannel();
    createTrace();
}
ProcessTrace::~ProcessTrace() {

}

bool ProcessTrace::isPreexistingTrace() {
    const bool isChannel = _vna->channelId(_channelName) != 0;
    const bool isDataTrace = _vna->isTrace(_dataTraceName);
    const bool isMemoryTrace = _vna->isTrace(_memoryTraceName);
    if (!isChannel || !isDataTrace || !isMemoryTrace) {
        if (isChannel)
            _vna->deleteChannel(_vna->channelId(_channelName));
        if (_vna->isTrace(_dataTraceName))
            _vna->deleteTrace(_dataTraceName);
        if (_vna->isTrace(_memoryTraceName))
            _vna->deleteTrace(_memoryTraceName);
        return false;
    }
    else {
        return true;
    }
}

ComplexRowVector ProcessTrace::toComplex_dBm(QRowVector values_dBm) {
    // 0 dBm: sqrt(5.0)/10.0 ...?????
    const ComplexDouble zero_dBm(0.223606797749979, 0);
    return multiply(toMagnitude(values_dBm), zero_dBm);
}
ComplexRowVector ProcessTrace::toComplex_deg(QRowVector values_deg) {
    ComplexRowVector _result(values_deg.size());
//    _result.resize(values_deg.size());
    for (int i = 0; i < values_deg.size(); i++) {
        const double rad = values_deg[i] * PI / 180.0;
        _result[i] = ComplexDouble(cos(rad), sin(rad));
    }
    return _result;
}


bool ProcessTrace::retrieveData() {
    _x.clear();
    _y_complex.clear();
    _y_formatted.clear();


    // y s parameter?
    if (_settings->isYSParameter()) { // SParameter

        // get ports
        uint outputPort, inputPort;

        if (_settings->isYS11Trace()) {
            outputPort = 1;
            inputPort = 1;
        }

        else if (_settings->isYS21Trace()) {
            outputPort = 2;
            inputPort = 1;
        }

        else if (_settings->isYS12Trace()) {
            outputPort = 1;
            inputPort = 2;
        }

        else if (_settings->isYS22Trace()) {
            // S22
            outputPort = 2;
            inputPort = 2;
        }

        else {
          // this should not happen...
          return false;
        }


        // vs frequency?
        if (_settings->isXFrequency()) {

            // at compression?
            if (_settings->isAtCompression()) {
                // y: SParameter
                // x: Frequency
                // @: Compression
                _x = _data->frequencies_Hz();
                _y_complex = _data->sParameterAtCompression(outputPort, inputPort);
                return true;
            }

            // at max gain?
            if (_settings->isAtMaximumGain()) {
                // y: SParameter
                // x: Frequency
                // @: MaxGain
                _x = _data->frequencies_Hz();
                _y_complex = _data->sParameterAtMaxGain(outputPort, inputPort);
                return true;
            }

            // at pin?
            if (_settings->isAtPin()) {
                // y: SParameter
                // x: Frequency
                // @: Pin
                return _data->sParameterVsFrequencyAtPin(_settings->atValue, outputPort, inputPort, _x, _y_complex);
            }

            // at pout?
            // if (_settings->isAtPout()) {
            //   // y: sParameter
            //   // x: Frequency
            //   // @: Pout
            //   // TODO?
            //   return true;
            // }

            // this should not happen...
            return false;
        }


        // vs pin?
        if (_settings->isXPin()) {

            if (_settings->isAtFrequency()) {
              // y: SParameter
              // x: Pin
              // @: Frequency
              return _data->sParameterVsPin(_settings->atValue, outputPort, inputPort, _x, _y_complex);
            }

            // this should not happen...
            return false;
        }

        // vs pout?
        if (_settings->isXPout()) {

            if (_settings->isAtFrequency()) {
              // y: SParameter
              // x: Pout
              // @: Frequency
              return _data->sParameterVsPout(_settings->atValue, outputPort, inputPort, _x, _y_complex);
            }

            // this should never happen...
            return false;
        }

        // this should not happen...
        return false;
    }


    // y power?
    if (_settings->isYPower()) {

        // vs frequency?
        if (_settings->isXFrequency()) {

            _x = _data->frequencies_Hz();

            // pin?
            if (_settings->isYPin()) {

                // at compression?
                if (_settings->isAtCompression()) {
                    // y: Pin
                    // x: Frequency
                    // @: Compression
                    _y_formatted = _data->powerInAtCompression_dBm();
                    return true;
                }

                if (_settings->isAtMaximumGain()) {
                    // y: Pin
                    // x: Frequency
                    // @: MaxGain
                    _y_formatted = _data->powerInAtMaxGain_dBm();
                    return true;
                }

                // this should not happen...
                return false;
            }

            // pout?
            if (_settings->isYPout()) {

                // at compression?
                if (_settings->isAtCompression()) {
                    // y: Pout
                    // x: Frequency
                    // @: Compression
                    _y_formatted = _data->powerOutAtCompression_dBm();
                    return true;
                }

                // at max gain?
                if (_settings->isAtMaximumGain()) {
                    // y: Pout
                    // x: Frequency
                    // @: MaxGain
                    _y_formatted = _data->powerOutAtMaxGain_dBm();
                    return true;
                }

                // at pin?
                if (_settings->isAtPin()) {
                    // y: Pout
                    // x: Frequency
                    // @: Pin
                    return _data->poutVsFrequency(_settings->atValue, _x, _y_formatted);
                }

                // this should not happen...
                return false;
            }

            // this should not happen...
            return false;
        }


        // vs pin?
        if (_settings->isXPin()) {

            if (_settings->isAtFrequency()) {
              // y: Pout
              // x: Pin
              // @: Frequency
              return _data->poutVsPin(_settings->atValue, _x, _y_formatted);
            }

            // this should not happen...
            return false;
        }

        // this should not happen...
        return false;
    }


    // y ampm?
    if (_settings->isYAmPm()) {

        // vs pin?
        if (_settings->isXPin()) {
            // y: AMPM
            // x: Pin
            // @: Frequency
            _data->amPmVsPin(_settings->atValue, _x, _y_formatted);
            return true;
        }


        // vs pout?
        if (_settings->isXPout()) {
            // y: AMPM
            // x: Pout
            // @: Frequency
            _data->amPmVsPout(_settings->atValue, _x, _y_formatted);
            return true;
        }


        // this should never happen...
        return false;
    }


    // y pae-related?
    if (_settings->isYPaeRelated()) {

        // vs frequency?
        if (_settings->isXFrequency()) {

            // get x
            _x = _data->frequencies_Hz();


            // at compression?
            if (_settings->isAtCompression()) {

                  if (_settings->isYVoltage()) {
                      // y: voltage
                      // x: frequency
                      // @: compression
                      _y_formatted = _data->voltageAtCompression_V;
                      return true;
                  }


                  if (_settings->isYCurrent()) {
                      // y: current
                      // x: frequency
                      // @: compression
                      _y_formatted = _data->currentAtCompression_A;
                      return true;
                  }


                  // pae or de

                  // dc power, mW
                  const QRowVector voltage_V = _data->voltageAtCompression_V;
                  const QRowVector current_A = _data->currentAtCompression_A;
                  const QRowVector denominator = dcPower_mW(voltage_V, current_A);


                  // pout, mW
                  const QRowVector pout_dBm = _data->powerOutAtCompression_dBm();
                  const QRowVector pout_mW = dBm_mW(pout_dBm);


                  // pae?
                  if (_settings->isYPowerAddedEfficiency()) {
                      // y: pae
                      // x: frequency
                      // @: compression
                      const QRowVector pin_dBm = _data->powerInAtCompression_dBm();
                      const QRowVector pin_mW = dBm_mW(pin_dBm);
                      const QRowVector numerator = subtract(pout_mW, pin_mW);
                      // pae: (pout_mW - pin_mW) / dcPower_mW
                      _y_formatted = divideEach(numerator, denominator);
                      return true;
                  }


                  // de?
                  if (_settings->isYDrainEfficiency()) {
                      // y: drain efficiency
                      // x: frequency
                      // @: compression
                      _y_formatted = divideEach(pout_mW, denominator);
                      return true;
                  }


                  // this should never happen...
                  return false;
            }


            // at max gain?
            if (_settings->isAtMaximumGain()) {

                if (_settings->isYVoltage()) {
                  // y: voltage
                  // x: frequency
                  // @: max gain
                  _y_formatted = _data->voltageAtMaxGain_V;
                  return true;
                }


                if (_settings->isYCurrent()) {
                  // y: current
                  // x: frequency
                  // @: max gain
                  _y_formatted = _data->currentAtMaxGain_A;
                  return true;
                }


                // pae or de

                // dc power, mW
                const QRowVector voltage_V = _data->voltageAtMaxGain_V;
                const QRowVector current_A = _data->currentAtMaxGain_A;
                const QRowVector denominator = dcPower_mW(voltage_V, current_A);


                // pout, mW
                const QRowVector pout_dBm = _data->powerOutAtMaxGain_dBm();
                const QRowVector pout_mW = dBm_mW(pout_dBm);


                // pae?
                if (_settings->isYPowerAddedEfficiency()) {
                  // y: pae
                  // x: frequency
                  // @: max gain

                  // numerator
                  const QRowVector pin_dBm = _data->powerInAtMaxGain_dBm();
                  const QRowVector pin_mW = dBm_mW(pin_dBm);
                  const QRowVector numerator = subtract(pout_mW, pin_mW);

                  // pae: (pout_mW - pin_mW) / dcPower_mW
                  _y_formatted = divideEach(numerator, denominator);
                  return true;
                }


                // drain efficiency?
                if (_settings->isYDrainEfficiency()) {
                  // y: drain efficiency
                  // x: frequency
                  // @: max gain
                  _y_formatted = divideEach(pout_mW, denominator);
                  return true;
                }


                // this should never happen...
                return false;
            }


            // at pin?
            if (_settings->isAtPin()) {
                // find pin
                const double pin_dBm = _settings->atValue;
                const uint iPower = _data->pin_dBm().indexOf(pin_dBm);
                if (iPower == -1) {
                  // pin not found
                  return false;
                }

                // voltage?
                if (_settings->isYVoltage()) {
                  // y: voltage
                  // x: frequency
                  // @: pin
                  _y_formatted = _data->voltage_V[iPower];
                  return true;
                }

                // current?
                if (_settings->isYCurrent()) {
                  // y: current
                  // x: frequency
                  // @: pin
                  _y_formatted = _data->current_A[iPower];
                  return true;
                }


                // pae or de


                // TODO: calculate pout_dBm
                const QRowVector gain_dB = _data->data()[iPower].y_dB(2, 1);
                const QRowVector pout_dBm = add(pin_dBm, gain_dB);
                const QRowVector pout_mW  = dBm_mW(pout_dBm);


                // calculate dc power (denominator)
                const QRowVector current_A = _data->current_A[iPower];
                const QRowVector voltage_V = _data->voltage_V[iPower];
                const QRowVector denominator = dcPower_mW(voltage_V, current_A);


                // pae?
                if (_settings->isYPowerAddedEfficiency()) {
                  // y: pae
                  // x: frequency
                  // @: pin


                  const double pin_mW = dBm_mW(pin_dBm);

                  // pae = (pout - pin)/dcPower
                  const QRowVector numerator = subtract(pout_mW, pin_mW);
                  _y_formatted = divideEach(numerator, denominator);
                  return true;
                }

                if (_settings->isYDrainEfficiency()) {
                  // y: drain efficiency
                  // x: frequency
                  // @: pin

                  // de = pout / dcPower
                  _y_formatted = divideEach(pout_mW, denominator);
                  return true;
                }

                // this should never happen...
                return false;
            }


            // at pout?
            // if (_settings->isAtPout()) {
            //     // y: pae-related
            //     // x: frequency
            //     // @: pout
            //     // TODO?
            //     return true;
            // }


            // this should not happen...
            return false;
        }


        if (_settings->isXPin()) {

            // must be at frequency
            if (!_settings->isAtFrequency()) {
                return false;
            }


            // find frequency index
            const double freq_Hz = _settings->atValue;
            const int iFreq = _data->frequencies_Hz().indexOf(freq_Hz);
            if (iFreq == -1) {
                // frequency not found
                return false;
            }


            // get pin
            const int points = _data->pin_dBm().size();
            const QRowVector pin_dBm = _data->pin_dBm();
            _x = pin_dBm;


            // y is voltage?
            if (_settings->isYVoltage()) {
                // y: voltage
                // x: pin
                // @: frequency
                _y_formatted.resize(points);
                for (int iPower = 0; iPower < points; iPower++) {
                    _y_formatted[iPower] = _data->voltage_V[iPower][iFreq];
                }
                return true;
            }


            // y is current?
            if (_settings->isYCurrent()) {
                // y: voltage
                // x: pin
                // @: frequency
                _y_formatted.resize(points);
                for (int iPower = 0; iPower < points; iPower++) {
                    _y_formatted[iPower] = _data->current_A[iPower][iFreq];
                }
                return true;
            }


            // y is pae or de


            // voltage, current
            QRowVector voltage_V(points);
            QRowVector current_A(points);
            for (int iPower = 0; iPower < points; iPower++) {
                voltage_V[iPower] = _data->voltage_V[iPower][iFreq];
                current_A[iPower] = _data->current_A[iPower][iFreq];
            }


            // dc power mW (denominator)
            const QRowVector denominator = dcPower_mW(voltage_V, current_A);


            // gain
            QRowVector gain_dB(points);
            for (int iPower = 0; iPower < points; iPower++) {
                gain_dB[iPower] = _data->data()[iPower].y_dB(2, 1)[iFreq];
            }


            // pout
            const QRowVector pout_dBm = add(pin_dBm, gain_dB);
            const QRowVector pout_mW = dBm_mW(pout_dBm);


            // y is pae?
            if (_settings->isYPowerAddedEfficiency()) {
                // y: pae
                // x: pin
                // @: frequency

                // pin mW
                const QRowVector pin_mW = dBm_mW(pin_dBm);

                // pae
                const QRowVector numerator = subtract(pout_mW, pin_mW);
                _y_formatted = divideEach(numerator, denominator);
                return true;
            }


            // y is drain efficiency?
            if (_settings->isYDrainEfficiency()) {
                // y: drain efficiency
                // x: pin
                // @: frequency
                _y_formatted = divideEach(pout_mW, denominator);
                return true;
            }


            // this should never happen...
            return false;
        }


        if (_settings->isXPout()) {

            // must be at frequency
            if (!_settings->isAtFrequency()) {
                return false;
            }


            // find frequency index
            const double freq_Hz = _settings->atValue;
            const int iFreq = _data->frequencies_Hz().indexOf(freq_Hz);
            if (iFreq == -1) {
                // frequency not found
                return false;
            }


            // get pin
            const int points = _data->pin_dBm().size();
            const QRowVector pin_dBm = _data->pin_dBm();


            // gain
            QRowVector gain_dB(points);
            for (int iPower = 0; iPower < points; iPower++) {
                gain_dB[iPower] = _data->data()[iPower].y_dB(2, 1)[iFreq];
            }


            // pout
            const QRowVector pout_dBm = add(pin_dBm, gain_dB);


            // y is voltage?
            if (_settings->isYVoltage()) {
                // y: voltage
                // x: pout
                // @: frequency
                _y_formatted.resize(points);
                for (int iPower = 0; iPower < points; iPower++) {
                    _y_formatted[iPower] = _data->voltage_V[iPower][iFreq];
                }
                return true;
            }


            // y is current?
            if (_settings->isYCurrent()) {
                // y: current
                // x: pout
                // @: frequency
                _y_formatted.resize(points);
                for (int iPower = 0; iPower < points; iPower++) {
                    _y_formatted[iPower] = _data->current_A[iPower][iFreq];
                }
                return true;
            }


            // pae or drain efficiency


            // pout mW
            const QRowVector pout_mW = dBm_mW(pout_dBm);


            // voltage, current
            QRowVector voltage_V(points);
            QRowVector current_A(points);
            for (int iPower = 0; iPower < points; iPower++) {
                voltage_V[iPower] = _data->voltage_V[iPower][iFreq];
                current_A[iPower] = _data->current_A[iPower][iFreq];
            }


            // dc power mW (denominator)
            const QRowVector denominator = dcPower_mW(voltage_V, current_A);


            // y is pae?
            if (_settings->isYPowerAddedEfficiency()) {
                // y: pae
                // x: pout
                // @: frequency
                const QRowVector pin_mW = dBm_mW(pin_dBm);
                const QRowVector numerator = subtract(pout_mW, pin_mW);
                _y_formatted = divideEach(numerator, denominator);
                return true;
            }


            // y is drain efficiency
            if (_settings->isYDrainEfficiency()) {
                // y: drain efficiency
                // x: pout
                // @: frequency
                _y_formatted = divideEach(pout_mW, denominator);
                return true;
            }


            // this should never happen...
            return false;
        }


        // this should never happen...
        return false;
    }


    // this should never happen...
    return false;
}


void ProcessTrace::updateChannel() {
    _channel = _vna->trace(_memoryTraceName).channel();
    if (_settings->isXFrequency()) {
        if (_vna->channel(_channel).sweepType() != VnaChannel::SweepType::Segmented) {
            _vna->deleteChannel(_channel);
            createChannel();
            createTrace();
            return;
        }
        else {
            _vna->channel(_channel).setFrequencies(_data->frequencies_Hz());
            if (_settings->isAtValue())
                _vna->channel(_channel).segmentedSweep().setPower(_settings->atValue);
        }
    }
    else {
        // x: dBm
        if (_vna->channel(_channel).sweepType() != VnaChannel::SweepType::Power) {
            _vna->deleteChannel(_channel);
            createChannel();
            createTrace();
            return;
        }
        else {
            _vna->channel(_channel).powerSweep().setStart(_x.first());
            _vna->channel(_channel).powerSweep().setStop(_x.last());
            _vna->channel(_channel).powerSweep().setPoints(_x.size());
            if (_settings->isAtValue())
                _vna->channel(_channel).powerSweep().setFrequency(_settings->atValue);
        }
    }
    _vna->settings().updateDisplay();
    _vna->settings().displayOn();
}


void ProcessTrace::createChannel() {
    _channel = _vna->createChannel();
    VnaChannel channel = _vna->channel(_channel);

    channel.manualSweepOn();
    channel.setName(_channelName);
    if (_settings->isXPower()) {
        channel.setSweepType(VnaChannel::SweepType::Power);
        channel.powerSweep().setStart(_x.first());
        channel.powerSweep().setStop(_x.last());
        channel.powerSweep().setPoints(_x.size());
        if (_settings->isAtValue())
            _vna->channel(_channel).powerSweep().setFrequency(_settings->atValue);
    }
    else {
        // x: Hz
        channel.setFrequencies(_x);
        if (_settings->isAtValue())
            _vna->channel(_channel).segmentedSweep().setPower(_settings->atValue);
    }
}


void ProcessTrace::createTrace() {
    const uint outputPort = _data->settings().outputPort();
    const uint inputPort = _data->settings().inputPort();
    _vna->createTrace(_dataTraceName, _channel);
    if (_settings->isYS11Trace()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, inputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
    }
    else if (_settings->isYS22Trace()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, outputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
    }
    else if (_settings->isYS21Trace()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
    }
    else if (_settings->isYS12Trace()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, inputPort, outputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
    }
    else if (_settings->isYPin()) {
        _vna->trace(_dataTraceName).setWaveQuantity(WaveQuantity::a, inputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
    }
    else if (_settings->isYPout()) {
        _vna->trace(_dataTraceName).setWaveQuantity(WaveQuantity::b, outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
    }
    else if (_settings->isYAmPm()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::UnwrappedPhase);
    }
    else if (_settings->isYVoltage()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::Magnitude);
    }
    else if (_settings->isYCurrent()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::Magnitude);
    }
    else if (_settings->isYPowerAddedEfficiency() || _settings->isYDrainEfficiency()) {
        _vna->trace(_dataTraceName).setPAEParameter(outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::Magnitude);
    }

    _vna->createDiagram(_diagram);
    _vna->trace(_dataTraceName).setDiagram(_diagram);
    _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
    _vna->trace(_memoryTraceName).setDiagram(_diagram);
    updateTrace();
    _vna->trace(_dataTraceName).hide();
}


void ProcessTrace::updateTrace() {
    const uint outputPort = _data->settings().outputPort();
    const uint inputPort = _data->settings().inputPort();
    VnaTrace trace = _vna->trace(_memoryTraceName);
    if (_settings->isYPower()) {
        if (_settings->isYPin()) {
            _vna->trace(_dataTraceName).setWaveQuantity(WaveQuantity::a, inputPort, inputPort);
            _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
            _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
//            _vna->trace(_memoryTraceName).setWaveQuantity(WaveQuantity::a, inputPort);
        }
        else /*if (_settings->isYPout())*/ {
            _vna->trace(_dataTraceName).setWaveQuantity(WaveQuantity::b, outputPort, inputPort);
            _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
            _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
//            _vna->trace(_memoryTraceName).setWaveQuantity(WaveQuantity::b, outputPort);
        }

        // if (!_vna->properties().isZvaFamily()) {
        //     trace.write(_y_formatted);
        // }
        // else {
        //     trace.write(toComplex_dBm(_y_formatted));
        // }
        trace.write(_y_formatted);
    }
    else if (_settings->isYAmPm()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::UnwrappedPhase);
        _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
//        _vna->trace(_memoryTraceName).setFormat(TraceFormat::UnwrappedPhase);

        // if (!_vna->properties().isZvaFamily()) {
        //     _vna->trace(_memoryTraceName).write(_y_formatted);
        // }
        // else {
        //     _vna->trace(_memoryTraceName).write(toComplex_deg(_y_formatted));
        // }
        trace.write(_y_formatted);
    }
    else if (_settings->isYSParameter()) {
        if (_settings->isYS11Trace()) {
            _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, inputPort, inputPort);
            _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
            _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
//            _vna->trace(_memoryTraceName).setNetworkParameter(NetworkParameter::S, inputPort, inputPort);
        }
        else if (_settings->isYS22Trace()) {
            _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, outputPort);
            _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
            _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
//            _vna->trace(_memoryTraceName).setNetworkParameter(NetworkParameter::S, outputPort, outputPort);
        }
        else if (_settings->isYS21Trace()) {
            _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
            _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
            _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
//            _vna->trace(_memoryTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
        }
        else if (_settings->isYS12Trace()) {
            _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, inputPort, outputPort);
            _vna->trace(_dataTraceName).setFormat(TraceFormat::DecibelMagnitude);
            _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
//            _vna->trace(_memoryTraceName).setNetworkParameter(NetworkParameter::S, inputPort, outputPort);
        }

        trace.write(_y_complex);
    }
    else if (_settings->isYVoltage() || _settings->isYCurrent()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::Magnitude);
        _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
        trace.write(_y_formatted);
    }
    else /*if (_settings->isYPowerAddedEfficiency() || _settings->isYDrainEfficiency())*/ {
        _vna->trace(_dataTraceName).setPAEParameter(outputPort, inputPort);
        _vna->trace(_dataTraceName).setFormat(TraceFormat::Magnitude);
        _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
        trace.write(_y_formatted);
    }
}

void ProcessTrace::debugPrint(const QString &name) {
    qDebug() << "  at \'" << name << "\':";
    qDebug() << "    x.size:     " << _x.size();
    qDebug() << "    y.size:     " << _y_complex.size();
    qDebug() << "    y_dBm.size: " << _y_formatted.size();
}

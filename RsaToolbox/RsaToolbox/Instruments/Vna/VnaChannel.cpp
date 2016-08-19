

// RsaToolbox includes
#include "General.h"
#include "IndexName.h"
#include "Vna.h"
#include "VnaChannel.h"
#include "VnaScpi.h"
using namespace RsaToolbox;

// Qt includes
#include <QDebug>

/*!
 * \class RsaToolbox::VnaChannel
 * \ingroup VnaGroup
 * \brief The \c %VnaChannel class
 * manipulates a channel.
 */

VnaChannel::VnaChannel(QObject *parent) :
    QObject(parent)
{
    placeholder.reset(new Vna());
    _vna = placeholder.data();
    _index = 0;
}
VnaChannel::VnaChannel(const VnaChannel &other) {
    if (other.isFullyInitialized()) {
        _vna = other._vna;
        _index = other._index;
    }
    else {
        placeholder.reset(new Vna());
        _vna = placeholder.data();
        _index = 0;
    }
}
VnaChannel::VnaChannel(Vna *vna, uint index, QObject *parent) :
    QObject(parent)
{
    _vna = vna;
    _index = index;
}
VnaChannel::~VnaChannel() {

}

uint VnaChannel::index() {
    return(_index);
}
QString VnaChannel::name() {
    QString scpi = ":CONF:CHAN%1:NAME?\n";
    scpi = scpi.arg(_index);
    return(_vna->query(scpi).trimmed().remove("\'"));
}
void VnaChannel::setName(QString name) {
    QString scpi = ":CONF:CHAN%1:NAME \'%2\'\n";
    scpi = scpi.arg(_index).arg(name);
    _vna->write(scpi);
}

void VnaChannel::select() {
    // Turning on a channel that already
    // exists selects it without
    // modification
    _vna->createChannel(this->_index);
}

QVector<uint> VnaChannel::diagrams() {
    return(QVector<uint>());
}
QStringList VnaChannel::traces() {
    // This function does not exist on a ZVA
    if (_vna->properties().isZvaFamily())
        return(zvaTraces());

    QString scpi = ":CONF:CHAN%1:TRAC:CAT?";
    scpi = scpi.arg(_index);
    QString result = _vna->query(scpi, 1000, 1000).trimmed();
    QVector<IndexName> indexNames;
    indexNames = IndexName::parse(result, ",", "\'");
    return(IndexName::names(indexNames));
}

// Trigger
void VnaChannel::startSweep() {
    _vna->write(":INIT:SCOP SING\n");
    QString scpi = QString(":INIT%1\n").arg(_index);
    _vna->write(scpi);
}
uint VnaChannel::numberOfSweeps() {
    QString scpi = ":SENS%1:SWE:COUN?\n";
    scpi = scpi.arg(_index);
    QString result = _vna->query(scpi);
    return(result.trimmed().toUInt());
}
void VnaChannel::setNumberOfSweeps(uint sweeps) {
    QString scpi = ":SENS%1:SWE:COUN %2\n";
    scpi = scpi.arg(_index).arg(sweeps);
    _vna->write(scpi);
}
bool VnaChannel::isSweepOn() {
    if (_vna->properties().isZvaFamily()) {
        if (_vna->isLogConnected())
            *_vna->log() << "WARNING: isSweepOn() not available on ZVA-family instrument.\n\n";
        return(true);
    }

    QString scpi = ":CONF:CHAN%1:MEAS?\n";
    scpi = scpi.arg(_index);
    return(_vna->query(scpi).trimmed() == "1");
}
bool VnaChannel::isSweepOff() {
    return(!isSweepOn());
}
void VnaChannel::sweepOn(bool isOn) {
    if (_vna->properties().isZvaFamily()) {
        if (_vna->isLogConnected())
            *_vna->log() << "WARNING: sweepOn() not available on ZVA-family instrument.\n\n";
        return;
    }

    QString scpi = ":CONF:CHAN%1:MEAS %2\n";
    scpi = scpi.arg(_index);
    if (isOn)
        scpi = scpi.arg(1);
    else
        scpi = scpi.arg(0);
    _vna->write(scpi);
}
void VnaChannel::sweepOff(bool isOff) {
    sweepOn(!isOff);
}
bool VnaChannel::isContinuousSweep() {
    QString scpi = ":INIT%1:CONT?\n";
    scpi = scpi.arg(_index);
    return(_vna->query(scpi).trimmed() == "1");
}
bool VnaChannel::isManualSweep() {
    return(!isContinuousSweep());
}
void VnaChannel::continuousSweepOn(bool isOn) {
    QString scpi = QString(":INIT%1:CONT %2\n");
    scpi = scpi.arg(_index);
    if (isOn)
        scpi = scpi.arg(1);
    else
        scpi = scpi.arg(0);
    _vna->write(scpi);
}
void VnaChannel::manualSweepOn(bool isOn) {
    continuousSweepOn(!isOn);
}
uint VnaChannel::sweepCount() {
    QString scpi = ":SENS%1:SWE:COUN?\n";
    scpi = scpi.arg(_index);
    return _vna->query(scpi).trimmed().toUInt();
}
void VnaChannel::setSweepCount(uint count) {
    QString scpi = ":SENS%1:SWE:COUN %2\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(count);
    _vna->write(scpi);
}

uint VnaChannel::sweepTime_ms() {
    SweepType type = sweepType();
    switch(type) {
    case SweepType::Linear:
        return linearSweep().sweepTime_ms();
    case SweepType::Log:
        return logSweep().sweepTime_ms();
    case SweepType::Segmented:
        return segmentedSweep().sweepTime_ms();
    case SweepType::Power:
        return powerSweep().sweepTime_ms();
    case SweepType::Cw:
        return cwSweep().sweepTime_ms();
    case SweepType::Time:
        return timeSweep().time_s() * 1000.0;
    default:
        return linearSweep().sweepTime_ms();
    }
}
uint VnaChannel::calibrationSweepTime_ms() {
    if (averaging().isOff())
        return sweepTime_ms();

    return sweepTime_ms() * averaging().number();
}

// Sweep
bool VnaChannel::isFrequencySweep() {
    VnaChannel::SweepType type = sweepType();
    switch(type) {
    case SweepType::Linear:
    case SweepType::Log:
    case SweepType::Segmented:
        return true;
    default:
        return false;
    }
}
bool VnaChannel::isLinearSweep() {
    return(sweepType() == SweepType::Linear);
}
bool VnaChannel::isLogarithmicSweep() {
    return(sweepType() == SweepType::Log);
}
bool VnaChannel::isSegmentedSweep() {
    return(sweepType() == SweepType::Segmented);
}
bool VnaChannel::isPowerSweep() {
    return(sweepType() == SweepType::Power);
}
bool VnaChannel::isCwSweep() {
    return(sweepType() == SweepType::Cw);
}
bool VnaChannel::isTimeSweep() {
    return(sweepType() == SweepType::Time);
}
VnaChannel::SweepType VnaChannel::sweepType() {
    QString scpi = ":SENS%1:SWE:TYPE?\n";
    scpi = scpi.arg(_index);
    return(VnaScpi::toSweepType(_vna->query(scpi).trimmed()));
}
void VnaChannel::setSweepType(VnaChannel::SweepType sweepType) {
    QString scpi = ":SENS%1:SWE:TYPE %2\n";
    scpi = scpi.arg(_index).arg(VnaScpi::toString(sweepType));
    _vna->write(scpi);
}
void VnaChannel::setFrequencies(QRowVector values, SiPrefix prefix) {
    VnaSegmentedSweep sweep = segmentedSweep();
    sweep.deleteSegments();
    const int points = values.size();
    for (int i = 0; i < points; i++) {
        const uint s = i + 1;
        sweep.addSegment(s);
        VnaSweepSegment segment = sweep.segment(s);
        segment.setPoints(1);
        segment.setStop(values[i], prefix);
        if (i % 1000 == 0) {
            // Adding sweep segments gets slower as
            // the number of segments goes up.
            // This can cause timeouts in later
            // commands unless we pause periodically
            // here.
            _vna->pause(5000);
        }
    }
    setSweepType(SweepType::Segmented);
    _vna->pause(5000); // For good measure
}

VnaLinearSweep &VnaChannel::linearSweep() {
    _frequencySweep.reset(new VnaLinearSweep(_vna, this));
    return(*_frequencySweep);
}
VnaLinearSweep *VnaChannel::takeLinearSweep() {
    return(new VnaLinearSweep(_vna, _index));
}
VnaLogSweep &VnaChannel::logSweep() {
    _logSweep.reset(new VnaLogSweep(_vna, this));
    return(*_logSweep);
}
VnaLogSweep *VnaChannel::takeLogSweep() {
    return(new VnaLogSweep(_vna, _index));
}
VnaSegmentedSweep &VnaChannel::segmentedSweep() {
    _segmentedSweep.reset(new VnaSegmentedSweep(_vna, this));
    return(*_segmentedSweep);
}
VnaSegmentedSweep *VnaChannel::takeSegmentedSweep() {
    return(new VnaSegmentedSweep(_vna, _index));
}
VnaPowerSweep &VnaChannel::powerSweep() {
    _powerSweep.reset(new VnaPowerSweep(_vna, this));
    return(*_powerSweep);
}
VnaPowerSweep *VnaChannel::takePowerSweep() {
    return(new VnaPowerSweep(_vna, _index));
}
VnaCwSweep &VnaChannel::cwSweep() {
    _cwSweep.reset(new VnaCwSweep(_vna, this));
    return(*_cwSweep);
}
VnaCwSweep *VnaChannel::takeCwSweep() {
    return(new VnaCwSweep(_vna, _index));
}
VnaTimeSweep &VnaChannel::timeSweep() {
    _timeSweep.reset(new VnaTimeSweep(_vna, this));
    return(*_timeSweep);
}
VnaTimeSweep *VnaChannel::takeTimeSweep() {
    return(new VnaTimeSweep(_vna, _index));
}

// Attenuators
double VnaChannel::sourceAttenuation_dB(uint port) {
    // ZVA only
    QString scpi = ":SOUR%1:POW%2:ATT?\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(port);
    QString result = _vna->query(scpi).trimmed();
    if (!result.isEmpty())
        return result.toDouble();
    else
        return 0;
}
void VnaChannel::setSourceAttenuation(double attenuation_dB, uint port) {
    // ZVA Only
    QString scpi = ":SOUR%1:POW%2:ATT %3\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(port);
    scpi = scpi.arg(attenuation_dB);
    _vna->write(scpi);
}
void VnaChannel::setSourceAttenuations(double attenuation_dB) {
    uint ports = _vna->testPorts();
    for (uint port = 1; port <= ports; port++)
        setSourceAttenuation(attenuation_dB, port);
}
double VnaChannel::receiverAttenuation_dB(uint port) {
    QString scpi = ":SENS%1:POW:ATT? %2\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(port);
    QString result = _vna->query(scpi).trimmed();
    if (!result.isEmpty())
        return result.toDouble();
    else
        return 0;
}
void VnaChannel::setReceiverAttenuation(double attenuation_dB, uint port) {
    QString scpi = ":SENS%1:POW:ATT %2,%3\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(port);
    scpi = scpi.arg(attenuation_dB);
    _vna->write(scpi);
}
void VnaChannel::setReceiverAttenuations(double attenuation_dB) {
    uint ports = _vna->testPorts();
    for (uint port = 1; port <= ports; port++)
        setReceiverAttenuation(attenuation_dB, port);
}

// Balanced ports
uint VnaChannel::numberOfLogicalPorts() {
    uint ports = _vna->testPorts();
    for (uint i = 1; i < ports; i++) {
        if (isBalancedPort(i))
            ports--;
    }
    return(ports);
}
bool VnaChannel::isSingleEndedPort(uint logicalPort) {
    return(physicalPorts(logicalPort).size() == 1);
}
uint VnaChannel::physicalPort(uint logicalPort) {
    QVector<uint> ports = physicalPorts(logicalPort);
    if (ports.size() == 1)
        return(ports.first());
    else
        return(0);
}
bool VnaChannel::isBalancedPort(uint logicalPort) {
    return(physicalPorts(logicalPort).size() == 2);
}
QVector<uint> VnaChannel::physicalPorts(uint logicalPort) {
    QString scpi = ":SOUR%1:LPOR%2?\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(logicalPort);
    return(parseUints(_vna->query(scpi).trimmed(), ","));
}
void VnaChannel::physicalPorts(uint logicalPort, uint &physicalPort1, uint &physicalPort2) {
    QVector<uint> ports = physicalPorts(logicalPort);
    if (ports.size() == 2) {
        physicalPort1 = ports.first();
        physicalPort2 = ports.last();
    }
    else {
        physicalPort1 = 0;
        physicalPort2 = 0;
    }
}
void VnaChannel::createSingleEndedPort(uint logicalPort, uint physicalPort) {
    QString scpi = ":SOUR%1:LPOR%2 %3\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(logicalPort);
    scpi = scpi.arg(physicalPort);
    _vna->write(scpi);
}
void VnaChannel::createBalancedPort(uint logicalPort, uint physicalPort1, uint physicalPort2) {
    QString scpi = ":SOUR%1:LPOR%2 %3,%4\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(logicalPort);
    scpi = scpi.arg(physicalPort1);
    scpi = scpi.arg(physicalPort2);
    _vna->write(scpi);
}
void VnaChannel::deleteBalancedPort(uint logicalPort) {
    QString scpi = ":SOUR%1:LPOR%2:CLE\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(logicalPort);
    _vna->write(scpi);
}
void VnaChannel::deleteBalancedPorts() {
    QString scpi = ":SOUR%1:LPOR:CLE ALL\n";
    scpi = scpi.arg(_index);
    _vna->write(scpi);
}

// User-defined ports
bool VnaChannel::isUserDefinedPortOn(uint physicalPort) {
    QString scpi = ":SENS%1:UDSP%2:ACT?\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(physicalPort);
    return(_vna->query(scpi).trimmed() == "1");
}
bool VnaChannel::isUserDefinedPortOff(uint physicalPort) {
    return(!isUserDefinedPortOn(physicalPort));
}
void VnaChannel::userDefinedPortOn(uint physicalPort) {
    QString scpi = ":SENS%1:UDSP%2:ACT 1\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(physicalPort);
    _vna->write(scpi);
}
void VnaChannel::userDefinedPortOff(uint physicalPort) {
    QString scpi = ":SENS%1:UDSP%2:ACT 0\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(physicalPort);
    _vna->write(scpi);
}
bool VnaChannel::isUserDefinedPort(uint physicalPort) {
    return isUserDefinedPortOn(physicalPort);
}
bool VnaChannel::isNotUserDefinedPort(uint physicalPort) {
    return(isUserDefinedPortOff(physicalPort));
}
VnaUserDefinedPort VnaChannel::userDefinedPort(uint physicalPort) {
    QString scpi = ":SENS%1:UDSP%2:PAR?\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(physicalPort);

    QString result = _vna->query(scpi).trimmed().remove("\'");
    return VnaScpi::toUserDefinedPort(result);
}
void VnaChannel::setUserDefinedPort(uint physicalPort, VnaUserDefinedPort userDefinedPort) {
    QString scpi = ":SENS%1:UDSP%2:PAR \'%3\'\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(physicalPort);
    scpi = scpi.arg(VnaScpi::toString(userDefinedPort));
    _vna->write(scpi);
    userDefinedPortOn(physicalPort);
}
void VnaChannel::deleteUserDefinedPort(uint physicalPort) {
    userDefinedPortOff(physicalPort);
}
void VnaChannel::deleteUserDefinedPorts() {
    if (_vna->properties().isZvaFamily()) {
        QVector<uint> ports = range(uint(1), _vna->testPorts());
        foreach(uint port, ports) {
            deleteUserDefinedPort(port);
        }
        return;
    }

    QString scpi = ":SENS%1:UDSP:ACT 0\n";
    scpi = scpi.arg(_index);
    _vna->write(scpi);
}

// Averaging
VnaAveraging& VnaChannel::averaging() {
    _averaging.reset(new VnaAveraging(_vna, this));
    return(*_averaging);
}

// Corrections/Calibrate
bool VnaChannel::isCalibrated() {
    QString scpi = ":SENS%1:CORR:DATA:PAR:COUN?\n";
    scpi = scpi.arg(_index);
    return(_vna->query(scpi).trimmed().toInt() > 0);
}
bool VnaChannel::isCalGroup() {
    return(calGroup().isEmpty() == false);
}
void VnaChannel::saveCalibration(QString calGroup) {
    if (calGroup.contains(".cal", Qt::CaseInsensitive) == false)
        calGroup += ".cal";

    QString scpi = ":MMEM:STOR:CORR %1,\'%2\'\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(calGroup);
    _vna->write(scpi);
}
void VnaChannel::setCalGroup(QString calGroup) {
    if (calGroup.endsWith(".cal", Qt::CaseInsensitive) == false)
        calGroup += ".cal";

    QString scpi = ":MMEM:LOAD:CORR %1,\'%2\'\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(calGroup);
    _vna->write(scpi);
}
QString VnaChannel::calGroup() {
    QString scpi = ":MMEM:LOAD:CORR? %1\n";
    scpi = scpi.arg(_index);
    return(_vna->query(scpi).trimmed().remove("\'").remove(".cal"));
}
QString VnaChannel::calGroupFile() {
    QString scpi = ":MMEM:LOAD:CORR? %1\n";
    scpi = scpi.arg(_index);
    QString filePathName = _vna->query(scpi).trimmed().remove("\'");
    filePathName = _vna->fileSystem().directory(VnaFileSystem::CAL_GROUP_DIRECTORY) + "\\" + filePathName;
    return(filePathName);
}
void VnaChannel::dissolveCalGroup() {
    QString scpi = ":MMEM:LOAD:CORR:RES %1\n";
    scpi = scpi.arg(_index);
    _vna->write(scpi);
}

// Delay Offsets
double VnaChannel::delayOffset_s(uint port) {
    QString scpi = ":SENS%1:CORR:EDEL%2:TIME?\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(port);
    return _vna->query(scpi).trimmed().toDouble();
}
QRowVector VnaChannel::delayOffsets_s() {
    uint ports = _vna->testPorts();
    QRowVector delays;
    for (uint i = 1; i <= ports; i++)
        delays << delayOffset_s(i);
    return delays;
}
void VnaChannel::setDelayOffset(uint port, double delay, SiPrefix prefix) {
    delay *= toDouble(prefix);
    QString scpi = ":SENS%1:CORR:EDEL%2:TIME %3\n";
    scpi = scpi.arg(_index);
    scpi = scpi.arg(port);
    scpi = scpi.arg(delay);
    _vna->write(scpi);
}
void VnaChannel::setDelayOffsets(QRowVector delays, SiPrefix prefix) {
    int ports = _vna->testPorts();
    if (delays.size() > ports)
        delays.resize(ports);
    for (int i = 0; i < delays.size(); i++)
        setDelayOffset(i+1, delays[i], prefix);
}
void VnaChannel::clearDelayOffset(uint port) {
    setDelayOffset(port, 0);
}
void VnaChannel::clearDelayOffsets() {
    for (uint i = 1; i <= _vna->testPorts(); i++)
        clearDelayOffset(i);
}

// Corrections
VnaCorrections &VnaChannel::corrections() {
    _corrections.reset(new VnaCorrections(_vna, this));
    return(*_corrections);
}
VnaCalibrate &VnaChannel::calibrate() {
    _calibrate.reset(new VnaCalibrate(_vna, this));
    return(*_calibrate);
}
VnaCalibrate *VnaChannel::takeCalibrate() {
    return new VnaCalibrate(_vna, _index);
}

// Power calibration
VnaPowerCorrections &VnaChannel::powerCorrections() {
    _powerCorrections.reset(new VnaPowerCorrections(_vna, this));
    return *_powerCorrections;
}

// Extension Unit
VnaExtensionUnit &VnaChannel::extensionUnit() {
    _extensionUnit.reset(new VnaExtensionUnit(_vna, this));
    return *_extensionUnit;
}

// Pulse Generator Signal
VnaPulseGenerator &VnaChannel::pulseGenerator() {
    _pulseGenerator.reset(new VnaPulseGenerator(_vna, this));
    return *_pulseGenerator;
}

// Trigger
VnaTrigger &VnaChannel::trigger() {
    _trigger.reset(new VnaTrigger(_vna, this));
    return *_trigger;
}

void VnaChannel::operator=(const VnaChannel &other) {
    if (other.isFullyInitialized()) {
        _vna = other._vna;
        _index = other._index;
    }
    else {
        placeholder.reset(new Vna());
        _vna = placeholder.data();
        _index = 0;
    }
}

//void VnaChannel::moveToThread(QThread *thread) {
//    if (_frequencySweep.isNull() == false)
//        _frequencySweep->moveToThread(thread);
//    if (_logSweep.isNull() == false)
//        _logSweep->moveToThread(thread);
//    if (_segmentedSweep.isNull() == false)
//        _segmentedSweep->moveToThread(thread);
//    if (_powerSweep.isNull() == false)
//        _powerSweep->moveToThread(thread);
//    if (_timeSweep.isNull() == false)
//        _timeSweep->moveToThread(thread);
//    if (_cwSweep.isNull() == false)
//        _cwSweep->moveToThread(thread);
//    if (_corrections.isNull() == false)
//        _corrections->moveToThread(thread);
//    if (_calibrate.isNull() == false)
//        _calibrate->moveToThread(thread);

//}

// Private
bool VnaChannel::isFullyInitialized() const {
    if (_vna == NULL)
        return(false);
    if (_vna == placeholder.data())
        return(false);
    if (_index == 0)
        return(false);
    //else
    return(true);
}
QStringList VnaChannel::zvaTraces() {
    QStringList traces = _vna->traces();
    QStringList result;
//    foreach(QString trace, traces) {
//        if (_vna->trace(trace).channel() == _index)
//            result << trace;
//    }
    return(result);
}

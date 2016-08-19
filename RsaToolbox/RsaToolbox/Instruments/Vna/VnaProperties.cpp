#include "VnaProperties.h"


// RsaToolbox
#include "General.h"
#include "Vna.h"
using namespace RsaToolbox;

// Qt
#include <QDebug>


/*!
 * \class RsaToolbox::VnaProperties
 * \ingroup VnaGroup
 * \brief The \c %VnaProperties class
 * provides the immutable properties of the
 * Rohde \& Schwarz VNA.
 */


VnaProperties::VnaProperties(QObject *parent) :
    QObject(parent)
{
    _placeholder.reset(new Vna());
    _vna = _placeholder.data();
}

VnaProperties::VnaProperties(const VnaProperties &other)
{
    _vna = other._vna;
}

VnaProperties::VnaProperties(Vna *vna, QObject *parent) :
    QObject(parent)
{
    _vna = vna;
}
VnaProperties::~VnaProperties() {

}

VnaProperties::Model VnaProperties::model() {
    QStringList idList = _vna->idString().split(',');
    if (idList.size() < 2)
        return Model::Unknown;

    QString model = idList[1].toUpper();
    if (model.contains("ZVA"))
        return Model::Zva;
    if (model.contains("ZVB"))
        return Model::Zvb;
    if (model.contains("ZVH"))
        return Model::Zvh;
    if (model.contains("ZVL"))
        return Model::Zvl;
    if (model.contains("ZVT"))
        return Model::Zvt;
    if (model.contains("ZNBT"))
        return Model::Znbt;
    if (model.contains("ZNB"))
        return Model::Znb;
    if (model.contains("ZNC"))
        return Model::Znc;
    if (model.contains("ZND"))
        return Model::Znd;
    if (model.contains("ZNP"))
        return Model::Znp;

    return Model::Unknown;
}
QString VnaProperties::serialNumber() {
    QStringList idList = _vna->idString().split(',');
    if (idList.size() < 3)
        return "";
    else
        return idList[2].trimmed();
}
QString VnaProperties::firmwareVersion() {
    QStringList idList = _vna->idString().split(',');
    if (idList.size() < 4)
        return("");
    else
        return(idList[3].trimmed());
}
QStringList VnaProperties::optionsList() {
    return(_vna->optionsString().split(',', QString::SkipEmptyParts));
}

bool VnaProperties::isZvaFamily() {
    switch(model()) {
    case Model::Zva:
    case Model::Zvb:
    case Model::Zvh:
    case Model::Zvl:
    case Model::Zvt:
        return(true);
    default:
        return(false);
    }
}
bool VnaProperties::isNotZvaFamily() {
    return(!isZvaFamily());
}
bool VnaProperties::isZnbFamily() {
    switch(model()) {
    case Model::Znb:
    case Model::Znbt:
    case Model::Znc:
    case Model::Znp:
        return(true);
    default:
        return(false);
    }
}
bool VnaProperties::isNotZnbFamily() {
    return(!isZnbFamily());
}
bool VnaProperties::isKnownModel() {
    return(!isUnknownModel());
}
bool VnaProperties::isUnknownModel() {
    return(model() == Model::Unknown);
}
uint VnaProperties::physicalPorts() {
    return(_vna->query(":INST:PORT:COUN?\n").trimmed().toUInt());
}

double VnaProperties::minimumFrequency_Hz() {
    return(_vna->query(":SYST:FREQ? MIN\n").trimmed().toDouble());
}
double VnaProperties::maximumFrequency_Hz() {
    return(_vna->query(":SYST:FREQ? MAX\n").trimmed().toDouble());
}

QRowVector VnaProperties::ifBandwidthMantissa_Hz() {
    QVector<double> mantissaValues;
    if (isZnbFamily()) {
        mantissaValues << 1
                       << 2
                       << 5
                       << 10
                       << 20
                       << 50
                       << 100
                       << 200
                       << 500;
    }
    else if (isZvaFamily()) {
        mantissaValues << 1
                       << 2
                       << 3
                       << 4
                       << 5
                       << 7
                       << 10
                       << 15
                       << 20
                       << 30
                       << 50
                       << 70
                       << 100
                       << 150
                       << 200
                       << 300
                       << 500
                       << 700;
    }
    return(mantissaValues);
}
QRowVector VnaProperties::ifBandwidthMantissa_KHz() {
    QVector<double> mantissaValues;
    if (isZnbFamily()) {
        mantissaValues << 1
                       << 2
                       << 5
                       << 10
                       << 20
                       << 50
                       << 100
                       << 200
                       << 500;
    }
    else if (isZvaFamily()) {
        mantissaValues << 1
                       << 1.5
                       << 2
                       << 3
                       << 5
                       << 7
                       << 10
                       << 15
                       << 20
                       << 30
                       << 50
                       << 70
                       << 100
                       << 150
                       << 200
                       << 300
                       << 500
                       << 700;
    }
    return(mantissaValues);
}
QRowVector VnaProperties::ifBandwidthMantissa_MHz() {
    QVector<double> mantissaValues;
    if (isZnbFamily()) {

        mantissaValues << 1
                       << 2
                       << 5
                       << 10
                       << 20
                       << 30;
    }
    else if (isZvaFamily()) {
        mantissaValues << 1
                       << 1.5
                       << 2
                       << 3
                       << 5
                       << 7
                       << 10;
    }
    return(mantissaValues);
}
QRowVector VnaProperties::ifBandwidthValues_Hz() {
    QRowVector values;
    values << ifBandwidthMantissa_Hz();
    values << multiply(ifBandwidthMantissa_KHz(), 1.0E3);
    values << multiply(ifBandwidthMantissa_MHz(), 1.0E6);
    return values;
}

double VnaProperties::minimumPower_dBm() {
    if (isZvaFamily())
        return(-150.0);
    else if (isZnbFamily())
        return(-40.0);
    else
        return(-40.0);
}
double VnaProperties::maximumPower_dBm() {
    if (isZvaFamily())
        return(100.0);
    else if (isZnbFamily())
        return(10.0);
    else
        return(10.0);
}

bool VnaProperties::hasSourceAttenuators() {
    // ZVA Only
    const uint port1 = 1;
    uint channel = _vna->createChannel();
    bool isErrorDisplayOn = _vna->settings().isErrorDisplayOn();
    bool isRfOn = _vna->settings().isRfOutputPowerOn();
    _vna->settings().errorDisplayOff();
    _vna->settings().rfOutputPowerOff();

    _vna->isError();
    _vna->clearStatus();
    _vna->channel(channel).setSourceAttenuation(0, port1);
    bool isSourceAttenuation = !_vna->isError();

    _vna->clearStatus();
    _vna->deleteChannel(channel);
    _vna->settings().errorDisplayOn(isErrorDisplayOn);
    _vna->settings().rfOutputPowerOn(isRfOn);
    return isSourceAttenuation;
}
QVector<uint> VnaProperties::sourceAttenuations_dB() {
    // ZVA Only
    const uint port1 = 1;
    QVector<uint> attenuations;
    attenuations << 0;
    uint channel = _vna->createChannel();

    bool isErrorDisplayOn = _vna->settings().isErrorDisplayOn();
    bool isRfOn = _vna->settings().isRfOutputPowerOn();
    _vna->clearStatus();
    _vna->settings().errorDisplayOff();
    _vna->settings().rfOutputPowerOff();
    _vna->channel(channel).setSourceAttenuation(0, port1);
    if (!_vna->isError()) {
        for (double i = 1; i <= 100; i++) {
            _vna->channel(channel).setSourceAttenuation(i, port1);
            double attenuation = _vna->channel(channel).sourceAttenuation_dB(port1);
            if (attenuation == i)
                attenuations << attenuation;
        }
    }

    _vna->isError();
    _vna->clearStatus();
    _vna->deleteChannel(channel);
    _vna->settings().errorDisplayOn(isErrorDisplayOn);
    _vna->settings().rfOutputPowerOn(isRfOn);
    return attenuations;
}

bool VnaProperties::hasReceiverAttenuators() {
    const uint port1 = 1;
    uint channel = _vna->createChannel();
    bool isErrorDisplayOn = _vna->settings().isErrorDisplayOn();
    bool isRfOn = _vna->settings().isRfOutputPowerOn();
    _vna->settings().errorDisplayOff();
    _vna->settings().rfOutputPowerOff();

    _vna->isError();
    _vna->clearStatus();
    _vna->channel(channel).setReceiverAttenuation(0, port1);
    bool isReceiverAttenuation = !_vna->isError();

    _vna->clearStatus();
    _vna->deleteChannel(channel);
    _vna->settings().errorDisplayOn(isErrorDisplayOn);
    _vna->settings().rfOutputPowerOn(isRfOn);
    return(isReceiverAttenuation);
}
QVector<uint> VnaProperties::receiverAttenuations_dB() {
    const uint port1 = 1;
    QVector<uint> attenuations;
    attenuations << 0;
    uint channel = _vna->createChannel();

    bool isErrorDisplayOn = _vna->settings().isErrorDisplayOn();
    bool isRfOn = _vna->settings().isRfOutputPowerOn();
    _vna->clearStatus();
    _vna->settings().errorDisplayOff();
    _vna->settings().rfOutputPowerOff();
    _vna->channel(channel).setReceiverAttenuation(0, port1);
    if (!_vna->isError()) {
        for (double i = 1; i <= 100; i++) {
            _vna->channel(channel).setReceiverAttenuation(i, port1);
            double attenuation = _vna->channel(channel).receiverAttenuation_dB(port1);
            if (attenuation == i)
                attenuations << attenuation;
        }
    }

    _vna->isError();
    _vna->clearStatus();
    _vna->deleteChannel(channel);
    _vna->settings().errorDisplayOn(isErrorDisplayOn);
    _vna->settings().rfOutputPowerOn(isRfOn);
    return attenuations;
}
uint VnaProperties::maximumPoints() {
    if (isZnbFamily())
        return(100001);
    if (isZvaFamily())
        return(60001);
    // else
    return(60001);
}

void VnaProperties::operator=(VnaProperties const &other) {
    this->_vna = other._vna;
}

QString RsaToolbox::toString(VnaProperties::Model model) {
    switch(model) {
    case VnaProperties::Model::Zva:
        return(QString("ZVA"));
    case VnaProperties::Model::Zvb:
        return(QString("ZVB"));
    case VnaProperties::Model::Zvh:
        return(QString("ZVH"));
    case VnaProperties::Model::Zvl:
        return(QString("ZVL"));
    case VnaProperties::Model::Zvt:
        return(QString("ZVT"));
    case VnaProperties::Model::Znb:
        return(QString("ZNB"));
    case VnaProperties::Model::Znbt:
        return(QString("ZNBT"));
    case VnaProperties::Model::Znc:
        return(QString("ZNC"));
    case VnaProperties::Model::Znp:
        return(QString("ZNP"));
    default:
        return(QString("UNKNOWN"));
    }
}
QString RsaToolbox::toSetFileExtension(VnaProperties::Model model) {
    switch(model) {
    case VnaProperties::Model::Zva:
    case VnaProperties::Model::Zvb:
    case VnaProperties::Model::Zvh:
    case VnaProperties::Model::Zvl:
    case VnaProperties::Model::Zvt:
        return(QString(".zvx"));
    case VnaProperties::Model::Znb:
    case VnaProperties::Model::Znbt:
    case VnaProperties::Model::Znc:
    case VnaProperties::Model::Znp:
        return(QString(".znx"));
    default:
        // UNKNOWN_MODEL
        return(QString(".rsx"));
    }
}

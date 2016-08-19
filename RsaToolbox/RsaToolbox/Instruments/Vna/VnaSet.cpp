

// RsaToolbox includes
#include "VnaSet.h"
#include "Vna.h"
using namespace RsaToolbox;

// Qt includes
// #include <Qt>


VnaSet::VnaSet(QObject *parent) :
    QObject(parent)
{
    placeholder.reset(new Vna());
    _vna = placeholder.data();
}

VnaSet::VnaSet(const VnaSet &other) :
    QObject(other.parent())
{
    _name = other._name;
    _vna = other._vna;
}

VnaSet::VnaSet(Vna *vna, QString name, QObject *parent) :
    QObject(parent)
{
    _name = name;
    _vna = vna;
}
VnaSet::~VnaSet() {
}

void VnaSet::select() {
    QString scpi = ":MEM:SEL \'%1\'\n";
    scpi = scpi.arg(_name);
    _vna->write(scpi);
}
void VnaSet::save(QString &filePathName) {
    select();

    QString extension;
    if (_vna->properties().isZvaFamily())
        extension = ".zvx";
    else
        extension = ".znx";
    if (!filePathName.endsWith(extension, Qt::CaseInsensitive))
        filePathName += extension;

    QString scpi = ":MMEM:STOR:STAT 1,\'%1\'\n";
    scpi = scpi.arg(filePathName);

    QString directory = _vna->fileSystem().directory();
    _vna->fileSystem().changeDirectory(VnaFileSystem::Directory::RECALL_SETS_DIRECTORY);

    // BUG: If you have already saved/loaded a set
    // and it has not been changed since you
    // last saved/loaded it, you cannot re-save it
    // (even if it has been deleted).
    // The firmware ignores the command.
    // I will make a small change to get past
    // this bug...
    const uint i = _vna->createChannel();
    _vna->deleteChannel(i);

    _vna->write(scpi);
    _vna->fileSystem().changeDirectory(directory);
    _vna->pause();
}

void VnaSet::operator=(const VnaSet &other) {
    if (other.isFullyInitialized()) {
        _name = other._name;
        _vna = other._vna;
    }
}

// Private
bool VnaSet::isFullyInitialized() const {
    if (_vna == NULL)
        return(false);
    if (_vna == placeholder.data())
        return(false);

    //else
    return(true);
}

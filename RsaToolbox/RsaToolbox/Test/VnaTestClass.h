#ifndef VNATESTCLASS_H
#define VNATESTCLASS_H


// RsaToolbox
#include "TestClass.h"
#include "Vna.h"

// Qt
#include <QScopedPointer>
#include <QDir>


namespace RsaToolbox {

class VnaTestClass : public TestClass
{
    Q_OBJECT
public:
    explicit VnaTestClass(QObject *parent = 0);
    ~VnaTestClass();

protected:
    QString _applicationName;
    QString _version;

    ConnectionType _connectionType;
    QString _address;
    QScopedPointer<Vna> _vna;

private slots:
    // Once
     virtual void initTestCase();
    // virtual void cleanupTestCase();

    // Per test
    virtual void init();
    virtual void cleanup();

};
}
#endif // VNATESTCLASS_H

#include "rsatoolboxtest.h"

#include <MockBus.h>
using namespace RsaToolbox;

#include <QVariant>
#include <QVariantList>

RsaToolboxTest::RsaToolboxTest(QObject *parent) : TestClass(parent)
{

}

void RsaToolboxTest::createMockBusTest() {
    QVariantList reads;
    reads << QVariant(QString("Rohde-Schwarz,ZNBT8-4Port\n"));

    MockBus bus;
    bus.write("*IDN?\n");
    bus.setReads(reads);

    QVERIFY(!bus.writes().isEmpty());
    QCOMPARE(bus.writes().first().toString(), QString("*IDN?\n"));
    QVERIFY(bus.readsLeft() > 0);
    QCOMPARE(bus.read(), QString("Rohde-Schwarz,ZNBT8-4Port\n"));
}

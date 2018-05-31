#ifndef RSATOOLBOXTEST_H
#define RSATOOLBOXTEST_H

#include <TestClass.h>
#include <QObject>

class RsaToolboxTest : public RsaToolbox::TestClass
{
    Q_OBJECT
public:
    explicit RsaToolboxTest(QObject *parent = nullptr);

private slots:
    void createMockBusTest();
};

#endif // RSATOOLBOXTEST_H

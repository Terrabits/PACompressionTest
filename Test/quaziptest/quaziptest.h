#ifndef QuaZipTest_H
#define QuaZipTest_H

#include <TestClass.h>
#include <QObject>

class QuaZipTest : public RsaToolbox::TestClass
{
    Q_OBJECT
public:
    explicit QuaZipTest(QObject *parent = nullptr);

private slots:
    void createZip();
};

#endif // QuaZipTest_H

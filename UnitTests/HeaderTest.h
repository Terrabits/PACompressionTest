#ifndef HEADERTEST_H
#define HEADERTEST_H


// RsaToolbox
#include <Definitions.h>

// Qt
#include <QDir>
#include <QObject>


class HeaderTest : public QObject
{
    Q_OBJECT
public:
    explicit HeaderTest(QObject *parent = 0);
    ~HeaderTest();

private slots:
    // Once
//    void initTestCase();
//    void cleanupTestCase();

    // Once per test
//    void init();
//    void cleanup();

    void pulseSettings();

private:
    QDir _sourceDir;
    QString _ipAddress;

};

#endif // HEADERTEST_H

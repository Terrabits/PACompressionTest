#ifndef PROCESSTRACETEST_H
#define PROCESSTRACETEST_H

// Qt
#include <QString>
#include <QObject>

class ProcessTraceTest : public QObject
{
    Q_OBJECT
public:
    explicit ProcessTraceTest(QObject *parent = 0);
    ~ProcessTraceTest();

private slots:
    void pinAtCompression_data();
    void pinAtCompression();

private:
    QString _filename1;
    QString _filename2;
    QString _filename3;
    QString _filename4;
};

#endif // PROCESSTRACETEST_H

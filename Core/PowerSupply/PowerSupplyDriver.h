#ifndef POWERSUPPLYDRIVER_H
#define POWERSUPPLYDRIVER_H

// Qt
#include <QMetaType>
#include <QString>
#include <QStringList>


class PowerSupplyDriver
{

public:

    PowerSupplyDriver();
    PowerSupplyDriver(const PowerSupplyDriver &other);
    PowerSupplyDriver(const QString &filename);


    bool isOpen() const;
    bool isValid() const;
    bool isValid(QString &errorMessage) const;
    QString filename() const;


    bool        preset;
    QStringList setupScpi;
    QString     queryVoltageScpi;
    QString     queryCurrentScpi;


    void operator=(const PowerSupplyDriver &other);


private:

    // file info
    QString _filename;
    bool    _isOpen;
    bool    isOpenError() const;
    QString _openError;


    // load
    void init();
    void clearKeys();
    bool open(const QString &filename);


    // for copy constructor, operator=
    void copy(const PowerSupplyDriver &other);
};

#endif // POWERSUPPLYDRIVER_H

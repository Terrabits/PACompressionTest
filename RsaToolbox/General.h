#ifndef GENERAL_H
#define GENERAL_H


// Rsa
#include "Definitions.h"

// Qt
#include <QDir>
#include <QString>

// C++ std lib
#include <complex>
#include <vector>

namespace RsaToolbox {

// Enum conversions
double ToDouble(SiPrefix prefix);
QString ToString(ComplexFormat format);
QString ToString(NetworkParameter parameter);
QString ToString(SiPrefix prefix);
QString ToString(Units units);
QString ToString(ConnectionType connection_type);
QString ToString(VnaModel model);
QString ToSetFileExtension(VnaModel model);
SiPrefix String_To_SiPrefix(QString prefix);

// File system
QString AppendPath(QDir path, QString filename);
QString AppendCurrentDirectory(QString filename);
QString GetAppDataPath(QString program_folder);
QString AppendAppDataPath(QString program_folder, QString filename);

// Formatting Functions
QString FormatValue(double value, int decimalPlaces, Units units, SiPrefix prefix = NO_PREFIX);

// Math
double ToDb(double value);
double ToDb(std::complex<double> value);
double ToMagnitude(double decibels);
void LinearSpacing(QRowVector &result, double start, double stop, int points);
void LinearSpacing(RowVector &result, double start, double stop, int points);
double LinearInterpolateX(double x1, double y1, double x2, double y2, double y_desired);
double LinearInterpolateY(double x1, double y1, double x2, double y2, double x_desired);
}


#endif // GENERAL_H

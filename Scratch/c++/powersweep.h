#include "powerpoint.h"
#include <QVector>

class PowerSweep {
public:
  PowerSweep();
  PowerSweep(const QVector<PowerPoints> &points);
  PowerSweep(const PowerSweep &other);

  bool isEmpty() const;

  QVector<PowerPoint> points;

  RsaToolbox::QRowVector power_dBm() const;
  RsaToolbox::QMatrix3D          s() const;
  RsaToolbox::ComplexRowVector   s(uint receiver, uint source) const;

  void sort();
}

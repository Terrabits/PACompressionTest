#include "freqpoint.h"
#include <QVector>

class FreqSweep {
public:
  FreqSweep();
  FreqSweep(const QVector<FreqPoint> &points);
  FreqSweep(const FreqSweep &other);

  bool isEmpty() const;

  QVector<FreqPoint> points;
  RsaToolbox::QRowVector freq_Hz() const;
  RsaToolbox::QMatrix3D        s() const;
  RsaToolbox::ComplexRowVector s(uint receiver, uint source) const;

  void sort();
}

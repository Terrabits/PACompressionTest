#include "freqpoint.h"
#include "freqsweep.h"
#include "point.h"
#include "powerpoint.h"
#include "powersweep.h"
#include <QVector>

class Measurement {
public:
  Measurement();
  Measurement(const QVector<Points> &points);
  Measurement(const Measurement &other);

  QVector<Point> points;

  bool isEmpty() const;
  void add(FreqPoint  point);
  void add(PowerPoint point);
  void add(const FreqSweep  &sweep, double power_dBm);
  void add(const PowerSweep &sweep, double freq_Hz  );

  RsaToolbox::QRowVector freq_Hz  () const;
  RsaToolbox::QRowVector power_dBm() const;

  PowerSweep atFreq (double freq_Hz  );
  FreqSweep  atPower(double power_dBm);

  void sort();
}

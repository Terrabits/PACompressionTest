#include <freqpoint.h>
#include <powerpoint.h>
#include <Definitions.h>

class Point {
public:
  Point();
  Point(double freq_Hz, double power_dBm, RsaToolbox::QMatrix2D s);

  double freq_Hz
  double power_dBm
  RsaToolbox::QMatrix2D s;

  uint ports() const;

  PowerPoint toPowerPoint() const;
  FreqPoint  toFreqPoint () const;
}

bool operator<(const Point &left, const Point &right);

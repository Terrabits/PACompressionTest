#include <Definitions.h>

class FreqPoint {
public:
  FreqPoint();
  FreqPoint(double freq_Hz, RsaToolbox::QMatrix2D);

  double freq_Hz;
  RsaToolbox::QMatrix2D s;

  uint ports() const;
}

bool operator<(const FreqPoint &right, const FreqPoint &left);

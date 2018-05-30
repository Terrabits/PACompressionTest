#include <Definitions.h>

class PowerPoint {
public:
  PowerPoint();
  PowerPoint(double power_dBm, RsaToolbox::QMatrix2D s);

  double power_dBm;
  RsaToolbox::QMatrix2D s;
}

bool operator<(const PowerPoint &left, const PowerPoint &right);

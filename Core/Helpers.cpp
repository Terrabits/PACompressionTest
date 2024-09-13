#include "Helpers.h"


// RsaToolbox
using namespace RsaToolbox;


// std lib
#include <cmath>
#include <stdexcept>


QRowVector add(double left, const QRowVector& right)
{

    // result vector
    const int size = right.size();
    QRowVector result(size);

    // add
    for (int i = 0; i < size; i++)
    {
      result[i] = left + right[i];
    }

    return result;
}


double dcPower_mW(double voltage_V, double current_A)
{
    return 1000.0 * voltage_V * current_A;
}


QRowVector dcPower_mW(const QRowVector& voltage_V,const QRowVector& current_A)
{
    if (voltage_V.size() != current_A.size())
    {
        throw std::invalid_argument("voltage and current vector sizes must be equal");
    }

    // result vector
    const int size = voltage_V.size();
    QRowVector result(size);

    for (int i = 0; i < size; i++)
    {
        result[i] = dcPower_mW(voltage_V[i], current_A[i]);
    }
    return result;
}


double dBm_mW(double power_dBm)
{
    return pow(10.0, power_dBm / 10.0);
}


QRowVector dBm_mW(const QRowVector& power_dBm)
{
    // result vector
    const int size = power_dBm.size();
    QRowVector result(size);

    // convert dBm to mW
    for (int i = 0; i < size; i++)
    {
        result[i] = dBm_mW(power_dBm[i]);
    }
    return result;
}

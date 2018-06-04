#ifndef TOCOMPLEX_H
#define TOCOMPLEX_H

#include <Definitions.h>
#include <General.h>

RsaToolbox::ComplexRowVector toComplex(RsaToolbox::QRowVector values) {
    // 0 dBm: sqrt(5.0)/10.0 ...?????
    const RsaToolbox::ComplexDouble one(1, 0);
    return RsaToolbox::multiply(values, one);
}
RsaToolbox::ComplexRowVector toComplex_dBm(RsaToolbox::QRowVector values_dBm) {
    // 0 dBm: sqrt(5.0)/10.0 ...?????
    const RsaToolbox::ComplexDouble zero_dBm(0.223606797749979, 0);
    return RsaToolbox::multiply(RsaToolbox::toMagnitude(values_dBm), zero_dBm);
}
RsaToolbox::ComplexRowVector toComplex_deg(RsaToolbox::QRowVector values_deg) {
    RsaToolbox::ComplexRowVector _result(values_deg.size());
//    _result.resize(values_deg.size());
    for (int i = 0; i < values_deg.size(); i++) {
        const double rad = values_deg[i] * RsaToolbox::PI / 180.0;
        _result[i] = RsaToolbox::ComplexDouble(cos(rad), sin(rad));
    }
    return _result;
}

#endif // TOCOMPLEX_H

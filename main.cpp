

// RsaPaCompressionTest
#include "Settings.h"
#include "mainwindow.h"
#include "TraceSettings.h"
#include "TracesWidget.h"
#include "MeasurementData.h"
#include "ProcessTrace.h"
#include "PlotWidget.h"

// RsaToolbox
#include "Definitions.h"
#include "General.h"
#include "Log.h"
#include "Vna.h"
#include "Keys.h"
#include "Figure.h"

// Qt
#include <QApplication>
#include <QMessageBox>
#include <QLocale>


using namespace RsaToolbox;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Get key instance
    Keys keys(KEY_PATH);

    // Log
    Log log(LOG_FILENAME, APP_NAME, APP_VERSION);
    log.printHeader();

    // Sample data
//    MeasurementData data;
//    data.inputPort = 1;
//    data.outputPort = 2;
//    data.startPower_dBm = 0;
//    data.stopPower_dBm = 10;
//    data.powerPoints = 51;
//    data.startFreq_Hz = 1E9;
//    data.stopFreq_Hz = 6E9;
//    data.frequencyPoints = 51;
//    data.ifBw_Hz = 10E3;
//    data.compressionLevel_dB = 1.0;
//    data.processSettings();
//    data.resizeToPoints();
//    for (uint iPower = 0; iPower < data.powerPoints; iPower++) {
//        data.data[iPower].resize(2, data.frequencyPoints);
//        for (uint iFreq = 0; iFreq < data.frequencyPoints; iFreq++) {
//            data.data[iPower].y()[iFreq][0][0] = fromDbDegrees(-15, -45);
//            data.data[iPower].y()[iFreq][0][1] = fromDbDegrees(-40, 0);
//            data.data[iPower].y()[iFreq][1][1] = fromDbDegrees(-10, -45);

//            const double p = data.power_dBm[iPower];
//            const double mag_dB = -0.1*pow(p, 3.0) + 0.7*pow(p, 2.0) + 1.8*p + 5.1;
//            const double phase_deg = 200.0 - 20.0 * iFreq;
//            data.data[iPower].y()[iFreq][1][0] = fromDbDegrees(mag_dB, phase_deg);
//        }
//    }
//    data.calculateMetrics();

//    TraceSettings settings;
//    settings.yAxis = "Left";
//    settings.yParameter = "Input Reflection";
//    settings.yFormat = "VSWR";
//    settings.xParameter = "Frequency";
//    settings.atParameter = "Pin";
//    settings.atValue = 5.0;

//    if (!settings.isValid()) {
//        qDebug() << "Invalid trace settings!";
//        qDebug() << "  Y Axis: " << settings.isValidYAxis();
//        qDebug() << "  Y Parameter: " << settings.isValidYParameter();
//        qDebug() << "  Y Format: " << settings.isValidYFormat();
//        qDebug() << "  X Parameter: " << settings.isValidXParameter();
//        qDebug() << "  At Parameter: " << settings.isValidAtParameter();
//        return 0;
//    }

//    ProcessTrace process(settings, data);

//    Figure figure(1, 1);
//    figure.select(1,1);
//    figure.addTrace(process.x(), process.y(), Qt::red);
//    figure.show();
    
//    qDebug() << "TraceSettingsModel";
//    TraceSettingsModel model;

//    qDebug() << "TracesWidget";
//    TracesWidget tracesWidget;
//    tracesWidget.setModel(&model);
//    tracesWidget.show();

//    qDebug() << "PlotWidget";
//    PlotWidget plotWidget;
//    plotWidget.setData(&data);
//    plotWidget.setModel(&model);
//    plotWidget.show();
//    plotWidget.replot();

    // Create, display window
    MainWindow w(&keys, &log);
    w.show();

    return app.exec();
}


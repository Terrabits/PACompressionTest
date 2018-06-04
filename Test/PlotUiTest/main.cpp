

// RsaPaCompressionTest
#include "Settings.h"
#include "mainwindow.h"
#include "MeasurementData.h"

// RsaToolbox
#include "Log.h"
#include "Vna.h"
#include "Keys.h"
using namespace RsaToolbox;

// Qt
#include <QApplication>
#include <QMessageBox>


bool isConnected(Vna &vna);
bool isKnownModel(Vna &vna);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Vna vna(CONNECTION_TYPE, INSTRUMENT_ADDRESS);
    vna.startLog(LOG_FILENAME, APP_NAME, APP_VERSION);
    if (!isConnected(vna) || !isKnownModel(vna)) {
        return 1;
    }

    Keys keys(KEY_PATH);

//    QDir sourceDir(SOURCE_DIR);
//    MeasurementData data;
//    qDebug() << "Opening " << sourceDir.filePath("measurementData.dat");
//    qDebug() << "Opened? " << data.open(sourceDir.filePath("measurementData.dat"));
//    data.createExportFileHeader(vna);
//    data.exportToZip(sourceDir.filePath("exportedMeasurementData.zip"));
//    return 0;

    MainWindow w(vna, keys);
    w.show();
    return app.exec();
}

bool isConnected(Vna &vna) {
    if (vna.isOpen() && vna.isResponding()) {
        return true;
    }

    QString msg = "Instrument not found.\n";
    msg += "Please run this application on the instrument.";
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return false;
}
bool isKnownModel(Vna &vna) {
    if (vna.properties().isKnownModel()) {
        return true;
    }

    QString msg = "Instrument not recognized.\n";
    msg += "Please use %1 with a Rohde & Schwarz VNA";
    msg = msg.arg(APP_NAME);
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return false;
}


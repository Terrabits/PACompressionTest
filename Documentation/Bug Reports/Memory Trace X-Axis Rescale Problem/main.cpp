

// VnaXAxisRescaleBug
#include "Settings.h"

// RsaToolbox
#include "Log.h"
#include "Vna.h"
#include "Keys.h"
using namespace RsaToolbox;

// Qt
#include <QString>
#include <QApplication>
#include <QMessageBox>


bool isNoConnection(Vna &vna);
bool isUnknownModel(Vna &vna);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Log log(LOG_FILENAME, APP_NAME, APP_VERSION);
    log.printHeader();

    Vna vna(CONNECTION_TYPE, INSTRUMENT_ADDRESS);
    vna.useLog(&log);
    vna.printInfo();

    Keys keys(KEY_PATH);

    if (isNoConnection(vna) || isUnknownModel(vna))
        return 0;


    // Start from preset
    vna.write("*RST\n");
    vna.query("*OPC?\n");

    // Manual sweep mode
    vna.write("INIT1:CONT 0\n");

    // Create diagram #2
    vna.write("DISP:WIND2:STAT 1\n");

    // Create channel 2
    vna.write("CONF:CHAN2 1\n");

    // Set power sweep
    // Note: Does this with linear frequency sweep also
    vna.write("SENS2:SWE:TYPE POW\n");

    // Set start, stop, points
    vna.write("SOUR2:POW:STAR -20\n");
    vna.write("SOUR2:POW:STOP 0\n");
    vna.write("SENS2:SWE:POIN 3\n");

    // Create data trace
    vna.write("CALC2:PAR:SDEF \'data_trace\', \'S21\'\n");

    // data -> memory
    vna.write("TRAC:COPY \'memory_trace\', \'data_trace\'\n");

    // Write custom values to memory trace
    vna.write("CALC2:PAR:SEL \'memory_trace\'\n");
    vna.write("CALC2:DATA SDAT,1,0,1,0,1,0\n"); // 0 dB, 0 deg

    // Assign memory trace a diagram area
    vna.write("DISP:WIND2:TRAC:EFE \'memory_trace\'\n");

    // Change x-axis
    vna.write("SOUR2:POW:STAR -30\n");
    vna.write("SOUR2:POW:STOP 10\n");

    // Write new data
    vna.write("CALC2:DATA SDAT,2,0,2,0,2,0\n"); // ~6 dB, 0 deg

    qDebug() << "Error? " << vna.isError();
    vna.clearStatus();
    return 0;
}


bool isNoConnection(Vna &vna) {
    if (vna.isConnected() && !vna.idString().isEmpty())
        return false;

    QString msg = "Instrument not found.\n";
    msg += "Please run this application on the instrument.";
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return true;
}
bool isUnknownModel(Vna &vna) {
    if (vna.properties().isKnownModel())
        return false;

    QString msg = "Instrument not recognized.\n";
    msg += "Please use %1 with a Rohde & Schwarz VNA";
    msg = msg.arg(APP_NAME);
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return true;
}


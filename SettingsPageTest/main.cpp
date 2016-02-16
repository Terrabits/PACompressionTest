

// RsaPaCompressionTest
#include "Settings.h"
#include "SettingsPage.h"

// RsaToolbox
#include "Log.h"
#include "Vna.h"
#include "Keys.h"
using namespace RsaToolbox;

// Qt
#include <QApplication>
#include <QMessageBox>


bool isNoConnection(Vna &vna);
bool isUnknownModel(Vna &vna);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Log log(LOG_FILENAME, APP_NAME, APP_VERSION);
    log.printHeader();

    Vna vna(CONNECTION_TYPE, INSTRUMENT_ADDRESS);
    vna.useLog(&log);
    vna.printInfo();

    Keys keys(KEY_PATH);

    if (isNoConnection(vna) || isUnknownModel(vna))
            return(0);

    SettingsPage page;
    page.setVna(&vna);
    page.setKeys(&keys);
    page.loadKeys();
    page.show();

    int result = app.exec();

    if (page.hasAcceptableInput())
        page.saveKeys();

    return result;
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


#include "helpers.hpp"

// Core
#include "Settings.h"

// Logging
#include "logging.hpp"

// RsaToolbox
#include "About.h"
#include "Vna.h"
using namespace RsaToolbox;

// Qt
#include <QMessageBox>


bool isAboutMenu(int argc, char *argv[]) {
    if (argc != 2)
        return false;

    QString arg(argv[1]);
    arg = arg.trimmed().toUpper();
    if (arg == "-ABOUT" || arg == "--ABOUT") {
        LOG(info) << "Displaying About menu";
        Q_INIT_RESOURCE(AboutResources);
        About about;
        about.setAppName(APP_NAME);
        about.setVersion(APP_VERSION);
        about.setDescription(APP_DESCRIPTION);
        about.setContactInfo(CONTACT_INFO);
        about.exec();
        return true;
    }

    return false;
}
bool isNoConnection(Vna &vna) {
    if (vna.isConnected() && !vna.idString().isEmpty())
        return false;

    LOG(error) << "instrument not found";
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

    LOG(error) << "instrument make, model not recognized";
    QString msg = "Instrument not recognized.\n";
    msg += "Please use %1 with a Rohde & Schwarz VNA";
    msg = msg.arg(APP_NAME);
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return true;
}

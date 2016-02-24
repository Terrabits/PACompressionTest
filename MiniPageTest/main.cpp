


// Project
#include "MiniPage.h"

// RsaToolbox
using namespace RsaToolbox;

// Qt
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(CoreResources);

    MiniPage page;
    page.setWindowFlags(page.windowFlags() | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    page.show();
    return app.exec();
}

#ifndef LANDINGPAGE
#define LANDINGPAGE


// RsaToolbox
#include "WizardPage.h"
#include "LastPath.h"


namespace Ui {
class LandingPage;
}

class LandingPage : public RsaToolbox::WizardPage
{
    Q_OBJECT

public:
    explicit LandingPage(QWidget *parent = 0);
    ~LandingPage();

    void initialize();
    bool isReadyForNext();
    int next();

private slots:
    void linkActivated(const QString &url);

private:
    Ui::LandingPage *ui;
    QString _url;
    // Data *_data;
    RsaToolbox::LastPath _lastPath;

    bool open();
    void initNew();
    void initOpen();
    void initPlot();
};

#endif // LANDINGPAGE

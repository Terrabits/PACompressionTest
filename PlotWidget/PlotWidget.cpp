#include "PlotWidget.h"
#include "ui_PlotWidget.h"


// Project
#include "ProcessTrace.h"

// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QStringList>
#include <QFileInfo>
#include <QDebug>


PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget),
    _model(0),
    _title(0)
{
    ui->setupUi(this);
}

PlotWidget::~PlotWidget()
{
    delete ui;
}

void PlotWidget::setModel(TraceSettingsModel *model) {
    if (_model == model)
        return;

    if (_model) {
        disconnect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(replot()));
        disconnect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(replot()));
        disconnect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(replot()));
        disconnect(_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(replot()));
        disconnect(_model, SIGNAL(modelReset()),
                this, SLOT(replot()));
    }

    _model = model;
    if (_model) {
        connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(replot()));
        connect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(replot()));
        connect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(replot()));
        connect(_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(replot()));
        connect(_model, SIGNAL(modelReset()),
                this, SLOT(replot()));
    }
}
void PlotWidget::setData(MeasurementData *data) {
    _data = data;
}

void PlotWidget::replot() {
    qDebug() << "PlotWidget::replot";
    clearPlot();

    if (_model->traces().isEmpty()) {
        qDebug() << "  No traces. Exiting...";
        return;
    }

    // Title
    qDebug() << "  Title";
    if (isTitle()) {
        if (!_title) {
            // Add
            _title  = new QCPPlotTitle(ui->plot, "");
            ui->plot->plotLayout()->insertRow(0);
            ui->plot->plotLayout()->addElement(0, 0, _title);
        }
        _title->setVisible(true);
        _title->setText(generateTitle());
    }
    else if (_title) {
        _title->setVisible(false);
        if (ui->plot->plotLayout()->remove(_title))
            _title = 0;
        ui->plot->plotLayout()->simplify();
    }

    // Legend
    qDebug() << "  Legend";
    if (isHideLegend()) {
        qDebug() << "  Hiding legend";
        ui->plot->legend->setVisible(false);
    }
    else {
        qDebug() << "  Showing legend";
        ui->plot->legend->setVisible(true);
        ui->plot->legend->setFont(QFont("Helvetica", 9));
        ui->plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight | Qt::AlignBottom);
    }

    double yLeftMin, yLeftMax;
    generateLeftYAxis(yLeftMin, yLeftMax);

    double yRightMin, yRightMax;
    generateRightYAxis(yRightMin, yRightMax);

    double xMin, xMax;
    SiPrefix xPrefix;
    generateXAxis(xMin, xMax, xPrefix);


    qDebug() << "  Traces";
    foreach (TraceSettings t, _model->traces()) {
        ProcessTrace p(t, *_data);
        qDebug() << "  processing trace: " << p.name();
        QCPGraph *graph;
        if (t.isLeftYAxis()) {
            graph = ui->plot->addGraph();
            ui->plot->yAxis->setVisible(true);
        }
        else {
            graph = ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2);
            ui->plot->yAxis2->setVisible(true);
        }
        graph->setName(p.name());
        graph->setPen(QPen(t.color));
        graph->setData(p.x(xPrefix), p.y());
    }


    // Left Y Axis
    qDebug() << "  Left Y Axis";
    if (isLeftYAxisDegrees()) {
        qDebug() << "  Left Y Axis: deg";
        ui->plot->yAxis->setRange(-180.0, 180.0);
        ui->plot->yAxis->setAutoTickStep(false);
        ui->plot->yAxis->setTickStep(45.0);
        ui->plot->yAxis->setAutoSubTicks(false);
        ui->plot->yAxis->setSubTickCount(2);
    }
    else if (yLeftMin < yLeftMax) {
        qDebug() << "  Left Y Axis: Pretty";
        double tickStep = 1.0;
        int subTickCount = 4;
        prettyAxis(yLeftMin, yLeftMax, tickStep, subTickCount);
        qDebug() << "  " << yLeftMin << yLeftMax << tickStep << subTickCount;
        ui->plot->yAxis->setRange(yLeftMin, yLeftMax);
        ui->plot->yAxis->setAutoTickStep(false);
        ui->plot->yAxis->setTickStep(tickStep);
        ui->plot->yAxis->setAutoSubTicks(false);
        ui->plot->yAxis->setSubTickCount(subTickCount);
    }
    else {
        qDebug() << "  Left Y Axis: Autoscale";
        ui->plot->yAxis->rescale();
    }

    // Right Y Axis
    qDebug() << "   Right Y Axis";
    if (isRightYAxisDegrees()) {
        ui->plot->yAxis2->setRange(-180.0, 180.0);
        ui->plot->yAxis2->setAutoTickStep(false);
        ui->plot->yAxis2->setTickStep(45.0);
        ui->plot->yAxis2->setAutoSubTicks(false);
        ui->plot->yAxis2->setSubTickCount(2);
    }
    else if (yRightMin < yRightMax) {
        double tickStep = 1.0;
        int subTickCount = 4;
        prettyAxis(yRightMin, yRightMax, tickStep, subTickCount);
        ui->plot->yAxis2->setRange(yRightMin, yRightMax);
        ui->plot->yAxis2->setAutoTickStep(false);
        ui->plot->yAxis2->setTickStep(tickStep);
        ui->plot->yAxis2->setAutoSubTicks(false);
        ui->plot->yAxis2->setSubTickCount(subTickCount);
    }
    else {
        qDebug() << "  YAxis2 autoscale";
        ui->plot->yAxis2->rescale();
    }

    // X Axis
    qDebug() << "  X Axis";
    ui->plot->xAxis->setRange(xMin, xMax);
    ui->plot->xAxis->setAutoTickStep(false);
    ui->plot->xAxis->setAutoSubTicks(false);
    if (xMax - xMin < 5.0) {
        ui->plot->xAxis->setTickStep(1.0);
        ui->plot->xAxis->setSubTickCount(4);
    }
    else if (xMax - xMin < 10.0) {
        ui->plot->xAxis->setTickStep(2.0);
        ui->plot->xAxis->setSubTickCount(3);
    }
    else {
        ui->plot->xAxis->setTickStep(10.0);
        ui->plot->xAxis->setSubTickCount(4);
    }

    qDebug() << "  Labels";
    ui->plot->xAxis->setLabel(generateXLabel(xPrefix));
    ui->plot->yAxis->setLabel(generateLeftYLabel());
    ui->plot->yAxis2->setLabel(generateRightYLabel());
    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->plot->setInteractions(QCP::iSelectPlottables);
    ui->plot->replot();
}
bool PlotWidget::saveToFile(QString filename) const {
    QFileInfo fileInfo(filename);
    QString fileExtension = fileInfo.suffix().toLower();
    if (fileExtension == "pdf")
        return ui->plot->savePdf(filename);
    else if (fileExtension == "jpg" || fileExtension == "jpeg")
        return ui->plot->saveJpg(filename);
    else if (fileExtension == "png")
        return ui->plot->savePng(filename);
    else if (fileExtension == "bmp")
        return ui->plot->saveBmp(filename);
    else
        return ui->plot->saveJpg(filename + ".jpg");
}
void PlotWidget::clearPlot() {
    ui->plot->clearGraphs();
    ui->plot->clearItems();
    ui->plot->legend->clearItems();

    ui->plot->yAxis->setAutoTickStep(true);
    ui->plot->yAxis->setAutoSubTicks(true);
    ui->plot->yAxis->setVisible(false);

    ui->plot->yAxis2->setAutoTickStep(true);
    ui->plot->yAxis2->setAutoSubTicks(true);
    ui->plot->yAxis2->setVisible(false);
}

// Pretty Plot Helpers
bool PlotWidget::isTitle() const {
    return !generateTitle().isEmpty();
}
QString PlotWidget::generateTitle() const {
    if (_model->traces().isEmpty())
        return QString();

    if (_model->traces().size() == 1)
        return ProcessTrace(_model->traces().first(), *_data).name();

    bool isReflection = false;
    bool isInsertion = false;
    bool isPower = false;

    bool isAtMaxGain = false;
    bool isAtCompression = false;
    bool isAtFrequency = false;
    double frequencyValue;
    bool isAtPin = false;
    double pinValue;
    bool isAtSameValue = true;

    foreach (TraceSettings t, _model->traces()) {
        if (t.isYReflection())
            isReflection = true;
        if (t.isYInsertion())
            isInsertion = true;
        if (t.isYPower())
            isPower = true;
        if (t.isAtMaximumGain())
            isAtMaxGain = true;
        if (t.isAtCompression())
            isAtCompression = true;
        if (t.isAtFrequency()) {
            if (!isAtFrequency) {
                isAtFrequency = true;
                frequencyValue = t.atValue;
            }
            else {
                if (frequencyValue != t.atValue)
                    isAtSameValue = false;
            }
        }
        if (t.isAtPin()) {
            if (!isAtPin) {
                isAtPin = true;
                pinValue = t.atValue;
            }
            else {
                if (pinValue != t.atValue)
                    isAtSameValue = false;
            }
        }
    }

    uint yTypes = 0;
    if (isReflection)
        yTypes++;
    if (isInsertion)
        yTypes++;
    if (isPower)
        yTypes++;

    uint atTypes = 0;
    if (isAtMaxGain)
        atTypes++;
    if (isAtCompression)
        atTypes++;
    if (isAtFrequency)
        atTypes++;
    if (isAtPin)
        atTypes++;

    QString _title;
    if (yTypes == 1) {
        if (isReflection)
            _title = "Reflections";
        if (isInsertion)
            _title = "Gain";
        if (isPower)
            _title = "Pin, Pout";

        if (atTypes == 1 && isAtSameValue) {
            if (isAtMaxGain)
                _title += " at Maximum Gain";
            if (isAtCompression)
                _title += " at Compression";
            if (isAtFrequency)
                _title += " at " + formatValue(frequencyValue, 3, Units::Hertz);
            if (isAtPin)
                _title += " at " + formatValue(pinValue, 2, Units::NoUnits) + " dBm";
        }
    }
    return _title;
}
bool PlotWidget::isHideLegend() const {
    return _model->traces().size() == 1;
}

void PlotWidget::generateXAxis(double &min, double &max, SiPrefix &prefix) const {
    min = DBL_INF;
    max = DBL_NEG_INF;
    bool isFrequency = false;
    foreach (TraceSettings t, _model->traces()) {
        if (t.isXFrequency())
            isFrequency = true;

        ProcessTrace p(t, *_data);
        const double newMin = RsaToolbox::min(p.x());
        if (newMin < min)
            min = newMin;
        const double newMax = RsaToolbox::max(p.x());
        if (newMax > max)
            max = newMax;
    }

    if (!isFrequency) {
        prefix = SiPrefix::None;
    }
    else {
        prefix = getPrefix(max);
        min = min / toDouble(prefix);
        max = max / toDouble(prefix);
    }
}
QString PlotWidget::generateXLabel(SiPrefix prefix) const {
    bool isFrequency = false;
    bool isPin = false;
    foreach (TraceSettings t, _model->traces()) {
        if (t.isXFrequency())
            isFrequency = true;
        if (t.isXPin())
            isPin = true;
    }

    QStringList units;
    if (isFrequency)
        units << toString(prefix, Units::Hertz);
    if (isPin)
        units << toString(prefix, Units::dBm);
    return toString(units, ", ");
}

bool PlotWidget::isLeftYAxisDegrees() const {
    foreach (TraceSettings t, _model->traces()) {
        if (t.isLeftYAxis()) {
            if (!t.isYdeg())
                return false;
        }
    }
    // Else
    return true;
}
void PlotWidget::generateLeftYAxis(double &min, double &max) const {
    qDebug() << "PlotWidget::generateLeftYAxis";
    min = DBL_INF;
    max = DBL_NEG_INF;
    foreach (TraceSettings t, _model->traces()) {
        ProcessTrace p(t, *_data);
        qDebug() << "  processing " << p.name();
        if (t.isLeftYAxis()) {
            qDebug() << "    is Left";
            qDebug() << "    y.size: " << p.y().size();
            const double newYMin = RsaToolbox::min(p.y());
            if (newYMin < min)
                min = newYMin;
            const double newYMax = RsaToolbox::max(p.y());
            if (newYMax > max)
                max = newYMax;
            qDebug() << "    min: " << newYMin;
            qDebug() << "    max: " << newYMax;
        }
    }
    qDebug() << "  Left Y Min: " << min;
    qDebug() << "  Left Y Max: " << max;
}
QString PlotWidget::generateLeftYLabel() const {
    bool isDb = false;
    bool isMag = false;
    bool isDeg = false;
    bool isRad = false;
    bool isVswr = false;
    bool isDbm = false;
    foreach (TraceSettings t, _model->traces()) {
        if (t.isLeftYAxis()) {
            if (t.isYdB())
                isDb = true;
            if (t.isYmag())
                isMag = true;
            if (t.isYdeg())
                isDeg = true;
            if (t.isYrad())
                isRad = true;
            if (t.isYvswr())
                isVswr = true;
            if (t.isYdBm())
                isDbm = true;
        }
    }

    QStringList units;
    if (isDb)
        units << "dB";
    if (isMag)
        units << "mag";
    if (isDeg)
        units << "deg";
    if (isRad)
        units << "rad";
    if (isVswr)
        units << "VSWR";
    if (isDbm)
        units << "dBm";
    return toString(units, ", ");
}

bool PlotWidget::isRightYAxisDegrees() const {
    foreach (TraceSettings t, _model->traces()) {
        if (t.isRightYAxis()) {
            if (!t.isYdeg())
                return false;
        }
    }
    // Else
    return true;
}
void PlotWidget::generateRightYAxis(double &min, double &max) const {
    min = DBL_INF;
    max = DBL_NEG_INF;
    foreach (TraceSettings t, _model->traces()) {
        ProcessTrace p(t, *_data);
        if (t.isRightYAxis()) {
            const double newYMin = RsaToolbox::min(p.y());
            if (newYMin < min)
                min = newYMin;
            const double newYMax = RsaToolbox::max(p.y());
            if (newYMax > max)
                max = newYMax;
        }
    }
}
QString PlotWidget::generateRightYLabel() const {
    bool isDb = false;
    bool isMag = false;
    bool isDeg = false;
    bool isRad = false;
    bool isVswr = false;
    bool isDbm = false;
    foreach (TraceSettings t, _model->traces()) {
        if (t.isRightYAxis()) {
            if (t.isYdB())
                isDb = true;
            if (t.isYmag())
                isMag = true;
            if (t.isYdeg())
                isDeg = true;
            if (t.isYrad())
                isRad = true;
            if (t.isYvswr())
                isVswr = true;
            if (t.isYdBm())
                isDbm = true;
        }
    }

    QStringList units;
    if (isDb)
        units << "dB";
    if (isMag)
        units << "mag";
    if (isDeg)
        units << "deg";
    if (isRad)
        units << "rad";
    if (isVswr)
        units << "VSWR";
    if (isDbm)
        units << "dBm";
    return toString(units, ", ");
}

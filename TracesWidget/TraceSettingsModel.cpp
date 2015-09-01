#include "TraceSettingsModel.h"

// RsaToolbox
 #include <General.h>
 using namespace RsaToolbox;

// Qt
#include <QDebug>
#include <QPixmap>


TraceSettingsModel::TraceSettingsModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}

TraceSettingsModel::~TraceSettingsModel()
{

}

QVariant TraceSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (section < 0 || section >= COLUMNS)
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Vertical)
        return QAbstractTableModel::headerData(section, orientation, role);

    switch (section) {
    case Column::yAxis:
        return "Y Axis";
    case Column::color:
        return "Color";
    case Column::yParameter:
        return "Y Parameter";
    case Column::yFormat:
        return "Y Format";
    case Column::xParameter:
        return "X Parameter";
    case Column::atParameter:
        return "At";
    case Column::atValue:
        return QVariant();
    default:
        return QVariant();
    }

}
QModelIndex TraceSettingsModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid())
        return QModelIndex();
    if (row < 0 || row >= _traces.size())
        return QModelIndex();
    if (column < 0 || column >= COLUMNS)
        return QModelIndex();

    return createIndex(row, column);
}
QModelIndex TraceSettingsModel::parent(const QModelIndex &child) const {
    Q_UNUSED(child);
    return QModelIndex();
}
Qt::ItemFlags TraceSettingsModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemFlags();

    const int row = index.row();
    const int column = index.column();

    Qt::ItemFlags flags = QAbstractTableModel::flags(index);;
    switch (column) {
    case Column::yAxis:
    case Column::color:
    case Column::yParameter:
        return flags | Qt::ItemIsEditable;
    case Column::yFormat:
        if (_traces[row].possibleYFormats().size() > 1)
            return flags | Qt::ItemIsEditable;
        else
            return flags;
    case Column::xParameter:
        if (_traces[row].possibleXParameters().size() > 1)
            return flags | Qt::ItemIsEditable;
        else
            return flags;
    case Column::atParameter:
        if (_traces[row].possibleAtParameters().size() > 1)
            return flags | Qt::ItemIsEditable;
        else
            return flags;
    case Column::atValue:
        if (_traces[row].isAtValue())
            return flags | Qt::ItemIsEditable;
        else
            return flags;
    default:
        return flags;
    }
}
int TraceSettingsModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    else
        return _traces.size();
}
int TraceSettingsModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    else
        return COLUMNS;
}
QVariant TraceSettingsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();
    if (index.model() != this)
        return QVariant();

    const int row = index.row();
    const int column = index.column();
    if (row < 0 || row >= _traces.size())
        return QVariant();
    if (column < 0 || column >= COLUMNS)
        return QVariant();

    if (role != Qt::DisplayRole
            && role != Qt::EditRole
            && role != Qt::DecorationRole)
    {
        return QVariant();
    }
    else if (role == Qt::DecorationRole && column != Column::color) {
        return QVariant();
    }


    switch (column) {
    case Column::yAxis:
        return _traces[row].yAxis;
    case Column::color:
        if (role == Qt::DecorationRole) {
            QPixmap pixmap(10, 5);
            pixmap.fill(QColor(_traces[row].color));
            return pixmap;
        }
        else {
            return _traces[row].colorString();
        }
    case Column::yParameter:
        return _traces[row].yParameter;
    case Column::yFormat:
        return _traces[row].yFormat;
    case Column::xParameter:
        return _traces[row].xParameter;
    case Column::atParameter:
        return _traces[row].atParameter;
    case Column::atValue:
        if (!_traces[row].isAtValue()) {
            return QVariant();
        }
        if (role == Qt::EditRole){
            return _traces[row].atValue;
        }
        else {
            if (_traces[row].isAtFrequency())
                return formatValue(_traces[row].atValue, 3, Units::Hertz);
            else if (_traces[row].isAtPin())
                return formatValue(_traces[row].atValue, 2, Units::dBm);
            else
                return QVariant();
        }
    default:
        return QVariant();
    }
}
bool TraceSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid())
        return false;
    if (index.model() != this)
        return false;

    if (role != Qt::EditRole)
        return false;
    if (!value.isValid())
        return false;

    const int row = index.row();
    const int column = index.column();
    if (row < 0 || row >= _traces.size())
        return false;
    if (column < 0 || column >= COLUMNS)
        return false;

    if (column == atValue && !value.canConvert<double>())
        return false;
    else if (!value.canConvert<QString>())
        return false;

    Qt::GlobalColor originalColor;
    QModelIndex topLeft = createIndex(row, 0);
    QModelIndex bottomRight = createIndex(row, COLUMNS-1);
    switch (column) {
    case Column::yAxis:
        if (_traces[row].yAxis.compare(value.toString()) == 0)
            return true;
        _traces[row].yAxis = value.toString();
        emit dataChanged(topLeft, bottomRight);
        return true;
    case Column::color:
        originalColor = _traces[row].color;
        if (!_traces[row].setColorFromString(value.toString()))
            return false;
        if (originalColor != _traces[row].color)
            emit dataChanged(topLeft, bottomRight);
        return true;
    case Column::yParameter:
        if (_traces[row].yParameter.compare(value.toString()) == 0)
            return true;
        _traces[row].yParameter = value.toString();
        fixTraceSettings(row);
        emit dataChanged(topLeft, bottomRight);
        return true;
    case Column::yFormat:
        if (_traces[row].yFormat.compare(value.toString()) == 0)
            return true;
        _traces[row].yFormat = value.toString();
        fixTraceSettings(row);
        emit dataChanged(topLeft, bottomRight);
        return true;
    case Column::xParameter:
        if (_traces[row].xParameter.compare(value.toString()) == 0)
            return true;
        _traces[row].xParameter = value.toString();
        fixTraceSettings(row);
        emit dataChanged(topLeft, bottomRight);
        return true;
    case Column::atParameter:
        if (_traces[row].atParameter.compare(value.toString()) == 0)
            return true;
        _traces[row].atParameter = value.toString();
        fixTraceSettings(row);
        emit dataChanged(topLeft, bottomRight);
        return true;
    case Column::atValue:
        if (_traces[row].atValue == value.toDouble())
            return true;
        _traces[row].atValue = value.toDouble();
        fixTraceSettings(row);
        emit dataChanged(index, index);
        return true;
    default:
        return false;
    }
}

bool TraceSettingsModel::insertRows(int row, int count, const QModelIndex &parent) {
    if (parent.isValid())
        return false;
    if (row < 0 || row > _traces.size())
        return false;
    if (count < 0)
        return false;
    if (count == 0)
        return true;

    beginInsertRows(parent, row, row + count - 1);
    _traces.insert(row, count, TraceSettings());
    endInsertRows();
    return true;
}
bool TraceSettingsModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (parent.isValid())
        return false;
    if (row < 0 || row >= _traces.size())
        return false;
    if (count < 0)
        return false;
    if (count == 0)
        return true;
    const int lastRow = row + count - 1;
    if (lastRow >= _traces.size())
        return false;

    beginRemoveRows(parent, row, lastRow);
    _traces.remove(row, count);
    endRemoveRows();
    return true;
}

QVector<TraceSettings> TraceSettingsModel::traces() const {
    return _traces;
}

void TraceSettingsModel::reset() {
    beginResetModel();
    _traces.clear();
    endResetModel();
}

void TraceSettingsModel::fixTraceSettings(int row) {
    TraceSettings &t(_traces[row]);
    if (t.isValid())
        return;

    if (!t.isValidYAxis()) {
        t.yAxis = "Left";
    }
    if (!t.isValidYParameter()) {
        t.yParameter = "Gain";
    }
    if (!t.isValidYFormat()) {
        t.yFormat = t.possibleYFormats().first();
    }
    if (!t.isValidXParameter()) {
        t.xParameter = t.possibleXParameters().first();
    }
    if (!t.isValidAtParameter()) {
        t.atParameter = t.possibleAtParameters().first();
    }
    if (!t.isValidAtValue()) {
        t.atValue = 0;
    }
}

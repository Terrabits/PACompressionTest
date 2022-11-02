#include <QDebug>

// RsaTolbox
#include "General.h"
#include "GenericInstrument.h"
#include "VisaBus.h"
#include "TcpBus.h"
#include "NoBus.h"
using namespace RsaToolbox;


GenericInstrument::GenericInstrument(QObject *parent) :
    QObject(parent)
{
    resetBus();
}
/*!
 * \brief Constructor that uses the provided bus connection
 * \param bus Connection to an instrument
 * \param parent Optional parent QObject
 */
GenericInstrument::GenericInstrument(GenericBus *bus, QObject *parent) :
    QObject(parent)
{
    resetBus(bus);
    _bus->setBufferSize(500);
    _bus->setTimeout(1000);
}
/*!
 * \brief Construct which connects to the instrument at the address
 * specified
 *
 * When using this constructor, %GenericInstrument defaults to
 * VisaBus.
 * Please make sure VISA is installed on the target system.
 *
 * \param type The type of connection (see ConnectionType)
 * \param address Address of the instrument (e.g. "127.0.0.1")
 * \param parent Optional parent QObject
 */
GenericInstrument::GenericInstrument(ConnectionType type, QString address, QObject *parent) :
    QObject(parent)
{
    resetBus(type, address);
}


void GenericInstrument::resetBus() {
    bool emitSignal = isConnected();
    _bus.reset(new NoBus());
    QObject::connect(_bus.data(), SIGNAL(error()), this, SIGNAL(busError()));
    if (emitSignal)
        emit disconnected();
}

void GenericInstrument::resetBus(GenericBus *bus) {
    resetBus();
    _bus.reset(bus);
    QObject::connect(_bus.data(), SIGNAL(error()), this, SIGNAL(busError()));
    if (isConnected()) {
        emit connected();
    }
}


void GenericInstrument::resetBus(ConnectionType type, QString address) {
#ifdef Q_OS_WIN32
    resetBus(new VisaBus(type, address));
#else
    if (VisaBus::isVisaInstalled())
        resetBus(new VisaBus(type, address));
    else if (type == ConnectionType::TcpSocketConnection)
        resetBus(new TcpBus(type, address));
    else
        resetBus(new NoBus());
#endif
}

/*!
 * \brief Checks for an instrument connection
 * \return \c true if an instrument is connected,
 * \c false otherwise
 */
bool GenericInstrument::isConnected() const {
    return !_bus.isNull() && _bus->isOpen();
}
/*!
 * \brief Checks for no connection to an instrument
 * \return \c true if no instrument connection is
 * present, \c false otherwise
 */
bool GenericInstrument::isDisconnected() const {
    return(!isConnected());
}

/*!
 * \brief Passes ownership of the current instrument
 * connection to callee.
 *
 * %GenericInstrument object is left in a disconnected
 * state.
 *
 * \note If no instrument connection is present,
 * \c bus may be \c NULL. Check the value
 * of \c bus for NULL and check to make sure
 * \c bus is connected before use.
 *
 * \return Ownership of \c bus pointer
 * \sa bus()
 */
GenericBus* GenericInstrument::takeBus() {
    return(_bus.take());
    disconnected();
}

/*!
 * \brief Returns the current connection type
 *
 * \note If no connection is present, \c NO_CONNECTION
 * is returned.
 *
 * \return Instrument connection type
 * \sa address(), timeout_ms(), RsaToolbox::ConnectionType
 */
ConnectionType GenericInstrument::connectionType() const {
    if (isConnected())
        return(_bus->connectionType());
    //else
    return(ConnectionType::NoConnection);
}

/*!
 * \brief Returns the address of the instrument
 * that is connected
 *
 * \note If no instrument connection is present,
 * an emtpy string ("") is returned.
 *
 * \return Instrument address
 * \sa connectionType(), timeout_ms()
 */
QString GenericInstrument::address() const {
    if (isConnected())
        return(_bus->address());
    // else
    return("");
}

/*!
 * \brief GenericInstrument::read
 * \param buffer
 * \param bufferSize_B
 * \param timeout_ms
 * \return
 */
bool GenericInstrument::read(char *buffer, uint bufferSize_B, uint timeout_ms) {
    uint previousTimeout = _bus->timeout_ms();
    _bus->setTimeout(timeout_ms);
    bool success = _bus->read(buffer, bufferSize_B);
    _bus->setTimeout(previousTimeout);
    return(success);
}



QString GenericInstrument::read(uint bufferSize_B, uint timeout_ms) {
    uint previousBufferSize = _bus->bufferSize_B();
    uint previousTimeout = _bus->timeout_ms();

    _bus->setBufferSize(bufferSize_B);
    _bus->setTimeout(timeout_ms);
    QString result = _bus->read();

    _bus->setBufferSize(previousBufferSize);
    _bus->setTimeout(previousTimeout);
    return(result);
}

void GenericInstrument::write(QString scpiCommand) {
    _bus->write(scpiCommand);
}

QString GenericInstrument::query(QString scpiCommand, uint bufferSize_B, uint timeout_ms) {
    uint previousBufferSize = _bus->bufferSize_B();
    uint previousTimeout = _bus->timeout_ms();

    _bus->setBufferSize(bufferSize_B);
    _bus->setTimeout(timeout_ms);
    QString result = _bus->query(scpiCommand);

    _bus->setBufferSize(previousBufferSize);
    _bus->setTimeout(previousTimeout);
    return(result);
}

/*!
 * \brief GenericInstrument::binaryRead
 * \param buffer
 * \param bufferSize_B
 * \param bytesRead
 * \param timeout_ms
 * \return
 */
bool GenericInstrument::binaryRead(char *buffer, uint bufferSize_B, uint &bytesRead, uint timeout_ms) {
    uint previousTimeout = _bus->timeout_ms();
    _bus->setTimeout(timeout_ms);
    bool success = _bus->binaryRead(buffer, bufferSize_B, bytesRead);
    _bus->setTimeout(previousTimeout);
    return(success);
}

QByteArray GenericInstrument::binaryRead(uint bufferSize_B, uint timeout_ms)
{
    uint previousBufferSize = _bus->bufferSize_B();
    uint previousTimeout = _bus->timeout_ms();

    _bus->setBufferSize(bufferSize_B);
    _bus->setTimeout(timeout_ms);
    QByteArray result = _bus->binaryRead();

    _bus->setBufferSize(previousBufferSize);
    _bus->setTimeout(previousTimeout);
    return(result);
}
void GenericInstrument::binaryWrite(QByteArray scpiCommand) {
    _bus->binaryWrite(scpiCommand);
}
/*!
 * \brief Queries binary response from the
 * instrument
 *
 * The data contained in scpiCommand is written
 * to the instrument as-is; no null-terminations
 * or assumentions are made. The returned binary
 * data is treated in a similar manner.
 *
 * This is appropriate
 * when standard string formatting would interfere
 * with the query operation, such as when
 * transferring the raw binary contents of a file.
 *
 * \note bufferSize_B() must be equal to or
 * greater than the amount of data to be read,
 * otherwise a partial read will occur.
 *
 * \param scpiCommand
 * \param bufferSize_B
 * \param timeout_ms
 * \return Query result
 */
QByteArray GenericInstrument::binaryQuery(QByteArray scpiCommand,
                                          uint bufferSize_B, uint timeout_ms)
{
    uint previousBufferSize = _bus->bufferSize_B();
    uint previousTimeout = _bus->timeout_ms();

    _bus->setBufferSize(bufferSize_B);
    _bus->setTimeout(timeout_ms);
    QByteArray result = _bus->binaryQuery(scpiCommand);

    _bus->setBufferSize(previousBufferSize);
    _bus->setTimeout(previousTimeout);
    return(result);
}

/*!
 * \brief Locks the instrument for exclusive access
 *
 * This method calls the \c GenericBus::lock() method
 * of the underlying connection bus. Note that
 * Not all instrument busses support locking the instrument.
 *
 * \return \c true if operation is successful, \c false otherwise
 * \sa GenericBus::lock(), unlock()
 */
bool GenericInstrument::lock() {
    if (isConnected())
        return(_bus->lock());
    // else
    return(false);
}

/*!
 * \brief Unlocks the instrument for access by other applications
 *
 * This method calls the \c GenericBus::unlock() method
 * of the underlying connection bus. Note that
 * Not all instrument busses support locking the instrument.
 *
 * \return \c true if operation is successful, \c false otherwise
 * \sa GenericBus::unlock(), lock()
 */
bool GenericInstrument::unlock() {
    if (isConnected())
        return(_bus->unlock());
    // else
    return(false);
}

/*!
 * \brief Puts the instrument into local mode
 * via the current bus protocol
 *
 * This method calls the \c GenericBus::local()
 * method of the current bus object. Note that
 * not all busses support putting the instrument
 * into local mode. If a bus-level method is not provided,
 * check the instrument documentation for an equivalent
 * device-specific SCPI commands.
 *
 * \return \c true if successful, \c false otherwise
 * \sa GenericBus::remote(), local()
 */
bool GenericInstrument::local() {
    if (isConnected())
        return(_bus->local());
    // else
    return(false);
}

/*!
 * \brief Puts the instrument into remote mode
 * via the current bus protocol
 *
 * This method calls the \c GenericBus::remote()
 * method of the current bus object. Note that
 * not all busses support putting the instrument
 * into remote mode. If a bus-level method is not provided,
 * check the instrument documentation for an equivalent
 * device-specific SCPI commands.
 *
 * \return \c true if successful, \c false otherwise
 * \sa GenericBus::remote(), local()
 */
bool GenericInstrument::remote() {
    if (isConnected())
        return(_bus->remote());
    // else
    return(false);
}

/*!
 * \brief Determines if the instrument is a Rohde \&
 * Schwarz instrument
 *
 * This method uses the \c idString(). If the
 * \c idString() contains the string "Rohde",
 * this method returns true.
 *
 * \return \c true if instrument is Rohde \& Schwarz,
 * \c false otherwise
 */
bool GenericInstrument::isRohdeSchwarz() {
    return(idString().contains("Rohde", Qt::CaseInsensitive));
}

// General SCPI commands (*)

/*!
 * \brief Returns the identification string
 *
 * This method uses the "*IDN?" SCPI
 * command to query the id string
 * of the instrument.
 *
 * \return Identification string
 */
QString GenericInstrument::idString() {
    return(_bus->query("*IDN?\n").trimmed());
}

/*!
 * \brief Returns the options string
 *
 * This method uses the "*OPT?"
 * SCPI command.
 *
 * \return Options string
 * \sa idString()
 */
QString GenericInstrument::optionsString() {
    return(_bus->query("*OPT?\n").trimmed());
}

/*!
 * \brief Presets the instrument
 *
 * This method sends the "*RST"
 * SCPI command.
 */
void GenericInstrument::preset() {
    _bus->write("*RST\n");
}

/*!
 * \brief Sends the "*WAI" SCPI command
 *
 * *WAI is used to synchonize SCPI commands.
 * *WAI commands the instrument to process all
 * preceeding commands and allow all sweeps to
 * finish before processing any subsequent SCPI
 * commands. This method immediately returns,
 * although any subsequent read/write operations
 * may be held up while *WAI is being processed.
 *
 * For a gentle introduction to synchronous
 * instrument programming, see Application Note
 * <a href="http://www.rohde-schwarz.de/file/1GP79_1E_SCPI_Programming_Guide_SigGens.pdf">
 * 1GP79-1E: Top Ten SCPI Programming Tips for
 * Signal Generators</a>. Although the App Note is
 * specifically for Signal Generators, many of
 * the concepts apply equally well to all
 * instruments.
 *
 *\note Make sure that a valid instrument connection is
 * present before calling this method, otherwise
 * unintended behavior may result.
 */
void GenericInstrument::wait() {
    _bus->write("*WAI\n");
}

/*!
 * \brief Pauses until previous commands are completed.
 *
 * Use this method to synchronize your application
 * with the SCPI command queue of the instrument.
 *
 * This method sends the "*OPC?" SCPI command
 * to the instrument. Upon receiving this query,
 * the instrument will complete all previous
 * SCPI commands before returning the string "1".
 * \c Pause waits for this return value before
 * returning to the caller.
 *
 * This command is used for synchronous
 * measurement sweeps.
 *
 * For a gentle introduction to synchronous
 * instrument programming, see Application Note
 * <a href="http://www.rohde-schwarz.de/file/1GP79_1E_SCPI_Programming_Guide_SigGens.pdf">
 * 1GP79-1E: Top Ten SCPI Programming Tips for
 * Signal Generators</a>. Although the App Note is
 * specifically for Signal Generators, many of
 * the concepts apply equally well to all
 * instruments.
 *
 * \note This command has a timeout of
 * \c GenericBus::timeout_ms() milliseconds.
 *
 * \return \c true if operation(s) completed.
 * \c false otherwise.
 * \sa pause(uint timeout_ms), GenericBus::timeout_ms()
 */
bool GenericInstrument::pause() {
    return _bus->query("*OPC?\n").toUInt() == 1;
}

/*!
 * \brief Pauses until previous commands are completed
 * or for \c timeout_ms, whichever comes first.
 * \param timeout_ms
 * \return \c true if operation(s) completed.
 * \c false otherwise
 * \sa pause()
 */
bool GenericInstrument::pause(uint timeout_ms) {
    uint oldTime = _bus->timeout_ms();
    _bus->setTimeout(timeout_ms);
    bool isOperationComplete = pause();
    _bus->setTimeout(oldTime);
    return isOperationComplete;
}

/*!
 * \brief Initialize poll-and-loop synchronization
 *
 * This command sets the Operation Complete
 * bit (bit 0) of the Event Status Register (ESR)
 * to zero. Call this command immediately after
 * the operation you want to synchronize to.
 *
 * When the instrument completes the current operation,
 * it will set the Operation Complete bit to
 * 1. Use the \c isOperationComplete() method
 * to check for this.
 *
 * For a gentle introduction to synchronous
 * instrument programming, see Application Note
 * <a href="http://www.rohde-schwarz.de/file/1GP79_1E_SCPI_Programming_Guide_SigGens.pdf">
 * 1GP79-1E: Top Ten SCPI Programming Tips for
 * Signal Generators</a>. Although the App Note is
 * specifically for Signal Generators, many of
 * the concepts apply equally well to all
 * instruments.
 * \sa isOperationComplete()
 */
void GenericInstrument::initializePolling() {
    _bus->write("*OPC\n");
}

/*!
 * \brief Polls instrument for operation status
 *
 * When used with \c initializePolling(),
 * this method returns \c true when the current
 * operation is complete.
 *
 * For a gentle introduction to synchronous
 * instrument programming, see Application Note
 * <a href="http://www.rohde-schwarz.de/file/1GP79_1E_SCPI_Programming_Guide_SigGens.pdf">
 * 1GP79-1E: Top Ten SCPI Programming Tips for
 * Signal Generators</a>. Although the App Note is
 * specifically for Signal Generators, many of
 * the concepts apply equally well to all
 * instruments.
 * \return \c true if operation is complete,
 * \c false otherwise
 * \sa initializePolling()
 */
bool GenericInstrument::isOperationComplete() {
    const uint opcBit = 0x1;
    uint esr = _bus->query("*ESR?\n").toUInt();
    return (esr & opcBit) == 1;
}

/*!
 * \brief Clears all errors
 *
 * Sends the "*CLS" SCPI command to
 * the instrument, which clears
 * all errors from the
 * error queue.  These errors are
 * discarded without being read
 * or handled.
 */
void GenericInstrument::clearStatus() {
    _bus->write("*CLS\n");
}

cmake_minimum_required(VERSION 3.5)
project(RsaToolbox LANGUAGES CXX)

# settings
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# debug settings
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_MODE")
add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# no warnings
add_compile_options(-w)

# conan config
include(${CMAKE_BINARY_DIR}/../../conanbuildinfo.cmake)
conan_basic_setup(Logging)

# packages
find_package(Qt5 REQUIRED COMPONENTS Widgets Network PrintSupport Test)

# set project files
set(PROJECT_SOURCES
      Bus/GenericBus.cpp
      Bus/GenericBus.h
      Bus/No/NoBus.cpp
      Bus/No/NoBus.h
      Bus/Tcp/TcpBus.cpp
      Bus/Tcp/TcpBus.h
      Bus/Visa/VisaBus.cpp
      Bus/Visa/VisaBus.h
      GUI/About/About.cpp
      GUI/About/About.h
      GUI/About/About.ui
      GUI/About/AboutResources.qrc
      GUI/About/main.cpp
      GUI/ConnectButton/ConnectButton.cpp
      GUI/ConnectButton/ConnectButton.h
      GUI/ConnectionTypeComboBox/ConnectionTypeComboBox.cpp
      GUI/ConnectionTypeComboBox/ConnectionTypeComboBox.h
      GUI/DecibelEdit/DecibelEdit.cpp
      GUI/DecibelEdit/DecibelEdit.h
      GUI/ErrorLabel/ErrorLabel.cpp
      GUI/ErrorLabel/ErrorLabel.h
      GUI/Figure/Figure.cpp
      GUI/Figure/Figure.h
      GUI/Figure/Figure.ui
      GUI/FrequencyEdit/FrequencyEdit.cpp
      GUI/FrequencyEdit/FrequencyEdit.h
      GUI/GetSnpFile/GetSnpFile.cpp
      GUI/GetSnpFile/GetSnpFile.h
      GUI/GetSnpFile/GetSnpFile.ui
      GUI/LastPath/LastPath.cpp
      GUI/LastPath/LastPath.h
      GUI/ListPanel/ListPanel.cpp
      GUI/ListPanel/ListPanel.h
      GUI/ListPanel/ListPanel.ui
      GUI/PointsEdit/PointsEdit.cpp
      GUI/PointsEdit/PointsEdit.h
      GUI/PortsLineEdit/PortsLineEdit.cpp
      GUI/PortsLineEdit/PortsLineEdit.h
      GUI/PowerEdit/PowerEdit.cpp
      GUI/PowerEdit/PowerEdit.h
      GUI/TimedProgressBar/TimedProgressBar.cpp
      GUI/TimedProgressBar/TimedProgressBar.h
      GUI/TimedProgressBar/TimedProgressBar.ui
      GUI/Update/Update.cpp
      GUI/Update/Update.h
      GUI/Update/Update.ui
      GUI/Wizard/BreadCrumbs.cpp
      GUI/Wizard/BreadCrumbs.h
      GUI/Wizard/BreadCrumbs.ui
      GUI/Wizard/Wizard.cpp
      GUI/Wizard/Wizard.h
      GUI/Wizard/Wizard.ui
      GUI/Wizard/WizardButtons.cpp
      GUI/Wizard/WizardButtons.h
      GUI/Wizard/WizardButtons.ui
      GUI/Wizard/WizardPage.cpp
      GUI/Wizard/WizardPage.h
      GUI/Wizard/WizardResources.qrc
      GUI/getSaveFileNameWidget/getSaveFileNameWidget.cpp
      GUI/getSaveFileNameWidget/getSaveFileNameWidget.h
      GUI/getSaveFileNameWidget/getSaveFileNameWidget.ui
      General/BalancedPort.cpp
      General/BalancedPort.h
      General/Connector.cpp
      General/Connector.h
      General/Definitions.h
      General/FormattedComplex.cpp
      General/FormattedComplex.h
      General/FormattedNetworkData.cpp
      General/FormattedNetworkData.h
      General/FormattedTouchstone.cpp
      General/FormattedTouchstone.h
      General/General.cpp
      General/General.h
      General/IndexName.cpp
      General/IndexName.h
      General/Keys.cpp
      General/Keys.h
      General/NameLabel.cpp
      General/NameLabel.h
      General/NetworkData.cpp
      General/NetworkData.h
      General/NetworkTraceData.cpp
      General/NetworkTraceData.h
      General/SnpStreamReader.cpp
      General/SnpStreamReader.h
      General/SnpStreamWriter.cpp
      General/SnpStreamWriter.h
      General/Touchstone.cpp
      General/Touchstone.h
      General/Version.cpp
      General/Version.h
      Instruments/GenericInstrument.cpp
      Instruments/GenericInstrument.h
      Instruments/Vna/Vna.cpp
      Instruments/Vna/Vna.h
      Instruments/Vna/VnaAveraging.cpp
      Instruments/Vna/VnaAveraging.h
      Instruments/Vna/VnaCalKit.cpp
      Instruments/Vna/VnaCalKit.h
      Instruments/Vna/VnaCalStandard.cpp
      Instruments/Vna/VnaCalStandard.h
      Instruments/Vna/VnaCalUnit.cpp
      Instruments/Vna/VnaCalUnit.h
      Instruments/Vna/VnaCalibrate.cpp
      Instruments/Vna/VnaCalibrate.h
      Instruments/Vna/VnaChannel.cpp
      Instruments/Vna/VnaChannel.h
      Instruments/Vna/VnaCorrections.cpp
      Instruments/Vna/VnaCorrections.h
      Instruments/Vna/VnaCwSweep.cpp
      Instruments/Vna/VnaCwSweep.h
      Instruments/Vna/VnaDiagram.cpp
      Instruments/Vna/VnaDiagram.h
      Instruments/Vna/VnaExtensionUnit.cpp
      Instruments/Vna/VnaExtensionUnit.h
      Instruments/Vna/VnaFileSystem.cpp
      Instruments/Vna/VnaFileSystem.h
      Instruments/Vna/VnaLimits.cpp
      Instruments/Vna/VnaLimits.h
      Instruments/Vna/VnaLinearSweep.cpp
      Instruments/Vna/VnaLinearSweep.h
      Instruments/Vna/VnaLogSweep.cpp
      Instruments/Vna/VnaLogSweep.h
      Instruments/Vna/VnaMarker.cpp
      Instruments/Vna/VnaMarker.h
      Instruments/Vna/VnaMath.cpp
      Instruments/Vna/VnaMath.h
      Instruments/Vna/VnaPowerCalibrate.cpp
      Instruments/Vna/VnaPowerCalibrate.h
      Instruments/Vna/VnaPowerCorrections.cpp
      Instruments/Vna/VnaPowerCorrections.h
      Instruments/Vna/VnaPowerSweep.cpp
      Instruments/Vna/VnaPowerSweep.h
      Instruments/Vna/VnaProperties.cpp
      Instruments/Vna/VnaProperties.h
      Instruments/Vna/VnaPulseGenerator.cpp
      Instruments/Vna/VnaPulseGenerator.h
      Instruments/Vna/VnaReferenceMarker.cpp
      Instruments/Vna/VnaReferenceMarker.h
      Instruments/Vna/VnaScpi.cpp
      Instruments/Vna/VnaScpi.h
      Instruments/Vna/VnaSegmentedSweep.cpp
      Instruments/Vna/VnaSegmentedSweep.h
      Instruments/Vna/VnaSet.cpp
      Instruments/Vna/VnaSet.h
      Instruments/Vna/VnaSettings.cpp
      Instruments/Vna/VnaSettings.h
      Instruments/Vna/VnaStandardModel.cpp
      Instruments/Vna/VnaStandardModel.h
      Instruments/Vna/VnaSweepSegment.cpp
      Instruments/Vna/VnaSweepSegment.h
      Instruments/Vna/VnaSwitchMatrix.cpp
      Instruments/Vna/VnaSwitchMatrix.h
      Instruments/Vna/VnaTimeDomain.cpp
      Instruments/Vna/VnaTimeDomain.h
      Instruments/Vna/VnaTimeSweep.cpp
      Instruments/Vna/VnaTimeSweep.h
      Instruments/Vna/VnaTrace.cpp
      Instruments/Vna/VnaTrace.h
      Instruments/Vna/VnaTrigger.cpp
      Instruments/Vna/VnaTrigger.h
      Instruments/Vna/VnaUndo.cpp
      Instruments/Vna/VnaUndo.h
      Instruments/Vna/VnaUserDefinedPort.cpp
      Instruments/Vna/VnaUserDefinedPort.h
      Test/SpeedTracker/SpeedTracker.cpp
      Test/SpeedTracker/SpeedTracker.h
      Test/SpeedTracker/SpeedTrackerChild.cpp
      Test/SpeedTracker/SpeedTrackerChild.h
      Test/Test.cpp
      Test/Test.h
      Test/TestClass.cpp
      Test/TestClass.h
      Test/TestRunner/TestRunner.cpp
      Test/TestRunner/TestRunner.h
      Test/VnaTestClass.cpp
      Test/VnaTestClass.h
)

# build static lib
add_library(RsaToolbox STATIC ${PROJECT_SOURCES})

# linking
target_link_libraries(RsaToolbox PUBLIC Logging)
target_link_libraries(RsaToolbox PUBLIC qcustomplot)
target_link_libraries(RsaToolbox PUBLIC Qt5::Widgets Qt5::Network Qt5::PrintSupport Qt5::Test)

# includes
target_include_directories(RsaToolbox PUBLIC Bus)
target_include_directories(RsaToolbox PUBLIC Bus/No)
target_include_directories(RsaToolbox PUBLIC Bus/Tcp)
target_include_directories(RsaToolbox PUBLIC Bus/Visa)
target_include_directories(RsaToolbox PUBLIC Bus/Visa/RsVisa)
target_include_directories(RsaToolbox PUBLIC GUI/About)
target_include_directories(RsaToolbox PUBLIC GUI/ConnectButton)
target_include_directories(RsaToolbox PUBLIC GUI/ConnectionTypeComboBox)
target_include_directories(RsaToolbox PUBLIC GUI/DecibelEdit)
target_include_directories(RsaToolbox PUBLIC GUI/ErrorLabel)
target_include_directories(RsaToolbox PUBLIC GUI/Figure)
target_include_directories(RsaToolbox PUBLIC GUI/FrequencyEdit)
target_include_directories(RsaToolbox PUBLIC GUI/GetSnpFile)
target_include_directories(RsaToolbox PUBLIC GUI/LastPath)
target_include_directories(RsaToolbox PUBLIC GUI/ListPanel)
target_include_directories(RsaToolbox PUBLIC GUI/PointsEdit)
target_include_directories(RsaToolbox PUBLIC GUI/PortsLineEdit)
target_include_directories(RsaToolbox PUBLIC GUI/PowerEdit)
target_include_directories(RsaToolbox PUBLIC GUI/TimedProgressBar)
target_include_directories(RsaToolbox PUBLIC GUI/Update)
target_include_directories(RsaToolbox PUBLIC GUI/Wizard)
target_include_directories(RsaToolbox PUBLIC GUI/getSaveFileNameWidget)
target_include_directories(RsaToolbox PUBLIC General)
target_include_directories(RsaToolbox PUBLIC Instruments)
target_include_directories(RsaToolbox PUBLIC Instruments/Vna)
target_include_directories(RsaToolbox PUBLIC Log)
target_include_directories(RsaToolbox PUBLIC Test)
target_include_directories(RsaToolbox PUBLIC Test/SpeedTracker)
target_include_directories(RsaToolbox PUBLIC Test/TestRunner)

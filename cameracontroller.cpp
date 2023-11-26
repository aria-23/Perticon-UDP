#include "cameracontroller.h"
#include "ethernet.h"
#include "qdebug.h"
#include <math.h>
#include <QCoreApplication>
#include <QtNetwork>
#include <iostream>
#include <iomanip>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>
#include <QFuture>
#include <QSerialPortInfo>

CameraController::CameraController(QObject *parent) : QObject{parent} {


    QDir _homeFolder(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    _rootPath = _homeFolder.absolutePath() + "/Perticon/";
    QString _settingFolder = "/home/mostafa/Application/";
    _settingPath = _settingFolder + "/setting.txt";

    QVector<double> _red;
    QVector<double> _green;
    QVector<double> _blue;
    QVector<double> _gamma;
    QVector<double> _gain;
    QVector<double> _exp;
    int _ejectorPositopn;
    int _energize;
    int _deEnergize;
    QVector<int> _quantityBasedClassCountList;
    QVector<float> _quantityBasedClassConfList;
    QVector<int> _classConfList;
    QString _esp6s0GW;
    QString _esp8s0GW;
    QVector<QString> _esp6s0IP;
    QVector<QString> _esp8s0IP;
    QVector<QString> _esp6s0Serial;
    QVector<QString> _esp8s0Serial;


    if(!QDir(_rootPath).exists())
        QDir().mkdir(_rootPath);

    if(!QDir(_settingFolder).exists())
        QDir().mkdir(_settingFolder);

    bool _settingFounded = false;
    QFile settingFile(_settingPath);


    if(QFile(_settingPath).exists())
    {
        qInfo() << "setting file exists, reading the data";
        if(settingFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&settingFile);
            try {
                while(!in.atEnd()) {
                    QString line = in.readLine();
                    auto data = line.split(" ");
                    if(data.length()>0){
                        auto _lineName = data[0];
                        if(_lineName == "Red"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _red.push_back(data[i].toDouble());
                            }
                        }
                        if(_lineName == "Green"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _green.push_back(data[i].toDouble());
                            }
                        }
                        if(_lineName == "Blue"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _blue.push_back(data[i].toDouble());
                            }
                        }
                        if(_lineName == "Gamma"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _gamma.push_back(data[i].toDouble());
                            }
                        }
                        if(_lineName == "Gain"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _gain.push_back(data[i].toDouble());
                            }
                        }
                        if(_lineName == "Exposure"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _exp.push_back(data[i].toDouble());
                            }
                        }
                        if(_lineName == "EjectorPosition"){
                            _ejectorPositopn = data[1].toInt();
                        }
                        if(_lineName == "Energize"){
                            _energize = data[1].toInt();

                        }
                        if(_lineName == "de-Energize"){
                            _deEnergize = data[1].toInt();

                        }
                        if(_lineName == "QuantityBasedClassCount"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _quantityBasedClassCountList.push_back(data[i].toDouble());
                            }
                        }
                        if(_lineName == "QuantityBasedClassConf"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _quantityBasedClassConfList.push_back(data[i].toDouble());
                                //
                            }
                        }
                        if(_lineName == "ClassConfList"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _classConfList.push_back(data[i].toDouble());
                            }
                            qDebug() << m_classConfList.size() << "m_classConfListm_classConfList";
                        }
                        if(_lineName == "esp6s0GW"){
                            _esp6s0GW = data[1];
                        }
                        if(_lineName == "esp8s0GW"){
                            _esp8s0GW = data[1];
                        }
                        if(_lineName == "esp6s0IP"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _esp6s0IP.push_back(data[i]);
                            }
                        }
                        if(_lineName == "esp8s0IP"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _esp8s0IP.push_back(data[i]);
                            }
                        }
                        if(_lineName == "esp6s0Serial"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _esp6s0Serial.push_back(data[i]);
                            }
                        }
                        if(_lineName == "esp8s0Serial"){
                            for (int i = 1; i < data.length(); ++i) {
                                if(data[i]!= "")
                                    _esp8s0Serial.push_back(data[i]);
                            }
                        }
                    }
                }
                settingFile.close();
                _settingFounded = true;
            }
            catch (...) {
                qDebug() << "****** Can not read setting file" ;
            }
        }
        else {
            qDebug() << "****** Can not open setting file" ;
        }
    }


    if(!_settingFounded){

        defaultValues myDefaultValues;

        QString _redStr = "Red ";
        QString _greenStr = "Green ";
        QString _blueStr = "Blue ";
        QString _gammaStr = "Gamma ";
        QString _gainStr = "Gain ";
        QString _expStr = "Exposure ";
        QString _ejectorPositopnStr = "EjectorPosition ";
        QString _energizeStr = "Energize ";
        QString _deEnergizeStr = "de-Energize ";
        QString _quantityBasedClassCountListStr = "QuantityBasedClassCount ";
        QString _quantityBasedClassConfListStr = "QuantityBasedClassConf ";
        QString _classConfListStr = "ClassConfList ";
        QString _esp6s0GWStr = "esp5s0GW ";
        QString _esp8s0GWStr = "esp8s0GW ";
        QString _esp6s0IPStr = "esp5s0IP ";
        QString _esp8s0IPStr = "esp8s0IP ";
        QString _esp6s0SerialStr = "esp5s0Serial ";
        QString _esp8s0SerialStr = "esp8s0Serial ";



        settingFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&settingFile);

        for (int i = 0; i < mySetup.numOfCamera; ++i) {

            _red.append(myDefaultValues.redDifault);
            _redStr += QString::number(myDefaultValues.redDifault) + " ";

            _green.append(myDefaultValues.greenDifault);
            _greenStr += QString::number(myDefaultValues.greenDifault) + " ";

            _blue.append(myDefaultValues.blueDifault);
            _blueStr += QString::number(myDefaultValues.blueDifault) + " ";

            _gamma.append(myDefaultValues.gammaDifault);
            _gammaStr += QString::number(myDefaultValues.gammaDifault) + " ";

            _gain.append(myDefaultValues.gainDifault);
            _gainStr += QString::number(myDefaultValues.gainDifault) + " ";

            _exp.append(myDefaultValues.expoDifault);
            _expStr += QString::number(myDefaultValues.expoDifault) + " ";

            _esp6s0Serial.append(myDefaultValues._esp6s0Serial[i]);
            _esp6s0SerialStr += myDefaultValues._esp6s0Serial[i] + " ";

            _esp8s0Serial.append(myDefaultValues._esp8s0Serial[i]);
            _esp8s0SerialStr += myDefaultValues._esp8s0Serial[i] + " ";

            _esp6s0IP.append(myDefaultValues._esp6s0IP[i]);
            _esp6s0IPStr += myDefaultValues._esp6s0IP[i] + " ";

            _esp8s0IP.append(myDefaultValues._esp8s0IP[i]);
            _esp8s0IPStr += myDefaultValues._esp8s0IP[i] + " ";


        }

        for (int i = 0; i < myDefaultValues.classConfListDifault.size(); ++i) {

            _quantityBasedClassCountList.append(myDefaultValues.quantityBasedClassCountListDifault[i]);
            _quantityBasedClassCountListStr += QString::number(myDefaultValues.quantityBasedClassCountListDifault[i]) + " ";

            _quantityBasedClassConfList.append(myDefaultValues.quantityBasedClassConfListDifault[i]);
            _quantityBasedClassConfListStr += QString::number(myDefaultValues.quantityBasedClassConfListDifault[i]) + " ";

            _classConfList.append(myDefaultValues.classConfListDifault[i]);
            _classConfListStr += QString::number(myDefaultValues.classConfListDifault[i]) + " ";


        }

        _esp6s0GW = myDefaultValues._esp6s0GW;
        _esp6s0GWStr += myDefaultValues._esp6s0GW;

        _esp8s0GW = myDefaultValues._esp8s0GW;
        _esp8s0GWStr += myDefaultValues._esp8s0GW;

        _energize = myDefaultValues.energizeDifault;
        _energizeStr += QString::number(myDefaultValues.energizeDifault);

        _deEnergize = myDefaultValues.deEnergizeDifault;
        _deEnergizeStr += QString::number(myDefaultValues.deEnergizeDifault);

        _ejectorPositopn = myDefaultValues.ejectorPositionDifault;
        _ejectorPositopnStr += QString::number(myDefaultValues.ejectorPositionDifault);


        out << _redStr + "\n";
        out << _greenStr + "\n";
        out << _blueStr + "\n";
        out << _gammaStr + "\n";
        out << _gainStr + "\n";
        out << _expStr + "\n";
        out << _ejectorPositopnStr + "\n";
        out << _energizeStr + "\n";
        out << _deEnergizeStr + "\n";
        out << _quantityBasedClassCountListStr + "\n";
        out << _quantityBasedClassConfListStr + "\n";
        out << _classConfListStr + "\n";
        out << _esp6s0GWStr + "\n";
        out << _esp6s0SerialStr + "\n";
        out << _esp6s0IPStr + "\n";
        out << _esp8s0GWStr + "\n";
        out << _esp8s0SerialStr + "\n";
        out << _esp8s0IPStr + "\n";

        settingFile.close();
    }

    qDebug() << "--- m_classConfList : " << _classConfList;
    qDebug() << "--- m_quantityBasedClassCountList : " << _quantityBasedClassCountList;
    qDebug() << "--- m_quantityBasedClassConfList : " << _quantityBasedClassConfList;

    setRed(_red);
    setGreen(_green);
    setBlue(_blue);
    setGamma(_gamma);
    setGain(_gain);
    setExposure(_exp);
    setEjectorPosition(_ejectorPositopn);
    setEnergize(_energize);
    setDeEnergize(_deEnergize);
    setClassConfList(_classConfList);
    setQuantityBasedClassCountList(_quantityBasedClassCountList);
    setQuantityBasedClassConfList(_quantityBasedClassConfList);
    setEsp6s0GW(_esp6s0GW);
    setEsp8s0GW(_esp8s0GW);
    setEsp6s0IP(_esp6s0IP);
    setEsp8s0IP(_esp8s0IP);
    setEsp6s0Serial(_esp6s0Serial);
    setEsp8s0Serial(_esp8s0Serial);
    set_Camera_Ethernet_Setting();

    _processor.setterMySetup(mySetup.multiFrame, mySetup.numOfLines);


}

CameraController::~CameraController()
{
    stopAnalise();
}

void CameraController::resetSettingFile()
{
    QString _red = "Red ";
    for (int i = 0; i < m_red.length(); ++i)
        _red += QString::number(m_red[i]) + " ";

    QString _green = "Green ";
    for (int i = 0; i < m_green.length(); ++i)
        _green += QString::number(m_green[i]) + " ";

    QString _blue = "Blue ";
    for (int i = 0; i < m_blue.length(); ++i)
        _blue += QString::number(m_blue[i]) + " ";

    QString _gamma = "Gamma ";
    for (int i = 0; i < m_gamma.length(); ++i)
        _gamma += QString::number(m_gamma[i]) + " ";

    QString _gain = "Gain ";
    for (int i = 0; i < m_gain.length(); ++i)
        _gain += QString::number(m_gain[i]) + " ";

    QString _exposure = "Exposure ";
    for (int i = 0; i < m_exposure.length(); ++i)
        _exposure += QString::number(m_exposure[i]) + " ";

    //    QString _shift = "Shift " + QString::number(m_shift);
    QString _ejectorPositopn = "EjectorPosition " + QString::number(m_ejectorPosition);
    QString _energize = "Energize " + QString::number(m_energize);
    QString _deEnergize = "de-Energize " + QString::number(m_deEnergize);


    if(QFile(_settingPath).exists()){
        QFile settingFile(_settingPath);
        settingFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&settingFile);

        out << _red + "\n";
        out << _green + "\n";
        out << _blue + "\n";
        out << _gamma + "\n";
        out << _gain + "\n";
        out << _exposure + "\n";
        //        out << _shift + "\n";
        out << _ejectorPositopn + "\n";
        out << _energize + "\n";
        out << _deEnergize + "\n";

        settingFile.close();
    }
}

void CameraController::set_Camera_Ethernet_Setting()
{
    qDebug() << "---" << Q_FUNC_INFO;
    QProcess process;
    const QString NetMask = "255.255.255.0";

    for(int i=0; i<m_esp6s0IP.length(); i++){
        process.start("tcam-gigetool", QStringList() << "rescue" << "--ip" << m_esp6s0IP[i] << "--netmask" << NetMask << "--gateway" << m_esp6s0GW << m_esp6s0Serial[i]);
        process.waitForFinished();
    }
    for(int i=0; i<m_esp8s0IP.length(); i++){
        process.start("tcam-gigetool", QStringList() << "rescue" << "--ip" << m_esp8s0IP[i] << "--netmask" << NetMask << "--gateway" << m_esp8s0GW << m_esp8s0Serial[i]);
        process.waitForFinished();
    }
}

bool CameraController::analyseStatue() const
{
    return m_analyseStatue;
}
void CameraController::setAnalyseStatue(bool newAnalyseStatue)
{
    qDebug() << "---" << Q_FUNC_INFO << newAnalyseStatue;

    if (m_analyseStatue == newAnalyseStatue)
        return;
    m_analyseStatue = newAnalyseStatue;
    Q_EMIT analyseStatueChanged();
}

bool CameraController::readyToProcess() const
{
    return m_readyToProcess;
}
void CameraController::setReadyToProcess(bool newReadyToProcess)
{
    m_readyToProcess = newReadyToProcess;
    Q_EMIT readyToProcessChanged();
}

bool CameraController::saveResult() const
{
    return m_saveResult;
}
void CameraController::setSaveResult(bool newSaveResult)
{
    qDebug() << "---" << Q_FUNC_INFO << newSaveResult;

    if (m_saveResult == newSaveResult)
        return;

    m_saveResult = newSaveResult;
    _processor._saveResult = m_saveResult;
    Q_EMIT saveResultChanged();
}

const QVector<QString> &CameraController::classNamesList() const
{
    return m_classNamesList;
}
void CameraController::setClassNamesList(const QVector<QString> &newClassNamesList)
{
    m_classNamesList = newClassNamesList;
    Q_EMIT classNamesListChanged();
}

QString CameraController::esp6s0GW() const
{
    return m_esp6s0GW;
}
void CameraController::setEsp6s0GW(QString newEsp6s0GW)
{
    qDebug() << "---" << Q_FUNC_INFO << newEsp6s0GW;
    m_esp6s0GW = newEsp6s0GW;
    Q_EMIT esp6s0GWChanged();
}

QString CameraController::esp8s0GW() const
{
    return m_esp8s0GW;
}
void CameraController::setEsp8s0GW(QString newEsp8s0GW)
{
    qDebug() << "---" << Q_FUNC_INFO << newEsp8s0GW;
    m_esp8s0GW = newEsp8s0GW;
    Q_EMIT esp8s0GWChanged();
}

QVector<QString> CameraController::esp6s0IP() const
{
    return m_esp6s0IP;
}
void CameraController::setEsp6s0IP(QVector<QString> newEsp6s0IP)
{
    qDebug() << "---" << Q_FUNC_INFO << newEsp6s0IP;
    m_esp6s0IP = newEsp6s0IP;
    Q_EMIT esp6s0IPChanged();
}

QVector<QString> CameraController::esp8s0IP() const
{
    return m_esp8s0IP;
}
void CameraController::setEsp8s0IP(QVector<QString> newEsp8s0IP)
{
    qDebug() << "---" << Q_FUNC_INFO << newEsp8s0IP;
    m_esp8s0IP = newEsp8s0IP;
    Q_EMIT esp8s0IPChanged();
}

QVector<QString> CameraController::esp6s0Serial() const
{
    return m_esp6s0Serial;
}
void CameraController::setEsp6s0Serial(QVector<QString> newEsp6s0Serial)
{
    qDebug() << "---" << Q_FUNC_INFO << newEsp6s0Serial;
    m_esp6s0Serial = newEsp6s0Serial;
    Q_EMIT esp6s0SerialChanged();
}

QVector<QString> CameraController::esp8s0Serial() const
{
    return m_esp8s0Serial;
}
void CameraController::setEsp8s0Serial(QVector<QString> newEsp8s0Serial)
{
    qDebug() << "---" << Q_FUNC_INFO << newEsp8s0Serial;
    m_esp8s0Serial = newEsp8s0Serial;
    Q_EMIT esp8s0SerialChanged();
}

QVector<bool> CameraController::classEnableList() const
{
    return m_classEnableList;
}
void CameraController::setClassEnableList(QVector<bool> newClassEnableList)
{
    if (m_classEnableList == newClassEnableList)
        return;
    m_classEnableList = newClassEnableList;
    Q_EMIT classEnableListChanged();
}
void CameraController::setClassesEnabled(int _id, bool _enb)
{
    qDebug() << "---" << Q_FUNC_INFO << m_classNamesList[_id] << " ->" << _enb;

    try {
        m_classEnableList[_id] = _enb;
    } catch (...) {
        qDebug()<< "Can not set enable/disable this class:";
    }
}

const QVector<int> &CameraController::classConfList() const
{
    return m_classConfList;
}
void CameraController::setClassConfList(const QVector<int> &newClassConfList)
{
    m_classConfList = newClassConfList;
    Q_EMIT classConfListChanged();
}
void CameraController::setClassesConf(int _id, int _conf)
{
    qDebug() << "---" << Q_FUNC_INFO << m_classNamesList[_id] << " ->" << _conf;

    try {
        m_classConfList[_id] = _conf;
    } catch (...) {
        qDebug()<< "Can not save this Confidence:" << _conf;
    }
}


bool CameraController::quantityEnable() const
{
    return m_quantityEnable;
}
void CameraController::setQuantityEnable(bool newQuantityEnable)
{
    qDebug() << "---" << Q_FUNC_INFO << newQuantityEnable;

    if (m_quantityEnable == newQuantityEnable)
        return;

    m_quantityEnable = newQuantityEnable;
    //    _processor._saveResult = m_quantityEnable; //ToDo
    Q_EMIT quantityEnableChanged();
}

const QVector<int> &CameraController::quantityBasedClassCountList() const
{
    return m_quantityBasedClassCountList;
}
void CameraController::setQuantityBasedClassCountList(const QVector<int> &newQuantityBasedClassCountList)
{
    m_quantityBasedClassCountList = newQuantityBasedClassCountList;
    Q_EMIT quantityBasedClassCountListChanged();
}

const QVector<float> &CameraController::quantityBasedClassConfList() const
{
    return m_quantityBasedClassConfList;
}
void CameraController::setQuantityBasedClassConfList(const QVector<float> &newQuantityBasedClassConfList)
{
    m_quantityBasedClassConfList = newQuantityBasedClassConfList;
    Q_EMIT quantityBasedClassConfListChanged();
}

void CameraController::setclassQuantity(int _id, int _qnt)
{
    qDebug() << "---" << Q_FUNC_INFO << m_classNamesList[_id] << " ->" << _qnt;

    try {
        m_quantityBasedClassCountList[_id] = _qnt;
    } catch (...) {
        qDebug()<< "Can not save this Confidence:" << _qnt;
    }
}

const QVector<bool> &CameraController::classQuantityEnabledList() const
{
    return m_classQuantityEnabledList;
}
void CameraController::setclassQuantityEnabledList(const QVector<bool> &newclassQuantityEnabledList)
{
    m_classQuantityEnabledList = newclassQuantityEnabledList;
    Q_EMIT classQuantityEnabledListChanged();
}
void CameraController::setclassEnabledQuantity(int _id, bool _qntEnable)
{
    qDebug() << "---" << Q_FUNC_INFO << m_classNamesList[_id] << " ->" << _qntEnable;

    try {
        m_classQuantityEnabledList[_id] = _qntEnable;
    } catch (...) {
        qDebug()<< "Can not save this Confidence:" << _qntEnable;
    }
}

////
const QVector<int> &CameraController::classesCount() const
{
    return m_classesCount;
}
void CameraController::setClassesCount(const QVector<int> &newClassesCount)
{
    if (m_classesCount == newClassesCount)
        return;

    m_classesCount = newClassesCount;
    Q_EMIT classesCountChanged();
}
void CameraController::increaseClassesCountByID(int _id)
{
    try {
        m_classesCount[_id] += 1;
        Q_EMIT classesCountChanged();
    } catch (...) {
        qDebug()<< "Can not increase this classes counter:" << m_classNamesList[_id];
    }
}

int CameraController::ejectorPosition() const
{
    return m_ejectorPosition;
}
void CameraController::setEjectorPosition(int newEjectorPosition)
{
    qDebug() << "---" << Q_FUNC_INFO << newEjectorPosition;

    if (m_ejectorPosition == newEjectorPosition)
        return;
    m_ejectorPosition = newEjectorPosition;
    Q_EMIT ejectorPositionChanged();
}

int CameraController::energize() const
{
    return m_energize;
}
void CameraController::setEnergize(int newEnergize)
{
    qDebug() << "---" << Q_FUNC_INFO << newEnergize;

    if (m_energize == newEnergize)
        return;
    m_energize = newEnergize;
    Q_EMIT energizeChanged();
}

int CameraController::deEnergize() const
{
    return m_deEnergize;
}
void CameraController::setDeEnergize(int newDeEnergize)
{
    qDebug() << "---" << Q_FUNC_INFO << newDeEnergize;

    if (m_deEnergize == newDeEnergize)
        return;
    m_deEnergize = newDeEnergize;
    Q_EMIT deEnergizeChanged();
}

QVector<double> CameraController::red() const
{
    return m_red;
}
void CameraController::setRed(QVector<double> newRed)
{
    qDebug() << "---" << Q_FUNC_INFO << newRed;
    m_red = newRed;
    Q_EMIT redChanged();
}
void CameraController::setRed_idx(double newRed, int idx)
{
    m_red[idx] = newRed;
}

QVector<double> CameraController::green() const
{
    return m_green;
}
void CameraController::setGreen(QVector<double> newGreen)
{
    qDebug() << "---" << Q_FUNC_INFO << newGreen;

    m_green = newGreen;
    Q_EMIT greenChanged();
}
void CameraController::setGreen_idx(double newGreen, int idx)
{
    m_green[idx] = newGreen;
}

QVector<double> CameraController::blue() const
{
    return m_blue;
}
void CameraController::setBlue(QVector<double> newBlue)
{
    qDebug() << "---" << Q_FUNC_INFO << newBlue;

    m_blue = newBlue;
    Q_EMIT blueChanged();
}
void CameraController::setBlue_idx(double newBlue, int idx)
{
    m_blue[idx] = newBlue;
}

QVector<double> CameraController::gamma() const
{
    return m_gamma;
}
void CameraController::setGamma(QVector<double> newGamma)
{
    qDebug() << "---" << Q_FUNC_INFO << newGamma;

    m_gamma = newGamma;
    Q_EMIT gammaChanged();
}
void CameraController::setGamma_idx(double newGamma, int idx)
{
    m_gamma[idx] = newGamma;
}

QVector<double> CameraController::exposure() const
{
    return m_exposure;
}
void CameraController::setExposure(QVector<double> newExposure)
{
    qDebug() << "---" << Q_FUNC_INFO << newExposure;

    m_exposure = newExposure;
    Q_EMIT exposureChanged();
}
void CameraController::setExposure_idx(double newExposure, int idx)
{
    m_exposure[idx] = newExposure;
}

QVector<double> CameraController::gain() const
{
    return m_gain;
}
void CameraController::setGain(QVector<double> newGain)
{
    qDebug() << "---" << Q_FUNC_INFO << newGain;

    m_gain = newGain;
    Q_EMIT gainChanged();
}
void CameraController::setGain_idx(double newGain, int idx)
{
    m_gain[idx] = newGain;
}

int CameraController::batchID() const
{
    return m_batchID;
}
void CameraController::stepBatchID()
{
    m_batchID++;

    if(Q_UNLIKELY(restartFlag && analyseStatue() && m_batchID > mySetup.batchIDIgnogance)){
        setReadyToProcess(true);
        restartFlag = false;
    }

    Q_EMIT batchIDChanged();
}
void CameraController::setBatchID(int newBatchID)
{
    m_batchID = newBatchID;
    Q_EMIT batchIDChanged();
}

int CameraController::total() const
{
    return m_total;
}
void CameraController::increaseTotalFruits(){
    //    if(Q_UNLIKELY(restartFlag && analyseStatue() && m_batchID > mySetup.batchIDIgnogance))
    if(Q_UNLIKELY(m_batchID > mySetup.batchIDIgnogance))
        m_total += 1;
    Q_EMIT totalChanged();
}
void CameraController::setTotal(int newtotal)
{
    //    qDebug() << "m total : " << newtotal;
    m_total = newtotal;
    Q_EMIT totalChanged();
}

void CameraController::setEncoder(int newEncoder){
    m_encoder = newEncoder;
    Q_EMIT encoderChanged();
    //    mainLoop();
}
int CameraController::getEncoder() const{
    return m_encoder;
}

QString CameraController::elapsedTime() const
{
    return m_elapsedTime;
}
void CameraController::setElapsedTime(QString newElapsedTime)
{
    m_elapsedTime = newElapsedTime;
    Q_EMIT elapsedTimeChanged();
}

const QVector<QString> &CameraController::serials() const
{
    return mySetup.m_serials;
}
void CameraController::setSerials(const QVector<QString> &newSerials)
{
    //    if (m_serials == newSerials)
    //        return;
    //    m_serials = newSerials;
    Q_UNUSED(newSerials);

    //    Q_EMIT serialsChanged();
}

bool CameraController::ethStatus() const
{
    return m_ethStatus;
}
void CameraController::setEthStatus(bool newEthStatus)
{
    m_ethStatus = newEthStatus;
    Q_EMIT ethStatusChanged();
}

bool CameraController::ardStatus() const
{
    return m_ardStatus;
}
void CameraController::setArdStatus(bool newArdStatus)
{
    m_ardStatus = newArdStatus;
    Q_EMIT ardStatusChanged();
}

bool CameraController::cameraStatus(QString Serial)
{
    return gsttcam::TcamCamera::SerialExists(Serial.toStdString());
}

void CameraController::stopAnalise()
{
    qDebug() << "---" << Q_FUNC_INFO;

    setAnalyseStatue(false);
    setReadyToProcess(false);
    releaseTheCameras();

    switch(com_type){
    case 0:
    {
        setEthStatus(false);
        disconnect(udpSocketReceive, SIGNAL(readyRead()), this, SLOT(readDataEthernet()));   //When a packet is received, call onPacketRecieved()
        //        disconnect(this, SIGNAL(sendChanged()), this, SLOT(writeDataEthernet()));
        udpSocketReceive->close();
        udpSocketSend->close();

        grabFirst = false;
        grabSecond = false;
        retrieveFirst = false;
        retrieveSecond = false;

        break;
    }
    case 1:
    {
        setArdStatus(false);
        closeSerialPort();
        disconnect(this,SIGNAL(encoderChanged()), this, SLOT(mainLoop()));
        disconnect(timer, &QTimer::timeout, this, &CameraController::timerFunction);

        grabFirst = false;
        grabSecond = false;
        retrieveFirst = false;
        retrieveSecond = false;



        break;
    }
    default:
        qInfo() << "Invalid com_type.";
    }


}
void CameraController::startAnalise()
{

    _processor.setModelPath(mySetup.modelPath);
    _processor.setClassesList(m_classNamesList);
    _processor.setOutputSize(cv::Size(mySetup.height, mySetup.height));

    modelWarmUp();

    Start_time = QTime::currentTime();
    _camPackList.clear();
    preparingTheCameras();
    setBatchID(0);
    m_total = 0;
    setClassesCount({0,0,0,0,0,0,0,0,0}); //TODO

    _NumberOfDefectsInEachImageOfBatchPrev.clear(); // Dim: numOfLines x numOfClasses
    lineClassValidityTotalPrev.clear();
    QVector<int> zeroVector(m_classNamesList.size(),0);
    for (int j = 0; j < mySetup.batchSize; j++) {
        _NumberOfDefectsInEachImageOfBatchPrev.append(zeroVector);
        lineClassValidityTotalPrev.append(zeroVector);
    }

    _ejectorQueue.clear(); // Dim: mySetup.maxEjectorLine x numOfLines
    for (int j = 0; j < mySetup.maxEjectorLine; j++) {
        QVector<int> zeroVector(mySetup.numOfLines,0); /// TODO
        _ejectorQueue.append(zeroVector);
    }

    for (int j = 0; j < 2; j++){ // Dim: 2(new and old) x numOfLines
        QVector<int> zeroVector(mySetup.numOfLines,0); /// TODO
        _prevbatchRes.append(zeroVector);
    }

    restartFlag = true;
    setAnalyseStatue(true);

    QString _folderPath = _rootPath + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");

    if(!QDir(_folderPath).exists())
        QDir().mkdir(_folderPath);

    QString _picPath = _folderPath + "/Images/";
    if(!QDir(_picPath).exists())
        QDir().mkdir(_picPath);

    QString _reportPath = _folderPath + "/Reports/";
    if(!QDir(_reportPath).exists())
        QDir().mkdir(_reportPath);

    _processor._folderPath = _picPath;

    switch(com_type){
    case 0:
    {
        qInfo() << "Ethernet connection...";
        // Receive Data
        udpSocketReceive = new QUdpSocket(this);
        udpSocketReceive->bind(mySetup.receiverPort, QUdpSocket::ShareAddress); //Allow other services to bind to the same address and port.
        bool success_receive_connection_eth = connect(udpSocketReceive, SIGNAL(readyRead()), this, SLOT(readDataEthernet()));   //When a packet is received, call onPacketRecieved()
        qDebug() << "Check Assert Receive" << success_receive_connection_eth;

        // Send Data
        udpSocketSend = new QUdpSocket(this); //create UDP socket to send to
        udpSocketSend->bind(mySetup.senderPort,QUdpSocket::ShareAddress); //Allow other services to bind to the same address and port.
        //        bool success_send_connection_eth = connect(this, SIGNAL(sendChanged()), this, SLOT(writeDataEthernet()));
        //        qDebug() << "Check Assert Send" << success_send_connection_eth;

        connect(timer, &QTimer::timeout, this, &CameraController::timerFunction);
        timer->start(2000);


        setEthStatus(true);
        break;
    }
    case 1:
    {
        qInfo() << "--- Serial connection is initialized";

        openSerialPort();
        QThread::msleep(1000);

        //QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CameraController::timerFunction);
        timer->start(2000);

        bool success_receive_connection_ = connect(serial, SIGNAL(readyRead()), this, SLOT(readDataSerial_thread()));   //When a packet is received, call onPacketRecieved()
        connect(this,SIGNAL(encoderChanged()), this, SLOT(mainLoop()));
        //QtConcurrent::run(this,&CameraController::readDataSerial_thread);

        setArdStatus(true);



        break;
    }
    default:
        qInfo() << "Invalid com_type.";

    }
}

void CameraController::modelWarmUp()
{
    qInfo() << "------ o" << Q_FUNC_INFO;
    int warmup_epochs=10;
    for(int i=0; i<warmup_epochs; i++){

        cv::Mat blackFrame = cv::Mat::zeros(mySetup.height, mySetup.height, CV_8UC3);
        std::vector<cv::Mat> blackFrameBatch;

        for(int j=0; j< mySetup.batchSize; j++){
            blackFrameBatch.push_back(blackFrame);
        }


        std::vector<cv::Mat> _outputFrames(blackFrameBatch.size());
        _processor(blackFrameBatch, _outputFrames, m_batchID, mySetup.numOfLines);

    }
    qInfo() << "------ ✓ model warmup done.";
}

void CameraController::timerFunction(){

    switch(com_type){

    case 0:{
        QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
        (interfaces[2].flags() & QNetworkInterface::IsUp) ? setEthStatus(true) : setEthStatus(false);
        break;
    }

    case 1:{

        QStringList serialPorts = serialPortCheck();

        bool serialPortNameRecognition = !serialPorts.isEmpty();
        bool serialPortIsOpen =  serial->isOpen();
        bool serialIsRequestToSend = serial->isRequestToSend();
        QString serialPortName = serialPortNameRecognition ? serialPorts[0] : "empty";
        //    qDebug() << Q_FUNC_INFO << serialPortNameRecognition << serialPortIsOpen << serialIsRequestToSend;


        //    && m_ardStatus
        if(!serialPortNameRecognition || !serialPortIsOpen || !serialIsRequestToSend)
            qCritical() << "✗✗✗ Serial Port Status Error" << serialPortNameRecognition << serialPortIsOpen << serialIsRequestToSend;

        if(!serialPortNameRecognition){

            qCritical() << "✗✗✗ ttyACM* Serial port disconnected.";
            closeSerialPort();
            setArdStatus(false);
        }

        if(serialPortNameRecognition && !serialPortIsOpen){

            qCritical() << "✗✗✗ Serial port is closed, trying to open...";
            closeSerialPort();

            _ejectorQueue.clear();
            for (int j = 0; j < mySetup.maxEjectorLine; j++) {

                QVector<int> temp_; /// TODO
                for (int i = 0; i < mySetup.numOfLines; i++) {
                    temp_.append(0);
                }
                _ejectorQueue.append(temp_);
            }

            openSerialPort();
            QThread::msleep(1000);

            bool success_receive_connection_ = connect(serial, SIGNAL(readyRead()), this, SLOT(readDataSerial_thread()));   //When a packet is received, call onPacketRecieved()
            //QtConcurrent::run(this,&CameraController::readDataSerial_thread);
            setArdStatus(true);
        }

        if(serialPortNameRecognition && serialPortIsOpen && !serialIsRequestToSend){

            // In Cases when arduino dis.con. and con. rapidly, all values(serialPortNameRecognition,serialPortIsOpen) are true like normal operation
            qCritical() << "✗✗✗ Serial port signal/slot Connection lost, trying to make another connection";
            closeSerialPort();

            _ejectorQueue.clear();
            for (int j = 0; j < mySetup.maxEjectorLine; j++) {

                QVector<int> temp_; /// TODO
                for (int i = 0; i < mySetup.numOfLines; i++) {
                    temp_.append(0);
                }
                _ejectorQueue.append(temp_);
            }

            openSerialPort();
            QThread::msleep(1000);

            bool success_receive_connection_ = connect(serial, SIGNAL(readyRead()), this, SLOT(readDataSerial_thread()));   //When a packet is received, call onPacketRecieved()
            //QtConcurrent::run(this,&CameraController::readDataSerial_thread);
            setArdStatus(true);
        }


        break;
    }

    default:
        qWarning() << " com_type error in timer";
    }


}

void CameraController::preparingTheCameras()
{
    qDebug() << "---" << Q_FUNC_INFO;

    for (int i = 0; i < mySetup.m_serials.size(); i++) {
        CaptureThread *_capture = new CaptureThread;
        camPack _pack;
        _pack.status = cameraStatus(mySetup.m_serials[i]);

        _capture->width = mySetup.width;
        _capture->height = mySetup.height;

        if (_pack.status){
            QString pipeline_description_str = "tcambin name=source serial=" + mySetup.m_serials[i] + " TriggerMode=on ! video/x-raw, format=BGRx, width=" + QString::number(mySetup.width) + ", height=" + QString::number(mySetup.height) + ", framerate=50/1 ! videoconvert ! appsink name=sink";
            _capture->setCapture(pipeline_description_str, m_gain[i], m_exposure[i], m_red[i], m_green[i], m_blue[i], m_gamma[i]); // This line gives error if serial in pipeline be invalid
        }


        _capture->setCaptureID(i);
        _pack.capture = _capture;
        _camPackList.push_back(_pack);
    }
}

void CameraController::releaseTheCameras() {
    qDebug() << "---" << Q_FUNC_INFO;
    for(auto &_pack : _camPackList) {
        _pack.capture->releaseCapture();
        _pack.capture->quit();
    }
}

QVector<cv::Mat> CameraController::separateLeftAndRigth(cv::Mat& frame){

    QVector<cv::Mat> output;
    cv::Mat left = frame(cv::Range(0,768), cv::Range(0,768)).clone();
    output.append(left);
    cv::Mat right = frame(cv::Range(0,768), cv::Range(672,1440)).clone();
    output.append(right);
    return output;
}
QVector<cv::Mat> CameraController::separateMiddle(cv::Mat& frame){

    QVector<cv::Mat> output;
    cv::Mat middle = frame(cv::Range(0,768), cv::Range(336,1104)).clone();
    output.append(middle);
    return output;
}

void CameraController::calElapsedTime()
{
    auto Current_time = QTime::currentTime();
    auto totalNumberOfSeconds = Start_time.secsTo(Current_time);
    int seconds = totalNumberOfSeconds % 60;
    int minutes = (totalNumberOfSeconds / 60) % 60;
    int hours = (totalNumberOfSeconds / 60 / 60);

    QString timeString = QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));

    setElapsedTime(timeString);
}

void CameraController::openSerialPort(){
    serial = new QSerialPort;

    // give permission to the available ttyACM*
    QProcess process;
    QStringList arguments;
    arguments << "-c" << "echo " + mySetup.systemPassword +" | sudo -S chmod +666 /dev/ttyACM*";
    process.start("bash", arguments);
    process.waitForFinished();

    QDir lsDev("/dev/");
    lsDev.setNameFilters(QStringList() << "ttyACM*");
    if(!lsDev.entryList(QDir::System).isEmpty()){
        serial->setPortName(lsDev.entryList(QDir::System)[0]);
        serial->setBaudRate(QSerialPort::Baud115200);
        //        serial->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        if (serial->open(QIODevice::ReadWrite)) {
            qDebug() << "--- Port Opened";
        } else {
            qDebug() << "Error" << serial->errorString();
        }
    }
    else{
        qDebug() << "✗✗✗ ttyACM* port not detected.  "; return ;
    }
}
void CameraController::closeSerialPort(){
    serial->close();
}
QStringList CameraController::serialPortCheck(){
    //    qInfo() << "---" << Q_FUNC_INFO;

    QDir lsDev("/dev/");
    lsDev.setNameFilters(QStringList() << "ttyACM*");
    QStringList portNames = lsDev.entryList(QDir::System);
    return portNames;
}
void CameraController::writeDataSerial(QString ejectorText){

    if(ejectorText != "J ")
        qDebug() << Q_FUNC_INFO << ejectorText;

    serial->write(ejectorText.toLatin1());
    serial->flush();
}
void CameraController::readDataSerial_thread()
{
    //qDebug() << "-------------------------------------------------------------- start";
    //serial->clear();
    //serial->clearError();
    //serial->readAll();
    int _encoder;
    while (serial->canReadLine()) {
        _encoder = QString(serial->readLine().data()).split("\r\n")[0].toInt();
        //qDebug() << "encoder: " << _encoder;
        if(mySetup._printEncoderClockFlag)
            qDebug() << "Clock : " << _encoder << QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss,zzz").split(" ")[1];

        setEncoder(_encoder);


    }
    //qDebug() << "-------------------------------------------------------------- end";
}

void CameraController::writeDataEthernet(QString ejectorText)
{

    if(ejectorText != "Z ")
        qDebug() << Q_FUNC_INFO << ejectorText;

    QString txPacket = ejectorText;
    QByteArray txDatagram;
    txDatagram = txPacket.toUtf8();
    QHostAddress ard_Address(QStringLiteral("192.168.1.3"));
    udpSocketSend->writeDatagram(txDatagram.data(),txDatagram.size(), ard_Address,mySetup.senderPort);
}
void CameraController::readDataEthernet()
{
    if(!m_analyseStatue)
        return;

    QByteArray datagram;
    do
    {
        datagram.resize(udpSocketReceive->pendingDatagramSize());
        udpSocketReceive->readDatagram(datagram.data(), datagram.size());
        signed int _encoder_ = (QByteArray::number(datagram.data()[1]).toInt())+128;


        //        int _encoder = abs(_encoder_-99); //Tehran real sorter
        int _encoder = _encoder_; //Tehran fake

        setEncoder(_encoder);

        if(mySetup._printEncoderClockFlag)
            qDebug() << "Clock : " << _encoder;
    }
    while(udpSocketReceive->hasPendingDatagrams());

    mainLoop();
}

void CameraController::mainLoop(){


    //grab single frame
    if (m_encoder>mySetup.grabTimes[0][0] && m_encoder<mySetup.grabTimes[0][1] && !grabFirst  && !mySetup.multiFrame){
        qDebug() << " ------- grab" ;
        try {
            for(auto &_pac : _camPackList){
                _pac.capture->grabFrame();
            }
        } catch (...) {
            qWarning("Failed to grab first image!");
        }

        grabFirst = true;

    }

    // retrieve single frame
    if ((m_encoder > mySetup.retrieveTimes[0][0]) && (m_encoder<mySetup.retrieveTimes[0][1]) && grabFirst && !retrieveFirst && !mySetup.multiFrame){
        qDebug() << " ------- retrieve" ;
        _framesBatch.clear();
        for(auto &_pac : _camPackList){
            auto f = _pac.capture->retrieveFrame();
            if(Q_LIKELY(!f.empty())){
                _framesBatch.push_back(f);
            }
        }
        if((int)_framesBatch.size() != mySetup.batchSize) // what if not be true? were it will be trapped? output table will be lagged?
            qDebug() << "EEEEEEEEEEEEERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRROOOOOOOOOOOOOOOOOOOOOOOOOORRRRRRRRRRRRRRRRRRRRRR" << _framesBatch.size();
        retrieveFirst = true;

    }

    // process
    if ((m_encoder > mySetup.processTime[0][0]) && (m_encoder<mySetup.processTime[0][1]) && (!mySetup.multiFrame && retrieveFirst)){
        grabFirst = false;
        retrieveFirst = false;

        qDebug() << " ------- Inference" ;

        std::vector<cv::Mat> _framesBatchReadyForInference;
        for(const auto &frm : _framesBatch)
            _framesBatchReadyForInference.push_back(frm.clone());
        _framesBatch.clear();

        ejectorLocker.lock();
        switch(_framesBatchReadyForInference.size()){
        case 2:{
            //            qDebug() <<
            _ejectorQueue.push_front(_prevbatchRes[0]);
            break;
        }
        case 6:{
            _ejectorQueue.push_front(_prevbatchRes[0]);

            for(int i=0; i<_prevbatchRes[1].size(); i++)
                _ejectorQueue[1][i] = _ejectorQueue[1][i]  || _prevbatchRes[1][i];

            break;
        }
        default:
            qInfo() << "--- mySetup.grabTimes.size()";
        }

        while(_ejectorQueue.size() > mySetup.maxEjectorLine)
            _ejectorQueue.pop_back();
        ejectorLocker.unlock();

        stepBatchID();

        calElapsedTime();
        qDebug() << "Batch ID : " << m_batchID;

        std::vector<std::vector<Detection>> _res;
        QFutureWatcher<std::vector<std::vector<Detection>>> *watcher = new QFutureWatcher<std::vector<std::vector<Detection>>>(this);
        QObject::connect(watcher, &QFutureWatcher<std::vector<std::vector<Detection>>>::finished, [this,watcher]() {
            try {
                postProcessing(watcher->result());
            } catch (...) {
                qDebug() << "*//////////////////////////////////////////////////////////////////////////////////////////// : ";
            }

        });

        _inferenceResult = QtConcurrent::run(&_processor, &Processor::process, _framesBatchReadyForInference, _framesBatchReadyForInference, m_batchID, mySetup.numOfLines);
        watcher->setFuture(_inferenceResult);

    }




    //grab first multi frame
    if (m_encoder>mySetup.grabTimes[0][0] && m_encoder<mySetup.grabTimes[0][1] && !grabFirst  && mySetup.multiFrame){
        qDebug() << " ------- grab1" << m_encoder;
        try {
            for(auto &_pac : _camPackList){
                _pac.capture->grabFrame();
            }
        } catch (...) {
            qWarning("Failed to grab first image!");
        }

        grabFirst = true;

    }

    //retrieve first multi frame
    if ((m_encoder > mySetup.retrieveTimes[0][0]) && (m_encoder<mySetup.retrieveTimes[0][1]) && grabFirst && !retrieveFirst && mySetup.multiFrame){
        qDebug() << " ------- retrieve1" << m_encoder;
        _framesBatch.clear();
        for(auto &_pac : _camPackList){
            auto f = _pac.capture->retrieveFrame();
            auto fs = separateLeftAndRigth(f);
            if(Q_LIKELY(!f.empty())){
                _framesBatch.push_back(fs[0].clone());
                _framesBatch.push_back(fs[1].clone());
            }
        }
        if((int)_framesBatch.size() != 8) // what if not be true? were it will be trapped? output table will be lagged?
            qDebug() << "retrieve 1, _framesBatch.size() = " << _framesBatch.size()  << "EEEEEEEEEEEEERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRROOOOOOOOOOOOOOOOOOOOOOOOOORRRRRRRRRRRRRRRRRRRRRR" << _framesBatch.size();
        retrieveFirst = true;


    }

    //grab second multi frame
    if (m_encoder>mySetup.grabTimes[1][0] && m_encoder<mySetup.grabTimes[1][1] && grabFirst && retrieveFirst && !grabSecond  && mySetup.multiFrame){
        qDebug() << " ------- grab2" << m_encoder;
        try {
            for(auto &_pac : _camPackList){
                _pac.capture->grabFrame();
            }
        } catch (...) {
            qWarning("Failed to grab first image!");
        }

        grabSecond = true;
    }

    //retrieve second multi frame
    if ((m_encoder > mySetup.retrieveTimes[1][0]) && (m_encoder<mySetup.retrieveTimes[1][1]) && grabSecond && !retrieveSecond && mySetup.multiFrame){
        qDebug() << " ------- retrieve2 " << m_encoder ;
        for(auto &_pac : _camPackList){
            auto f = _pac.capture->retrieveFrame();
            auto fs = separateMiddle(f);
            if(Q_LIKELY(!f.empty())){
                _framesBatch.push_back(fs[0]);
            }
        }
        if((int)_framesBatch.size() != mySetup.batchSize) // what if not be true? were it will be trapped? output table will be lagged?
            qDebug() << "retrieve 2, _framesBatch.size() = " << _framesBatch.size()  << "EEEEEEEEEEEEERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRROOOOOOOOOOOOOOOOOOOOOOOOOORRRRRRRRRRRRRRRRRRRRRR" << _framesBatch.size();
        retrieveSecond = true;

    }


    if ((m_encoder > mySetup.processTime[0][0]) && (m_encoder<mySetup.processTime[0][1]) && ((mySetup.multiFrame && retrieveFirst && retrieveSecond))){
        qDebug() << " ------- Inference_multi" << m_encoder << _framesBatch.size();

        grabFirst = false;
        retrieveFirst = false;
        grabSecond = false;
        retrieveSecond = false;

        std::vector<cv::Mat> _framesBatchReadyForInference;
        for(const auto &frm : _framesBatch)
            _framesBatchReadyForInference.push_back(frm.clone());
        _framesBatch.clear();

        ejectorLocker.lock();
        switch(_framesBatchReadyForInference.size()){
        case 2:{
            //            qDebug() <<
            _ejectorQueue.push_front(_prevbatchRes[0]);
            break;
        }
        case 6:{
            _ejectorQueue.push_front(_prevbatchRes[0]);

            for(int i=0; i<_prevbatchRes[1].size(); i++)
                _ejectorQueue[1][i] = _ejectorQueue[1][i]  || _prevbatchRes[1][i];

            break;
        }
        default:
            qInfo() << "--- mySetup.grabTimes.size()";
        }

        while(_ejectorQueue.size() > mySetup.maxEjectorLine)
            _ejectorQueue.pop_back();
        ejectorLocker.unlock();

        stepBatchID();

        calElapsedTime();
        qDebug() << "Batch ID : " << m_batchID;

        std::vector<std::vector<Detection>> _res;
        QFutureWatcher<std::vector<std::vector<Detection>>> *watcher = new QFutureWatcher<std::vector<std::vector<Detection>>>(this);
        QObject::connect(watcher, &QFutureWatcher<std::vector<std::vector<Detection>>>::finished, [this,watcher]() {
            try {
                postProcessing(watcher->result());
            } catch (...) {
                qDebug() << "*//////////////////////////////////////////////////////////////////////////////////////////// : ";
            }

        });

        _inferenceResult = QtConcurrent::run(&_processor, &Processor::process, _framesBatchReadyForInference, _framesBatchReadyForInference, m_batchID, mySetup.numOfLines);
        watcher->setFuture(_inferenceResult);


    }


    if (abs(m_encoder-m_energize)<6){
        ejectorLocker.lock();
        QVector<int> _ejectorQueueRow = _ejectorQueue[m_ejectorPosition];
        ejectorLocker.unlock();

        QString newSend = calcEjectorVectorRowString(_ejectorQueueRow);
        if(newSend != mySetup.ejectionPrefix+" " )
            qDebug() << "------------------- Eject text" << newSend;

        com_type == 0 ? writeDataEthernet(newSend) : writeDataSerial(newSend);
    }
}

void CameraController::postProcessing(std::vector<std::vector<Detection>> _res)
{
    if(m_batchID > mySetup.batchIDIgnogance){
        QVector<QVector<int>> batchEjection = calcEjectorVectorRow(_res);
        if(2 == batchEjection.size())
            _prevbatchRes = batchEjection;
        else{
            qDebug() << "EEEEEEEEEEEEEEEEEEEEEEEEERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORRRRRRRRRRRRRRRRRRRRR";
            qDebug() << 2 << " <-- mySetup.batchSize != batchEjection.size() -->" << batchEjection.size();

            // Filling batchEjection with zeros
            QVector<QVector<int>> batchEjection;//{{x,x},{x,x}}
            for (int j = 0; j < 2; j++){
                QVector<int> temp_; /// TODO
                for (int i = 0; i < mySetup.numOfLines; i++) {
                    temp_.append(0);
                }
                batchEjection.append(temp_); // Dim: 2(new and old) x numOfLines
            }
        }
    }
}

QVector<QVector<int>> CameraController::calcEjectorVectorRow(std::vector<std::vector<Detection>> _detList)
{

    // Creating empty matrix of batchEjection with size of: 2(new and old) x numOfLines
    QVector<QVector<int>> batchEjection;
    QVector<int> zerosVector(mySetup.numOfLines,0);
    for (int j = 0; j < 2; j++){
        batchEjection.append(zerosVector);
    }
    assert(batchEjection.size() == 2);

    // Creating two matrix with size of: batchSize x numOfClasses. lineClassValidityTotal(filled 0,1), _NumberOfDefectsInEachImageOfBatch(number of each defects in images)
    QVector<QVector<int>> lineClassValidityTotal;
    QVector<QVector<int>> _NumberOfDefectsInEachImageOfBatch;
    int counter=0;
    for (const auto &batch : _detList) {

        QVector<int> lineClassValidity(m_classNamesList.size());
        QVector<int> defectCount(m_classNamesList.size());

        for (const auto &_detObj : batch) {
            if(m_classEnableList[_detObj.classID] && _detObj.confidence*100 >= m_classConfList[_detObj.classID]){
                lineClassValidity[_detObj.classID] = 1;
            }

            // Defect counting
            if(m_quantityEnable &&  m_classEnableList[_detObj.classID] &&
                    m_classQuantityEnabledList[_detObj.classID] && _detObj.confidence*100 >= m_quantityBasedClassConfList[_detObj.classID])
                defectCount[_detObj.classID] += 1;

            if((_detObj.classID == 5 || _detObj.classID == 6 || _detObj.classID == 7) && _detObj.confidence*100 >= 50 && ((!mySetup.multiFrame && (counter ==0 || counter ==1)) || (mySetup.multiFrame && (counter ==0 || counter ==2))) ){
                increaseTotalFruits();
            }
        }

        counter += 1;
        lineClassValidityTotal.push_back(lineClassValidity);
        _NumberOfDefectsInEachImageOfBatch.push_back(defectCount);
    }


    // if system takes single image per each cup
    if(!mySetup.multiFrame){
        for (int i = 0; i < mySetup.numOfLines; ++i) { // for each batch (image=line=2)
            for (int j = 0; j < mySetup.numOfClasses; ++j){

                batchEjection[frameTimeInBatch::NEW][i] += lineClassValidityTotal[i][j];
                (lineClassValidityTotal[i][j] >= 1) ? increaseClassesCountByID(j) : void();

                // checking the number of defects if they exceed, and applying the result in batchEjection which is the output
                if(_NumberOfDefectsInEachImageOfBatch[i][j] >= m_quantityBasedClassCountList[j]){
                    batchEjection[frameTimeInBatch::NEW][i] += 1;
                    //                    increaseClassesCountByID(j);
                }
            }
        }
    }

    if(mySetup.multiFrame){

        for (int i = 0; i < mySetup.batchSize; ++i){

            int _line = (i >= mySetup.numOfLines*2) ? (i-mySetup.numOfLines*2):(i/2);
            int _frameOldOrNew = (i < mySetup.numOfLines*2) && (i%2 != 0) ? frameTimeInBatch::OLD : frameTimeInBatch::NEW;
            for (int j = 0; j < mySetup.numOfClasses; ++j){
                batchEjection[_frameOldOrNew][_line] += lineClassValidityTotal[i][j];
            }
        }

        int _C;
        for (int _line=0; _line < mySetup.numOfLines; ++_line){
            int _lineNewIndex = 2*_line;
            int _lineOldIndex = 2*_line + 1;
            int _lineMiddleIndex = 2*mySetup.numOfLines + _line;

            for (int j=0; j < mySetup.numOfClasses; ++j){

                _C = _NumberOfDefectsInEachImageOfBatch[_lineOldIndex][j] +
                        _NumberOfDefectsInEachImageOfBatchPrev[_lineNewIndex][j] +
                        _NumberOfDefectsInEachImageOfBatchPrev[_lineMiddleIndex][j];

                if(_C >= m_quantityBasedClassCountList[j]){
                    batchEjection[frameTimeInBatch::OLD][_line] += 1;
                    //                    increaseClassesCountByID(j);
                }

                (lineClassValidityTotal[_lineOldIndex][j] >= 1 ||
                        lineClassValidityTotalPrev[_lineNewIndex][j] >= 1 ||
                        lineClassValidityTotalPrev[_lineMiddleIndex][j] >= 1 ) ? increaseClassesCountByID(j) : void();


            }
        }

        _NumberOfDefectsInEachImageOfBatchPrev = _NumberOfDefectsInEachImageOfBatch;
        lineClassValidityTotalPrev = lineClassValidityTotal;
    }

    return batchEjection;
}
QString CameraController::calcEjectorVectorRowString(QVector<int> _ejectorQueueRow)
{
    // In case of any out of boundry error

    //    qDebug() << Q_FUNC_INFO << _ejectorQueueRow;

    if(_ejectorQueueRow.size() != mySetup.numOfLines){
        QVector<int> _zeroVector(mySetup.numOfLines,0);
        _ejectorQueueRow = _zeroVector;
        qDebug() << "*** Set size equal to mySetup.numOfLines 1";
    }

    QString _finalRes_tmp = mySetup.ejectionPrefix;
    for (int i = 0; i < mySetup.numOfLines; i++)
    {
        if(_ejectorQueueRow[i] > 0)
            _finalRes_tmp += mySetup.ejectionSymbols[i];
    }

    try {
        _finalRes_tmp += " ";
    } catch (...) {
        _finalRes_tmp = mySetup.ejectionPrefix + " ";
    }

    QString _finalRes = _finalRes_tmp;
    return _finalRes;
}

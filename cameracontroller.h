#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include <vector>

#include <QCoreApplication>
#include <QtNetwork>
#include <iostream>
#include <iomanip>
#include <QMainWindow>
#include <QTimer>

using namespace std;

struct setup //TODO
{
    const int numOfCamera = 4;//2
    const int numOfLines = 4;//2
    const int numOfClasses = 9;
    const int batchSize = 12;//6
    const int width = 1440;
    const int height = 768; // we have a width and heigth in capturethread
    const QString systemPassword = "7410"; //MUST
    const QString modelPath = "/home/mostafa/Application/model.torchscript";
    const bool multiFrame = true;

    const int batchIDIgnogance = 30;

    const QVector<QVector<int>> grabTimes =     {{92,98},{37,43}};
    const QVector<QVector<int>> retrieveTimes = {{30,37},{75,83}};
    const QVector<QVector<int>> processTime = {{84,91},{-1,-1}};

//    const QVector<QVector<int>> grabTimes =     {{10,16},{-1,-1}};
//    const QVector<QVector<int>> retrieveTimes = {{26,32},{-1,-1}};
//    const QVector<QVector<int>> processTime = {{33,39},{-1,-1}};

    const QVector<QString> m_serials = {"23224172","45224169","02324172","02324181"}; //MUST
    const int receiverPort = 8569;
    const int senderPort = 8570;
    const int maxEjectorLine = 30;

    bool _printEncoderClockFlag = false;
    QString ejectionPrefix = "J";
    const QString ejectionSymbols = "ABCDEFJHIJKLMNOPQRSTUVWXYZ";


};

struct defaultValues{
    float redDifault = 1.86;
    int greenDifault = 1;
    float blueDifault = 2.14;
    int gammaDifault = 1;
    int gainDifault = 0;
    int expoDifault = 800;
    int ejectorPositionDifault = 20;
    int energizeDifault = 63;
    int deEnergizeDifault = 90;
    QVector<int> classConfListDifault = {10,10,50,45,10,75,75,10,40};
    QVector<int> quantityBasedClassCountListDifault = {3,3,3,3,3,3,3,3,3};
    QVector<float> quantityBasedClassConfListDifault = {0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15};
    QString _esp6s0GW = "192.168.6.10";
    QString _esp8s0GW = "192.168.8.10";
    QVector<QString> _esp6s0IP = {"192.168.6.101","192.168.6.102"};
    QVector<QString> _esp8s0IP = {"192.168.8.101","192.168.8.102"};
    QVector<QString> _esp6s0Serial = {"23224172","45224169"};
    QVector<QString> _esp8s0Serial = {"02324172","02324181"};


};

struct camPack
{
    CaptureThread *capture;
    bool _frameDelivered = true;
    int frameNumber = -1;
    bool status = true;
    cv::Mat orgFrame = cv::Mat::ones(1, 1, CV_8UC3);
    cv::Mat lastFrameMat = cv::Mat::ones(1, 1, CV_8UC3);
};

enum communication_type {UDP=0, Serial=1};
enum framePositionMultiImage {LINE1_NEW=0, LINE1_OLD=1, LINE2_NEW=2, LINE2_OLD=3, LINE1_MIDDLE=4, LINE2_MIDDLE=5};
enum frameTimeInBatch {NEW=0, OLD=1};
enum frameLine {LINE1=0, LINE2=1};


class CameraController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector<QString> serials READ serials WRITE setSerials NOTIFY serialsChanged)
    Q_PROPERTY(bool analyseStatue READ analyseStatue WRITE setAnalyseStatue NOTIFY analyseStatueChanged)
    Q_PROPERTY(bool readyToProcess READ readyToProcess WRITE setReadyToProcess NOTIFY readyToProcessChanged)
    Q_PROPERTY(bool saveResult READ saveResult WRITE setSaveResult NOTIFY saveResultChanged)
    Q_PROPERTY(QVector<QString> classNamesList READ classNamesList WRITE setClassNamesList NOTIFY classNamesListChanged)
    Q_PROPERTY(QVector<bool> classEnableList READ classEnableList WRITE setClassEnableList NOTIFY classEnableListChanged)
    Q_PROPERTY(QVector<int> classConfList READ classConfList WRITE setClassConfList NOTIFY classConfListChanged)
    Q_PROPERTY(QVector<int> classesCount READ classesCount WRITE setClassesCount NOTIFY classesCountChanged)
    Q_PROPERTY(int ejectorPosition READ ejectorPosition WRITE setEjectorPosition NOTIFY ejectorPositionChanged)
    Q_PROPERTY(int energize READ energize WRITE setEnergize NOTIFY energizeChanged)
    Q_PROPERTY(int deEnergize READ deEnergize WRITE setDeEnergize NOTIFY deEnergizeChanged)
    Q_PROPERTY(QVector<double> red READ red WRITE setRed NOTIFY redChanged)
    Q_PROPERTY(QVector<double> green READ green WRITE setGreen NOTIFY greenChanged)
    Q_PROPERTY(QVector<double> blue READ blue WRITE setBlue NOTIFY blueChanged)
    Q_PROPERTY(QVector<double> gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(QVector<double> exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(QVector<double> gain READ gain WRITE setGain NOTIFY gainChanged)
    Q_PROPERTY(int batchID READ batchID WRITE setBatchID NOTIFY batchIDChanged)
    Q_PROPERTY(bool ardStatus READ ardStatus WRITE setArdStatus NOTIFY ardStatusChanged)
    Q_PROPERTY(bool ethStatus READ ethStatus WRITE setEthStatus NOTIFY ethStatusChanged) //E
    Q_PROPERTY(QString elapsedTime READ elapsedTime WRITE setElapsedTime NOTIFY elapsedTimeChanged)
    Q_PROPERTY(int encoder READ getEncoder WRITE setEncoder NOTIFY encoderChanged)
    Q_PROPERTY(bool quantityEnable READ quantityEnable WRITE setQuantityEnable NOTIFY quantityEnableChanged)
    Q_PROPERTY(QVector<bool> classQuantityEnabledList READ classQuantityEnabledList WRITE setclassQuantityEnabledList NOTIFY classQuantityEnabledListChanged)
    Q_PROPERTY(QVector<int> quantityBasedClassCountList READ quantityBasedClassCountList WRITE setQuantityBasedClassCountList NOTIFY quantityBasedClassCountListChanged)
    Q_PROPERTY(QVector<float> quantityBasedClassConfList READ quantityBasedClassConfList WRITE setQuantityBasedClassConfList NOTIFY quantityBasedClassConfListChanged)
    Q_PROPERTY(int total READ total WRITE setTotal NOTIFY totalChanged)
    Q_PROPERTY(QString esp6s0GW READ esp6s0GW WRITE setEsp6s0GW NOTIFY esp6s0GWChanged)
    Q_PROPERTY(QString esp8s0GW READ esp8s0GW WRITE setEsp8s0GW NOTIFY esp8s0GWChanged)
    Q_PROPERTY(QVector<QString> esp6s0IP READ esp6s0IP WRITE setEsp6s0IP NOTIFY esp6s0IPChanged)
    Q_PROPERTY(QVector<QString> esp8s0IP READ esp8s0IP WRITE setEsp8s0IP NOTIFY esp8s0IPChanged)
    Q_PROPERTY(QVector<QString> esp6s0Serial READ esp6s0Serial WRITE setEsp6s0Serial NOTIFY esp6s0SerialChanged)
    Q_PROPERTY(QVector<QString> esp8s0Serial READ esp8s0Serial WRITE setEsp8s0Serial NOTIFY esp8s0SerialChanged)





public:
    explicit CameraController(QObject *parent = nullptr);
    ~CameraController();

    setup mySetup;
    int com_type = communication_type::UDP; //MUST

    const QVector<QString> &serials() const;
    void setSerials(const QVector<QString> &newSerials);

    bool analyseStatue() const;
    void setAnalyseStatue(bool newAnalyseStatue);
    bool m_analyseStatue{false};

    bool readyToProcess() const;
    void setReadyToProcess(bool newReadyToProcess);
    bool m_readyToProcess{false};

    bool saveResult() const;
    void setSaveResult(bool newSaveResult);
    bool m_saveResult{false};

    const QVector<QString> &classNamesList() const;
    void setClassNamesList(const QVector<QString> &newClassNamesList);
//    QVector<QString> m_classNamesList = {"Sib","Mis_shape","Naghes","Sar","Tah","Zakhme_khoshk","Khale_siyah","Laki","Negini"};
//    QVector<QString> m_classNamesList = {"Apple","Mis_shape","Nubbin","Top","Bottom","Blotch","Scab","Bruising","Cut"}; //MUST
    QVector<QString> m_classNamesList = {"Tah","Sar","Shepesh","Kane","Choruk","Twin","Pahn","Salem","Genetic_big"};


    // Enabled Classes
    QVector<bool> classEnableList() const;
    void setClassEnableList(QVector<bool> newClassEnableList);
    QVector<bool> m_classEnableList = {false,false,true,true,true,true,true,false,true}; //MUST

    // Enabled Classes setter, getter
    const QVector<int> &classConfList() const;
    void setClassConfList(const QVector<int> &newClassConfList);
    QVector<int> m_classConfList;

    // Defineing if defect counting be enabled, at all
    bool quantityEnable() const;
    void setQuantityEnable(bool newquantityEnable);
    bool m_quantityEnable{false};

    // Defineing for which classes quantity based counting be considered
    const QVector<bool> &classQuantityEnabledList() const;
    void setclassQuantityEnabledList(const QVector<bool> &newclassQuantityEnabledList);
    QVector<bool> m_classQuantityEnabledList = {false,false,true,true,false,false,false,false,false}; //MUST


    QVector<int> m_quantityBasedClassCountList;
    const QVector<int> &quantityBasedClassCountList() const;
    void setQuantityBasedClassCountList(const QVector<int> &newQuantityBasedClassCountList);

    // this values will not be accessible from ui
    QVector<float> m_quantityBasedClassConfList;
    const QVector<float> &quantityBasedClassConfList() const;
    void setQuantityBasedClassConfList(const QVector<float> &newQuantityBasedClassConfList);

    QString esp6s0GW() const;
    void setEsp6s0GW(QString newEsp6s0GW);
    QString m_esp6s0GW;

    QString esp8s0GW() const;
    void setEsp8s0GW(QString newEsp8s0GW);
    QString m_esp8s0GW;

    QVector<QString> esp6s0IP() const;
    void setEsp6s0IP(QVector<QString> newEsp6s0IP);
    QVector<QString> m_esp6s0IP;

    QVector<QString> esp8s0IP() const;
    void setEsp8s0IP(QVector<QString> newEsp8s0IP);
    QVector<QString> m_esp8s0IP;

    QVector<QString> esp6s0Serial() const;
    void setEsp6s0Serial(QVector<QString> newEsp6s0Serial);
    QVector<QString> m_esp6s0Serial;

    QVector<QString> esp8s0Serial() const;
    void setEsp8s0Serial(QVector<QString> newEsp8s0Serial);
    QVector<QString> m_esp8s0Serial;

    const QVector<int> &classesCount() const;
    void setClassesCount(const QVector<int> &newClassesCount);
    QVector<int> m_classesCount = {0,0,0,0,0,0,0,0,0};

    int ejectorPosition() const;
    void setEjectorPosition(int newEjectorPosition);
    int m_ejectorPosition;

    int getEncoder() const;
    void setEncoder(int newEncoder);
    int m_encoder = 0;


    int energize() const;
    void setEnergize(int newEnergize);
    int m_energize;

    int deEnergize() const;
    void setDeEnergize(int newDeEnergize);
    int m_deEnergize;

    QVector<double> red() const;
    void setRed(QVector<double> newRed);
    QVector<double> m_red;

    QVector<double> green() const;
    void setGreen(QVector<double> newGreen);
    QVector<double> m_green;

    QVector<double> blue() const;
    void setBlue(QVector<double> newBlue);
    QVector<double> m_blue;

    QVector<double> gamma() const;
    void setGamma(QVector<double> newGamma);
    QVector<double> m_gamma;

    QVector<double> exposure() const;
    void setExposure(QVector<double> newExposure);
    QVector<double> m_exposure;

    QVector<double> gain() const;
    void setGain(QVector<double> newGain);
    QVector<double> m_gain;



    int batchID() const;
    void setBatchID(int newBatchID);
    void stepBatchID();
    int m_batchID{-1};


    int total() const;
    void setTotal(int newtotal);
    void increaseTotalFruits();
    int m_total{0};

    QString elapsedTime() const;
    void setElapsedTime(QString newElapsedTime);
    QString m_elapsedTime{"00:00:00"};

    bool ardStatus() const;
    void setArdStatus(bool newArdStatus);
    bool m_ardStatus{false};

    //E
    bool ethStatus() const;
    void setEthStatus(bool newEthStatus);
    bool m_ethStatus{false};

    bool cameraStatus(QString Serial);
    QString calcEjectorVectorRowString(QVector<int> _ejectorQueueRow);
    QTime Start_time;
    void calElapsedTime();

    GError* err = NULL;
    GstElement* pipeline;
    GstElement* source;
    GstElement* pipeline_swtrg;
    QVector<GstElement*> source_list;
    QVector<GstElement*> pipeline_swtrg_list;

    static GstPadProbeReturn handle_new_sample(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);

    void captureImage(GstElement *source);

    void set_Camera_Ethernet_Setting();

    void modelWarmUp();


    QVector<cv::Mat> separateMiddle(cv::Mat &frame);
    QVector<cv::Mat> separateLeftAndRigth(cv::Mat &frame);

public Q_SLOTS:

    void mainLoop();
    void readDataSerial_thread();
    void resetSettingFile();
    void setRed_idx(double newRed, int idx);
    void setGreen_idx(double newGreen, int idx);
    void setBlue_idx(double newBlue, int idx);
    void setGamma_idx(double newGamma, int idx);
    void setExposure_idx(double newExposure, int idx);
    void setGain_idx(double newGain, int idx);
    void startAnalise();
    void stopAnalise();
    void setClassesConf(int _id, int _conf);
    void setClassesEnabled(int _id, bool _enb);
    void readDataEthernet();
    void openSerialPort();
    void closeSerialPort();
    void setclassQuantity(int _id, int _qnt);
    void setclassEnabledQuantity(int _id, bool _qntEnable);
    void timerFunction();

private:


    bool grabFirst = false;
    bool grabSecond = false;
    bool retrieveFirst = false;
    bool retrieveSecond = false;
    bool restartFlag = false;


    QSerialPort *serial; ///////////////////////// be created only if
    QVector <gsttcam::TcamCamera> tcamList;
    Processor _processor;
    ArduinoController _arduCtrlr; // delete
    Ethernet _ethCtrl; //delete
    QUdpSocket *udpSocketSend;
    QUdpSocket *udpSocketReceive;
    QTimer *timer = new QTimer(this);


    QFuture<std::vector<std::vector<Detection>>> _inferenceResult;
    QString _rootPath;
    QString _settingPath;
    QList<camPack> _camPackList;
    QVector<QVector<int>> _prevbatchRes;
    QVector<QVector<int>> _ejectorQueue;



    QVector<QVector<int>> _NumberOfDefectsLine1;
    QVector<QVector<int>> _NumberOfDefectsLine2;
    QVector<QVector<int>> _NumberOfDefectsInEachImageOfBatchPrev;
    QVector<QVector<int>> lineClassValidityTotalPrev;



    QMutex ejectorLocker;
    std::vector<cv::Mat> _framesBatch;


    void preparingTheCameras();
    void releaseTheCameras();
    void increaseClassesCountByID(int _id);
    void postProcessing(std::vector<std::vector<Detection>> _detList);

    QVector<QVector<int>> calcEjectorVectorRow(std::vector<std::vector<Detection>> _detList);

    QStringList serialPortCheck();


    void writeDataSerial(QString ejectorText);
    void writeDataEthernet(QString ejectorText);

Q_SIGNALS:
    void serialsChanged();
    void saveResultChanged();
    void classNamesListChanged();
    void classConfListChanged();
    void classEnableListChanged();
    void classNumberEnableListChanged();
    void ejectorPositionChanged();
    void shiftChanged();
    void energizeChanged();
    void deEnergizeChanged();
    void redChanged();
    void greenChanged();
    void blueChanged();
    void gammaChanged();
    void exposureChanged();
    void gainChanged();
    void analyseStatueChanged();
    void classesCountChanged();
    void readyToProcessChanged();
    void batchIDChanged();
    void ardStatusChanged();
    void ethStatusChanged();
    void elapsedTimeChanged();
    void quantityEnableChanged(); //exist
    void classQuantityEnabledListChanged(); // which class enabled
    void quantityBasedClassCountListChanged(); // changing quantity of each class
    void quantityBasedClassConfListChanged();
    void encoderChanged();
    void totalChanged();
    void esp6s0GWChanged();
    void esp8s0GWChanged();
    void esp6s0IPChanged();
    void esp8s0IPChanged();
    void esp6s0SerialChanged();
    void esp8s0SerialChanged();

};

#endif // CAMERACONTROLLER_H

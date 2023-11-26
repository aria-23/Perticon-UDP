#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <controller.h>
#include <QtNetwork>
#include <QIcon>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    //    app.setWindowIcon(QIcon(":/Icons/outline_settings_black_48pt_3x.png"));
    app.setWindowIcon(QIcon(":/Icons/Logo.png"));


    QQuickStyle::setStyle("material");
    qmlRegisterType<Controller>("app.Controller", 1, 0, "Controller");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}

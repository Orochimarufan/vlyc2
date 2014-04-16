#ifndef TESTPLUGIN2_H
#define TESTPLUGIN2_H

#include <QObject>

#include <VlycPlugin.h>
#include <VlycUrlHandlerPlugin.h>

class TestPlugin2 : public QObject, public Vlyc::UrlHandlerPlugin
{
    Q_OBJECT
    Q_INTERFACES(Vlyc::Plugin)
    Q_PLUGIN_METADATA(IID "me.sodimm.oro.vlyc.Plugin/2.0" FILE "plugin.json")
public:
    TestPlugin2();

    QString id() const { return "me.sodimm.oro.vlyc.plugins.Test"; }

    void init(Vlyc::InitEvent &init);

    Vlyc::Result::ResultPtr handleUrl(const QUrl &url);
};

#endif // TESTPLUGIN2_H

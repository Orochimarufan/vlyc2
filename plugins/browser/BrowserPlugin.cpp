#include "BrowserPlugin.h"

#include "browser.h"

#include <QtWidgets/QAction>

#include "../../src/vlyc.h"

BrowserPlugin::BrowserPlugin()
{
}

void BrowserPlugin::init(Vlyc::InitEvent &init)
{
    vlyc = (VlycApp*)init.private_interface;
}

QAction *BrowserPlugin::toolMenuAction()
{
    QAction *browser = new QAction(tr("Browser"), this);
    connect(browser, &QAction::triggered, [=] () { newWindow(true); });
    return browser;
}

bool BrowserPlugin::navigationRequest(QUrl url)
{
    return ! vlyc->tryPlayUrl(url);
}

QNetworkAccessManager *BrowserPlugin::network()
{
    return vlyc->network();
}

#include "BrowserPlugin.h"

#include "browser.h"
#include "webview.h"
#include "LinkContextMenu.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>

#include <VlycResult/Result.h>

#include "../../src/vlyc.h"
#include "../../src/logic/__lv_hacks.h"

using namespace Vlyc::Result;

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
    ResultPtr result = vlyc->handleUrl(url);
    if (result.isValid())
        if (result.is<LegacyVideoResult>())
        {
            vlyc->playResult(result);
            return false;
        }
    return true;
}

QNetworkAccessManager *BrowserPlugin::network()
{
    return vlyc->network();
}

void BrowserPlugin::linkContextMenu(LinkContextMenu *menu)
{
    menu->addAction(tr("Play"), [this,menu](){
        ResultPtr result = vlyc->handleUrl(menu->url());
        if (result.isValid())
            vlyc->playResult(result);
        else
            QMessageBox::critical(menu->tab(), "Error",
                QStringLiteral("VLYC² cannot play url:\n%1").arg(menu->url().toString()));
    });
    menu->addAction(tr("Queue"), [this,menu]() {
        ResultPtr result = vlyc->handleUrl(menu->url());
        if (result.isValid())
            vlyc->queueResult(result);
        else
            QMessageBox::warning(menu->tab(), "Error",
                QStringLiteral("VLYC² cannot play url:\n%1").arg(menu->url().toString()));
    });
    menu->addSeparator();
    Browser::linkContextMenu(menu);
}

#include "GDPSHubLayer.hpp"
#include "PrivateServerNode.hpp"
#include "../utils/Structs.hpp"

#include <Geode/utils/cocos.hpp>

std::string current;

bool GDPSHubLayer::init()
{
    if (!CCLayer::init())
        return false;

    this->setID("GDPSHubLayer");

    this->setKeypadEnabled(true);

    auto background = createLayerBG();
    background->setID("background");
    this->addChild(background);

    auto winSize = CCDirector::get()->getWinSize();

    auto title = CCLabelBMFont::create("Private Server Switcher", "goldFont.fnt");
    title->setPosition(winSize.width / 2, winSize.height - 22.5f);
    title->setID("title-text");
    this->addChild(title);

    auto menu = CCMenu::create();
    menu->setID("menu");
    menu->setZOrder(10);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this, menu_selector(GDPSHubLayer::onGoBack));
    backBtn->setPosition(-winSize.width / 2 + 25.f, winSize.height / 2 - 25.f);
    backBtn->setID("back-button");
    menu->addChild(backBtn);
    this->addChild(menu);

    auto scrollBg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setOpacity(90);
    scrollBg->setContentSize({winSize.width - 85, winSize.height - 100});
    scrollBg->setPosition(winSize / 2);
    scrollBg->ignoreAnchorPointForPosition(false);
    scrollBg->setID("server-scroll-bg");
    this->addChild(scrollBg);

    scroll = ScrollLayer::create({winSize.width - 85, winSize.height - 100});
    scroll->setPosition((winSize - scroll->getContentSize()) / 2);
    scroll->setID("server-scroll");
    this->addChild(scroll);

    updateList();

    return true;
}

void GDPSHubLayer::keyBackClicked()
{
    this->onGoBack(nullptr);
}

void GDPSHubLayer::onGoBack(CCObject *)
{
    CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, MenuLayer::scene(false)));
}

GDPSHubLayer *GDPSHubLayer::create()
{
    auto ret = new GDPSHubLayer;
    if (ret->init())
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void GDPSHubLayer::updateList() {
    m_loadingCircle->setParentLayer(this);
    m_loadingCircle->setPosition({ -70,-40 });
    m_loadingCircle->setScale(1.f);
    m_loadingCircle->show();

    m_listener.bind([this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            auto resp = res->json().unwrapOr(matjson::Object{});
            log::info("{}", resp);
            m_loadingCircle->fadeAndRemove();
        }
    });

    auto req = web::WebRequest();
    m_listener.setFilter(req.get("https://api.gdpshub.com/gdps/geode"));
}
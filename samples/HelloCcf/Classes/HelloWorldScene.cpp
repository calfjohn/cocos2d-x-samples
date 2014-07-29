#include "HelloWorldScene.h"
#include "PopLayer.h"
#include "ChatScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);
    
    layer->setTag(10086);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    _menuPeer = Menu::create();
    _menuPeer->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    this->addChild(_menuPeer, 1);
    
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    auto peerListener = EventListenerCustom::create("IntelCCFPeerUpdate", CC_CALLBACK_1(HelloWorld::listenToPeerUpdate, this));
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(peerListener, this);
    
    auto peerInviteListener = EventListenerCustom::create("IntelCCFPeerInvite", CC_CALLBACK_1(HelloWorld::listenToPeerInvite, this));
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(peerInviteListener, this);
    
    auto peerAcknowledgmentListener = EventListenerCustom::create("IntelCCFInviteAcknowledgment", CC_CALLBACK_1(HelloWorld::listenToAcknowledgment, this));
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(peerAcknowledgmentListener, this);
    
    schedulePeer(0);
    
    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif
    
    Director::getInstance()->end();
    ConnectionInterface::Disconnect();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::listenToPeerUpdate(EventCustom *event)
{
    this->schedule(schedule_selector(HelloWorld::schedulePeer));
}

void HelloWorld::schedulePeer(float dt)
{
    std::list<tagPEER> listPeer;
    ConnectionInterface::getPeerList(listPeer);
    
    if (listPeer.size() == 0) return;
    
    CCLOG("receive Discovery");
    while(listPeer.size()){
        tagPEER tempPeer = listPeer.front();
        
        PeerMenuItem *pItem = queryPeerMenuItem(tempPeer);
        if (tempPeer.add && pItem == NULL) {
            auto peerItem = PeerMenuItem::create(
                                                 "CloseNormal.png",
                                                 "CloseSelected.png",
                                                 CC_CALLBACK_1(HelloWorld::menuPeerCallback, this));
            peerItem->setPeer(tempPeer);
            peerItem->setTag(10011);
            peerItem->setPosition(Vec2::ZERO);
            _menuPeer->addChild(peerItem);
        }
        
        if(tempPeer.add && pItem != NULL){
            pItem->removeFromParent();
        }
        
        listPeer.pop_front();
    }

    _menuPeer->alignItemsHorizontally();
}

void HelloWorld::listenToPeerInvite(EventCustom *event)
{
    this->schedule(schedule_selector(HelloWorld::schedulePop));
}

void HelloWorld::schedulePop(float dt)
{
    auto popLayer = PopLayer::create();
    addChild(popLayer, 1);
}

void HelloWorld::listenToAcknowledgment(cocos2d::EventCustom *event)
{
    this->schedule(schedule_selector(HelloWorld::scheduleAcknowledgement));
}

void HelloWorld::scheduleAcknowledgement(float dt)
{
    auto scene = ChatLayer::createScene();
    Director::getInstance()->replaceScene(scene);
}

void HelloWorld::menuPeerCallback(Ref* pSender)
{
    PeerMenuItem* pItem = (PeerMenuItem*) pSender;
    CCLOG("sessionId:%s", pItem->getPeer()._sessionId.c_str());
    
    ConnectionInterface::InvitePeer(pItem->getPeer());
}

PeerMenuItem* HelloWorld::queryPeerMenuItem(tagPEER peer)
{
    for (auto& child : _menuPeer->getChildren())
    {
        if(child && child->getTag() == 10011)
        {
            std::list<tagPEER>::iterator iter;
            PeerMenuItem *pNode = (PeerMenuItem *)child;
            if (pNode->getPeer()._sessionId == peer._sessionId) {
                return pNode;
            }
        }
    }
    
    return NULL;
}
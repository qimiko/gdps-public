#pragma once

#ifndef CLASSES_CALLBACKS_SHOWLOADINGCIRCLECALLBACK_HPP
#define CLASSES_CALLBACKS_SHOWLOADINGCIRCLECALLBACK_HPP

#include <Geode/Geode.hpp>

class ShowLoadingCircleCallback : public cocos2d::CCNode {
private:
    static LoadingCircle* loadingCircle_;

public:
    void onCircle(cocos2d::CCNode*);
    void onRemoveCircle(cocos2d::CCNode*);

    CREATE_FUNC(ShowLoadingCircleCallback)

    static void showLoadingCircle();
    static void hideLoadingCircle();
};

#endif //CLASSES_CALLBACKS_SHOWLOADINGCIRCLECALLBACK_HPP

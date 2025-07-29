#pragma once

#ifndef CLASSES_EXTENSIONS_GJSEARCHOBJECTEXT_HPP
#define CLASSES_EXTENSIONS_GJSEARCHOBJECTEXT_HPP

#include <Geode/Geode.hpp>

class GJSearchObjectExt : public cocos2d::CCNode {
private:
    bool super_ = false;

public:
    bool getSuper() const { return this->super_; };
    void setSuper(bool n_super) { this->super_ = n_super; }

    CREATE_FUNC(GJSearchObjectExt); // NOLINT(modernize-use-auto)
};

#endif //CLASSES_EXTENSIONS_GJSEARCHOBJECTEXT_HPP

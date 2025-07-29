#pragma once

#ifndef CLASSES_DELEGATES_SESSIONDELEGATE_HPP
#define CLASSES_DELEGATES_SESSIONDELEGATE_HPP

class SessionDelegate {
public:
    virtual void onSessionCreated() {};
    virtual void onSessionFailed(const char* reason) {};
};

#endif //CLASSES_DELEGATES_SESSIONDELEGATE_HPP

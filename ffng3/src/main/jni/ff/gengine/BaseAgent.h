#ifndef HEADER_BASEAGENT_H
#define HEADER_BASEAGENT_H

#include "common.h"
#include "NoCopy.h"
#include "BaseListener.h"
#include "ExInfo.h"
#include "AgentPack.h"

/**
 * Ancestor for all agents.
 */
class BaseAgent : public NoCopy, public BaseListener {
    private:
        bool m_initialized;
    protected:
        virtual void own_init() {}
        virtual void own_update() {}
        virtual void own_shutdown() {}
    public:
        BaseAgent();
        virtual ~BaseAgent() {}
        bool isInitialized() { return m_initialized; }

        void init();
        void update();
        void shutdown();
};

/**
 * Example: AGENT(VideoAgent, Name::VIDEO_NAME)
 *
 * Enables to obtain typed pointer
 * static VideoAgent *VideoAgent::agent();
 */
#define AGENT(TYPE, NAME) \
public: \
virtual const char *getName() const { return (NAME); } \
static TYPE *agent() \
{ \
    TYPE *result = dynamic_cast<TYPE *>(AgentPack::getAgent(sName())); \
    if (NULL == result) { \
        throw std::logic_error("cannot cast agent:"s + sName()); \
    } \
    return result; \
} \
private: \
static const char *sName() { return (NAME); }

#endif

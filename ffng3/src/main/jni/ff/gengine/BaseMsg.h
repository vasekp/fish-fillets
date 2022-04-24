#ifndef HEADER_BASEMSG_H
#define HEADER_BASEMSG_H

#include <string>

class BaseListener;

/**
 * Messgage.
 */
class BaseMsg {
    protected:
        std::string m_listenerName;
        std::string m_name;
    public:
        BaseMsg(const std::string &listenerName, const std::string &name);
        virtual ~BaseMsg() {}

    virtual void sendActual(BaseListener *listener) const = 0;

        void send() const;
        bool equalsName(const std::string &name) const
        { return m_name == name; }
        const std::string &getMsgName() const { return m_name; }
        const std::string &getListenerName() const { return m_listenerName; }

        virtual std::string toString() const;
};

#endif

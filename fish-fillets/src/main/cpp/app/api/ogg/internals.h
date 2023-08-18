#ifndef FISH_FILLETS_OGG_INTERNALS_H
#define FISH_FILLETS_OGG_INTERNALS_H

#include "common.h"

#include <vorbis/codec.h>
#include <theora/theoradec.h>

namespace ogg::internal {
    template<class C, auto init, auto clear>
    class OggStruct : public C {
    public:
        template<typename... Ts>
        OggStruct(Ts... ts) {
            if constexpr(init != nullptr)
                init(this, ts...);
        }

        OggStruct(C&& nativeObj) {
            std::memmove(this, &nativeObj, sizeof(C));
        }

        OggStruct(const OggStruct&) = delete;

        OggStruct& operator=(const OggStruct&) = delete;

        ~OggStruct() {
            if constexpr(clear != nullptr)
                clear(this);
        }
    };

    template<class C, auto free>
    class OggPointer {
    public:
        OggPointer() : m_native(nullptr) { }

        OggPointer(const OggPointer&) = delete;

        OggPointer& operator=(const OggPointer&) = delete;

        ~OggPointer() {
            if(m_native)
                free(m_native);
        }

        void set(C* ptr) {
            assert(!m_native);
            m_native = ptr;
        }

        C* native() { return m_native; }
        C** operator&() { return &m_native; }
        operator C*() { return native(); }

    private:
        C* m_native;
    };
}

#endif //FISH_FILLETS_OGG_INTERNALS_H

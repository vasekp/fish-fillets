#ifndef FISH_FILLETS_OGL_UTIL_H
#define FISH_FILLETS_OGL_UTIL_H

namespace ogl::util {

    template<typename T, T nullValue>
    struct ResetOnMove {
        T value;

        ResetOnMove(T x = nullValue) : value(x) { }

        ResetOnMove(ResetOnMove&& other) : value(other.value) {
            other.value = nullValue;
        }

        ResetOnMove& operator=(ResetOnMove&& other) {
            std::swap(value, other.value);
            return *this;
        }

        ResetOnMove& operator=(T newValue) {
            value = newValue;
            return *this;
        }

        operator T() const { return value; }

        T* operator&() { return &value; }
    };

}

#endif //FISH_FILLETS_OGL_UTIL_H

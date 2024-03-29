#ifndef FISH_FILLETS_ENUMBITSET_H
#define FISH_FILLETS_ENUMBITSET_H

#include <bitset>
#include <utility>

namespace util {

    template<typename Enum>
    class EnumBitset : private std::bitset<(std::size_t)Enum::SIZE> {
        using Base = std::bitset<(std::size_t)Enum::SIZE>;

    public:
        using Base::any;
        using Base::all;
        using Base::none;
        using Base::reset;
        using Base::operator~;

        EnumBitset() { }

        void set(Enum element, bool value = true) { Base::set((std::size_t)element, value); }
        void reset(Enum element) { Base::reset((std::size_t)element); }
        bool test(Enum element) const { return Base::test((std::size_t)element); }

        friend bool operator==(const EnumBitset& a, const EnumBitset& b) { return a.base() == b.base(); }
        friend bool operator!=(const EnumBitset& a, const EnumBitset& b) { return a.base() != b.base(); }

        friend EnumBitset& operator&(const EnumBitset& a, const EnumBitset& b) { return {a.base() & b.base()}; }
        friend EnumBitset& operator|(const EnumBitset& a, const EnumBitset& b) { return {a.base() | b.base()}; }
        friend EnumBitset& operator^(const EnumBitset& a, const EnumBitset& b) { return {a.base() ^ b.base()}; }
        EnumBitset& operator&=(const EnumBitset& other) { Base::operator&=(other.base()); }
        EnumBitset& operator|=(const EnumBitset& other) { Base::operator|=(other.base()); }
        EnumBitset& operator^=(const EnumBitset& other) { Base::operator^=(other.base()); }

        auto operator[](Enum element) { return Base::operator[]((std::size_t)element); }
        bool operator[](Enum element) const { return Base::operator[]((std::size_t)element); }

    private:
        EnumBitset(Base b) : Base(std::move(b)) { }
        const Base& base() const { return static_cast<const Base&>(*this); }
    };

}

#endif //FISH_FILLETS_ENUMBITSET_H

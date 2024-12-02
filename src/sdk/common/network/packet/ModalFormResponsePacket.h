#pragma once
#include "../Packet.h"
#include "sdk/String.h"

namespace SDK {

    class ModalFormResponsePacket : public Packet {
    public:
        class Value {
        public:
            enum class Type : int {
                Null = 0x0,
                Int = 0x1,
                Uint = 0x2,
                Real = 0x3,
                String = 0x4,
                Boolean = 0x5,
                Array = 0x6,
                Object = 0x7,
            };

            class CZString {
            public:
                enum class Policy : unsigned int {
                    noDuplication = 0,
                    duplicate = 1,
                    duplicateOnCopy = 2,
                    Mask = 3,
                };

                char const* cstr_;
                union {
                    unsigned int index_;
                    struct {
                        Policy       policy_ : 2;
                        unsigned int length_ : 30;
                    } storage_;
                };
            };

            union {
                int64_t                    int_;
                uint64_t                   uint_;
                double                     real_;
                bool                       bool_;
                char*                      string_;
                std::map<CZString, Value>* map_;
            } value_;
   
            struct {
                Type type_      : 8;
                bool allocated_ : 1;
            } bits_;
        };

        enum class Reason : signed char {
            UserClosed = 0x0,
            UserBusy = 0x1,
        };

        unsigned int                Id;
        std::optional<Value>        Json;
        std::optional<Reason>       CancelReason;

    };

}
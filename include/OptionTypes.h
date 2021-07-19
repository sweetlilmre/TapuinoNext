#pragma once
#include <inttypes.h>
#include <string.h>

namespace TapuinoNext
{
    enum class ConfigOptionType : uint8_t
    {
        ConfigValue,
        ConfigToggle,
        ConfigEnum
    };

    enum class OptionTagId : uint8_t
    {
        ButtonClickTime = 0,
        ButtonHoldTime,
        TickerTime,
        TickerHoldTime,
        IsNTSC,
        AutoPlay,
        Backlight,
        Machine,
        LAST
    };

    class IOptionType
    {
      public:
        virtual ConfigOptionType GetType() = 0;
        virtual OptionTagId GetTag() = 0;
    };

    class IChangeNotify
    {
      public:
        virtual void Notify(OptionTagId tag, IOptionType* optionType) = 0;

      private:
    };

    template <class T> class OptionTemplate : public IOptionType
    {
      public:
        OptionTemplate(OptionTagId tag, IChangeNotify* notify) : tag(tag), notify(notify)
        {
        }

        virtual ~OptionTemplate(){};

        virtual T GetValue()
        {
            return value;
        }

        virtual T SetValue(T newValue)
        {
            value = newValue;
            return value;
        }

        OptionTagId GetTag()
        {
            return (tag);
        }

        bool IsDirty()
        {
            return (origValue != value);
        }

        void Commit()
        {
            origValue = value;
            if (notify != NULL)
            {
                notify->Notify(tag, this);
            }
        }

        void Revert()
        {
            value = origValue;
        }

      protected:
        OptionTagId tag;
        IChangeNotify* notify;
        T value;
        T origValue;
    };

    class ValueOption : public OptionTemplate<uint32_t>
    {
      public:
        ValueOption(OptionTagId tag, IChangeNotify* notify, uint32_t initialValue, uint32_t minValue, uint32_t maxValue, uint32_t stepValue) : OptionTemplate<uint32_t>(tag, notify)
        {
            min = minValue;
            max = maxValue;
            step = stepValue;

            // might as well make sure that the clamping logic is applied on constuction
            SetValue(initialValue);
            origValue = value;
        }

        uint32_t SetValue(uint32_t newValue)
        {
            if (newValue >= min && newValue <= max)
            {
                value = newValue;
            }
            else
            {
                value = min;
            }
            return value;
        }

        uint32_t IncValue()
        {
            // overflow check
            if (max - step >= value)
            {
                value += step;
            }
            return value;
        }

        uint32_t DecValue()
        {
            // underflow check
            if (min + step <= value)
            {
                value -= step;
            }
            return value;
        }

        ConfigOptionType GetType()
        {
            return (ConfigOptionType::ConfigValue);
        }

      private:
        uint32_t min;
        uint32_t max;
        uint32_t step;
    };

    class ToggleOption : public OptionTemplate<bool>
    {
      public:
        ToggleOption(OptionTagId tag, IChangeNotify* notify, bool initialValue, const char* falseText, const char* trueText) : OptionTemplate<bool>(tag, notify)
        {
            origValue = value = initialValue;
            this->falseText = falseText;
            this->trueText = trueText;
        }

        const char* GetToggleText()
        {
            return value ? trueText : falseText;
        }

        void Toggle()
        {
            value = !value;
        }

        ConfigOptionType GetType()
        {
            return (ConfigOptionType::ConfigToggle);
        }

      private:
        const char* trueText;
        const char* falseText;
    };

    class EnumOption : public OptionTemplate<uint8_t>
    {
      public:
        EnumOption(OptionTagId tag, IChangeNotify* notify, const char** names, uint8_t length, uint8_t index) : OptionTemplate<uint8_t>(tag, notify), names(names), length(length)
        {
            origValue = value = 0;
            if (index < length)
                origValue = value = index;
        }

        ~EnumOption()
        {
        }

        void ParseValue(const char* textValue)
        {
            for (uint8_t i = 0; i < length; i++)
            {
                if (0 == strcmp(names[i], textValue))
                {
                    value = i;
                    break;
                }
            }
        }

        uint8_t Next()
        {
            if (value < (length - 1))
            {
                value++;
            }
            return value;
        }

        uint8_t Prev()
        {
            if (value != 0)
            {
                value--;
            }
            return value;
        }

        virtual const char* GetEnumText()
        {
            return names[value];
        }

        virtual ConfigOptionType GetType()
        {
            return (ConfigOptionType::ConfigEnum);
        }

      private:
        const char** names;
        uint8_t length;
    };
} // namespace TapuinoNext

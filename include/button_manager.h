#pragma once

#include <Arduino.h>

enum class ButtonId : uint8_t { Menu, Up, Down, Ok, Turn };
enum class ButtonEventType : uint8_t { ShortPress, LongPress };

struct ButtonEvent {
  ButtonId button;
  ButtonEventType type;
};

class ButtonManager {
 public:
  void begin();
  void update(uint32_t now);
  bool nextEvent(ButtonEvent& event);

 private:
  struct ButtonState {
    uint8_t pin;
    ButtonId id;
    bool rawPressed;
    bool stablePressed;
    bool longReported;
    uint32_t rawChangedAt;
    uint32_t pressedAt;
  };
  static constexpr uint8_t BUTTON_COUNT = 5;
  static constexpr uint8_t QUEUE_SIZE = 10;
  ButtonState buttons_[BUTTON_COUNT];
  ButtonEvent events_[QUEUE_SIZE];
  uint8_t queueHead_ = 0;
  uint8_t queueTail_ = 0;
  void enqueue(ButtonId button, ButtonEventType type);
};

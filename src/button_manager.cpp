#include "button_manager.h"

#include "config.h"

void ButtonManager::begin() {
  const uint8_t pins[BUTTON_COUNT] = {Config::MENU_BUTTON_PIN, Config::UP_BUTTON_PIN,
                                      Config::DOWN_BUTTON_PIN, Config::OK_BUTTON_PIN,
                                      Config::TURN_BUTTON_PIN};
  const ButtonId ids[BUTTON_COUNT] = {ButtonId::Menu, ButtonId::Up, ButtonId::Down,
                                      ButtonId::Ok, ButtonId::Turn};
  for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
    pinMode(pins[i], INPUT_PULLUP);
    const bool pressed = digitalRead(pins[i]) == LOW;
    buttons_[i] = {pins[i], ids[i], pressed, pressed, false, millis(), millis()};
  }
  Serial.println(F("[buttons] five pull-up inputs initialized"));
}

void ButtonManager::enqueue(ButtonId button, ButtonEventType type) {
  const uint8_t next = static_cast<uint8_t>((queueHead_ + 1U) % QUEUE_SIZE);
  if (next == queueTail_) {
    Serial.println(F("[buttons] WARNING: event queue full; event dropped"));
    return;
  }
  events_[queueHead_] = {button, type};
  queueHead_ = next;
}

void ButtonManager::update(uint32_t now) {
  for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
    ButtonState& button = buttons_[i];
    const bool pressed = digitalRead(button.pin) == LOW;
    if (pressed != button.rawPressed) {
      button.rawPressed = pressed;
      button.rawChangedAt = now;
    }

    if (pressed != button.stablePressed &&
        static_cast<uint32_t>(now - button.rawChangedAt) >= Config::BUTTON_DEBOUNCE_MS) {
      button.stablePressed = pressed;
      if (pressed) {
        button.pressedAt = now;
        button.longReported = false;
      } else if (!button.longReported) {
        enqueue(button.id, ButtonEventType::ShortPress);
      }
    }

    if (button.stablePressed && !button.longReported &&
        static_cast<uint32_t>(now - button.pressedAt) >= Config::BUTTON_LONG_PRESS_MS) {
      button.longReported = true;
      if (button.id == ButtonId::Menu) {
        enqueue(button.id, ButtonEventType::LongPress);
      }
    }
  }
}

bool ButtonManager::nextEvent(ButtonEvent& event) {
  if (queueTail_ == queueHead_) return false;
  event = events_[queueTail_];
  queueTail_ = static_cast<uint8_t>((queueTail_ + 1U) % QUEUE_SIZE);
  return true;
}

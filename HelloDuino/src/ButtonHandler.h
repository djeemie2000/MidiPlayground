#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

class CButtonHandler
{
public:
  CButtonHandler();

  void Update(bool ButtonPressed);

  bool ButtonClicked() const;

  bool ButtonHold() const;

private:
  const int m_HoldThreshold;

  enum EButtonState
  {
    Neutral,
    Clicked,
    Hold
  };

  bool m_PrevButtonPressed;
  int m_Counter;
  EButtonState m_ButtonState;
};

#endif // BUTTONHANDLER_H

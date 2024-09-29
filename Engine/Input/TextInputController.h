#ifndef TEXTINPUTCONTROLLER_H
#define TEXTINPUTCONTROLLER_H

#include <SFML/Graphics.hpp>
#include <string>

class TextInputController
{
public:
    TextInputController();
    void latchOn(sf::String& targetString);
    void release();
    void parseEvents(const sf::Event& event);

private:
    sf::String* m_targetString = nullptr;  // Pointer to hold the target string reference
};

#endif // TextINPUTCONTROLLER_H

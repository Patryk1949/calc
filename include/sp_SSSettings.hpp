#pragma once

#include "Utils.hpp"
#include <SFML/Graphics.hpp>
#include <type_traits>



namespace sp{	// BEGINNING OF NAMESPACE ///////////////////////////////////////////////////////////////////////

enum class SettingsParamType : uchar{
	Null, Bool, Int, Float, Word
};



template<int maxParamNumber, int maxVarStringLen = 16>
class SimpleSettingsList{
public:
// Constructors
	SimpleSettingsList(const sf::Font &font, const sf::Vector2f position = sf::Vector2f{0.f, 0.f}, sf::RenderTarget *const target = nullptr) noexcept;

// Addind, Removing Parameters
	void pushParameter(const char *const parameterName, const SettingsParamType parameterType, void *const parameter);

	void replaceParameter(const char *const parameterName, const SettingsParamType parameterType, void *const parameter);

	void popParameter(const int number = 1);

	inline void clearParameters();

//	Setters
	inline void setWindow(sf::RenderTarget *const target);

	inline void setPosition(const sf::Vector2f &position);

	inline void setTextSize(const uint size);

	inline void setTextColor(const sf::Color color);

	inline void setFont(const sf::Font &font);

	char *setValueFromString(const int parameterNumber, const char *const inputString);

// Getters
	inline float getCharacterSpacing() const;

	inline uint getCharacterSize() const;

	inline sf::Vector2f getVariableTextPosition(const int index = 0) const;

	inline float getVariableTextPositionX() const;

	inline float getVariableTextPositionY(const int index = 0) const;

	inline sf::Color getTextColor();

	inline int positionToIndex(const sf::Vector2f position);

	char *getStringOfValue(const int parameterNumber);

// Drawing
	void drawTo();

	void drawTo(sf::RenderTarget *const target);

private:
	sf::Text namesText;
	sf::Text varText;

	char paramNames[maxParamNumber*16];
	SettingsParamType paramType[maxParamNumber];
	void *paramValue[maxParamNumber];

	char varString[maxVarStringLen];

	float textSpace;
	int paramNumber;
	sf::RenderTarget *window;
};



template<int maxLength>
class TypingLine{
public:
// Constructors
	TypingLine(const sf::Font &font, const sf::Vector2f position = sf::Vector2f{0.f, 0.f}, sf::RenderTarget *const target = nullptr) noexcept;

// Setters
	inline void setWindow(sf::RenderTarget *const target);

	inline void setCursorPosition(const int position);

	inline void setCursorColor(const sf::Color color);

	inline void moveCursor(const int displacement);

	inline void moveCursorToEnd();

	inline void setSelectionLength(int length);

	inline void setSelection(int begin, int end);

	inline void setCharacterSize(const uint size);
	

// Getters
	inline sf::Vector2f getStringEndPosition() const;

	inline sf::Vector2f getStringMaxEndPosition() const;

	inline sf::Vector2f getCursorPosition() const;

// String Manipulations
	void putChar(const char character);

	void putChar(const char character, const int index);

	inline void deleteChar(const int index);

	inline void deleteNext();

	inline void deletePrevious();

	inline void deleteSelected();

	inline void clearCharacters();

// Drawing
	inline void update();

	inline void drawTo();

	inline void drawTo(sf::RenderTarget *const target);

private:
	sf::RectangleShape cursor;
public:
	sf::Text text;
	char string[maxLength+1];
private:
	sf::RenderTarget *window;
	int cursorPosition;
	int selectedNumber;
};






#define TPL template<int maxParamNumber, int maxVarStringLen>
#define CLS SimpleSettingsList<maxParamNumber, maxVarStringLen>

TPL CLS::SimpleSettingsList(const sf::Font &font, const sf::Vector2f position, sf::RenderTarget *const target) noexcept :
		paramNumber{0}, window{target} {

	textSpace = font.getLineSpacing(12);

	*paramNames = 0;

	varText.setFont(font);
	varText.setCharacterSize(12);
	varText.setFillColor(sf::Color{0, 200, 200, 255});

	namesText.setFont(font);
	namesText.setCharacterSize(12);
	namesText.setFillColor(sf::Color{0, 200, 200, 255});

	namesText.setPosition(position);
	varText.setOrigin(-position.x-8.f*textSpace, -position.y);
}

TPL void CLS::pushParameter(const char *const parameterName, const SettingsParamType parameterType, void *const parameter){
	char *const paramNamesEnd = paramNames+strlen(paramNames);
	if (paramNamesEnd != paramNames){
		strcpy(paramNamesEnd+1, parameterName);
		*paramNamesEnd = '\n';
	} else
		strcpy(paramNamesEnd, parameterName);
	namesText.setString(paramNames);

	paramType[paramNumber] = parameterType;
	paramValue[paramNumber] = parameter;
	
	++paramNumber;
}

TPL void CLS::replaceParameter(const char *const parameterName, const SettingsParamType parameterType, void *const parameter){
	char *paramNamesEnd = paramNames + strlen(paramNames) - 1;
	for (; *paramNamesEnd!='\n'; --paramNamesEnd);
	strcpy(paramNamesEnd+1, parameterName);
	namesText.setString(paramNames);

	paramType[paramNumber-1] = parameterType;
	paramValue[paramNumber-1] = parameter;
}
TPL void CLS::popParameter(const int number){
	paramNumber -= number;

	char *I = paramNames + strlen(paramNames) - 1;
	for (int i=0; i<number; ++i)
		for (; *I!='\n'; --I);

	*I = 0;
	namesText.setString(paramNames);
}

TPL inline void CLS::clearParameters(){
	paramNumber = 0;
	*paramNames = 0;
	namesText.setString(paramNames);
}

TPL inline void CLS::setWindow(sf::RenderTarget *const target){
	window = target;
}

TPL inline void CLS::setPosition(const sf::Vector2f &position){
	namesText.setPosition(position);
	varText.setOrigin(-position.x-8.f*textSpace, -position.y);
}

TPL inline void CLS::setTextSize(const uint size){
	textSpace = varText.getFont()->getLineSpacing(varText.getCharacterSize());
	namesText.setCharacterSize(size);
	varText.setCharacterSize(size);
}

TPL inline void CLS::setTextColor(const sf::Color color){
	namesText.setFillColor(color);
	varText.setFillColor(color);
}

TPL inline void CLS::setFont(const sf::Font &font){
	namesText.setFont(font);
	varText.setFont(font);
	textSpace = font.getLineSpacing(varText.getCharacterSize());
}

TPL char *CLS::setValueFromString(const int parameterNumber, const char *const inputString){
	switch (paramType[parameterNumber]){
	case SettingsParamType::Null:
		break;
	case SettingsParamType::Bool:
		*(bool *)paramValue[parameterNumber] = *inputString=='t' || *inputString=='T' || *inputString=='1';
		break;
	case SettingsParamType::Int:
		*(int *)paramValue[parameterNumber] = strtol(inputString, nullptr, 10);
		break;
	case SettingsParamType::Float:
		*(float *)paramValue[parameterNumber] = strtof(inputString, nullptr);
		break;
	case SettingsParamType::Word:
		strcpy((char *)paramValue[parameterNumber], inputString);
		break;
	default:
		break;
	}
	return varString;
}

TPL inline float CLS::getCharacterSpacing() const{
	return textSpace;
}

TPL inline uint CLS::getCharacterSize() const{
	return varText.getCharacterSize();
}

TPL inline sf::Vector2f CLS::getVariableTextPosition(const int index) const{
	return sf::Vector2f{0.f, index*textSpace} - varText.getOrigin();
}

TPL inline float CLS::getVariableTextPositionX() const{
	return -varText.getOrigin().x;
}

TPL inline float CLS::getVariableTextPositionY(const int index) const{
	return index*textSpace - varText.getOrigin().y;
}

TPL inline sf::Color CLS::getTextColor(){
	return varText.getColor();
}

TPL inline int CLS::positionToIndex(const sf::Vector2f position){
	if (position.x > -varText.getOrigin().x && position.x+varText.getOrigin().x < 8.f*textSpace && 
			position.y > -varText.getOrigin().y && position.y+varText.getOrigin().y < paramNumber*textSpace){
		return (position.y+varText.getOrigin().y)/textSpace;
	}
	return -1;
}

TPL char *CLS::getStringOfValue(const int parameterNumber){
	switch (paramType[parameterNumber]){
	case SettingsParamType::Null:
		break;
	case SettingsParamType::Bool:
		if(*(bool *)paramValue[parameterNumber])
			strcpy(varString, "true");
		else
			strcpy(varString, "false");
		break;
	case SettingsParamType::Int:
		sprintf(varString, "%d", *(int *)paramValue[parameterNumber]);
		break;
	case SettingsParamType::Float:
		sprintf(varString, "%.4f", *(float *)paramValue[parameterNumber]);
		break;
	case SettingsParamType::Word:
		strcpy(varString, (char *)paramValue[parameterNumber]);
		break;
	default:
		break;
	}
	return varString;
}

TPL void CLS::drawTo(){
	window->draw(namesText);

	for (int i=0; i<paramNumber; ++i){
		varText.setString(getStringOfValue(i));
		varText.setPosition(0.f, i*textSpace);
		window->draw(varText);
	}
}

TPL void CLS::drawTo(sf::RenderTarget *const target){
	target->draw(namesText);

	for (int i=0; i<paramNumber; ++i){
		varText.setString(getStringOfValue(i));
		varText.setPosition(0.f , i*textSpace);
		target->draw(varText);
	}
}

#undef TPL
#undef CLS



#define TPL template<int maxLength>
#define CLS TypingLine<maxLength>

TPL CLS::TypingLine(const sf::Font &font, const sf::Vector2f position, sf::RenderTarget *const target) noexcept :
		window{target}, cursorPosition{0}, selectedNumber{0} {
	
	*string = 0;

	text.setFont(font);

	text.setCharacterSize(12);
	text.setFillColor(sf::Color{0, 200, 200, 255});
	text.setPosition(position);
	
	cursor.setFillColor(sf::Color{0, 255, 100, 255});
	cursor.setSize(sf::Vector2f{font.getLineSpacing(12), font.getLineSpacing(12)});
	cursor.setOrigin(0.f, font.getLineSpacing(12)*0.1f);
}

TPL inline void CLS::setWindow(sf::RenderTarget *const target){
	window = target;
}

TPL inline void CLS::setCursorPosition(const int position){
	cursorPosition = sp::clamp(0, position, (int)strlen(string));
	selectedNumber = std::min(selectedNumber, (int)strlen(string)-cursorPosition);
}

TPL inline void CLS::setCursorColor(const sf::Color color){
	cursor.setFillColor(color);
}

TPL inline void CLS::moveCursor(const int displacement){
	cursorPosition = sp::clamp(0, cursorPosition+displacement, (int)strlen(string));
	selectedNumber = std::min(selectedNumber, (int)strlen(string)-cursorPosition);
}

TPL inline void CLS::moveCursorToEnd(){
	cursorPosition = strlen(string);
	selectedNumber = std::min(selectedNumber, (int)strlen(string)-cursorPosition);
}

TPL inline void CLS::setSelectionLength(int length){
	sp::clamp(0, &length, strlen(string)-cursorPosition);
}

TPL inline void CLS::setSelection(int begin, int end){
	selectedNumber = sp::clamp(begin, end, strlen(string)) - sp::clamp(0, begin, end);
}

TPL inline void CLS::setCharacterSize(const uint size){
	text.setCharacterSize(size);
	cursor.setSize(sf::Vector2f{text.getFont()->getLineSpacing(size), text.getFont()->getLineSpacing(size)});
	cursor.setOrigin(0.f, text.getFont()->getLineSpacing(size)*0.1f);
}

TPL inline sf::Vector2f CLS::getStringEndPosition() const{
	return text.getPosition()+sf::Vector2f(strlen(string)*text.getLetterSpacing(), 0.f);
}

TPL inline sf::Vector2f CLS::getStringMaxEndPosition() const{
	return text.getPosition()+sf::Vector2f(maxLength*text.getLetterSpacing(), 0.f);
}

TPL inline sf::Vector2f CLS::getCursorPosition() const{
	return text.findCharacterPos(cursorPosition) + sf::Vector2f{0.f, text.getFont()->getLineSpacing(text.getCharacterSize())*0.1f};
}

TPL void CLS::putChar(const char character){
	char *I = string+strlen(string)+1;
	if (I <= string+maxLength){
		for (; I!=string+cursorPosition; --I)
			*I = *(I-1);
		string[cursorPosition] = character;
		++cursorPosition;
	}
}

TPL void CLS::putChar(const char character, const int index){
	char *I = string+strlen(string)+1;
	if (I <= string+maxLength){
		for (; I!=string+index; --I)
			*I = *(I-1);
		string[index] = character;
	}
}

TPL inline void CLS::deleteChar(const int index){
	strcpy(string+cursorPosition, string+cursorPosition+1);
	cursorPosition -= string[cursorPosition+1]==0;
}

TPL inline void CLS::deleteNext(){
	if (cursorPosition != (int)strlen(string))
		strcpy(string+cursorPosition, string+cursorPosition+1);
}

TPL inline void CLS::deletePrevious(){
	if (cursorPosition != 0){
		strcpy(string+cursorPosition-1, string+cursorPosition);
		--cursorPosition;
	}
}

TPL inline void CLS::deleteSelected(){
	strcpy(string+cursorPosition, string+cursorPosition+selectedNumber);
	cursorPosition = std::min(cursorPosition, strlen(string));
}

TPL inline void CLS::clearCharacters(){
	*string = 0;
	cursorPosition = 0;
}

TPL inline void CLS::update(){
	text.setString(string);
	cursor.setPosition(text.findCharacterPos(cursorPosition));
	cursor.setScale(selectedNumber==0 ? 0.1f : (float)selectedNumber, 0.9f);
}

TPL inline void CLS::drawTo(){
	window->draw(cursor);
	window->draw(text);
}

TPL inline void CLS::drawTo(sf::RenderTarget *const target){
	target->draw(cursor);
	target->draw(text);
}

#undef TPL
#undef CLS

} // END OF NAMESPACE //////////////////////////////////////////////////////////////////////////////
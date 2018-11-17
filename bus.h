#pragma once

#include <Arduino.h>
#include <memory>

class bus
{
private:

	unsigned int _seconds_to_arrive;
	String _name;
	
public:

	bus();
	bus(const String & name, unsigned int seconds_to_arrive);
	
	const String & name();
	int seconds_to_arrive();
	
	typedef std::shared_ptr<bus> pointer_t;
};

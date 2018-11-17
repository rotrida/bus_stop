#include "bus.h"

bus::bus()
{
}

bus::bus(const String & name, unsigned int seconds_to_arrive):
	_name(name), _seconds_to_arrive(seconds_to_arrive)
{
}

const String & bus::name()
{
	return _name;
}

int bus::seconds_to_arrive()
{
	return _seconds_to_arrive;
}

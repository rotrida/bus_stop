#pragma once

#include <Arduino.h>
#include "bus.h"
#include <map>

typedef std::map<unsigned int, bus::pointer_t> buses_map;

class bus_stop
{
private:

	bool _error;
	String _bus_point;
	String _last_error;
	
	buses_map _next_buses;
	
	bool parse_data(const String & data);
	void set_last_error(const String & error);

public:

	bus_stop(const String & bus_point);
	
	bool error() const;
	String last_error_message() const;
	
	bool request_data();
	const buses_map & next_buses() const;
};

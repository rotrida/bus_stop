#include "bus_stop.h"
#include "ArduinoJson.h"
#include "https_client.h"
#include "utils.h"

std::pair<bool, unsigned int> parse_time(const String & date)
{
	auto parts = utils::split(date, 'T');
	
	if(parts.size() < 2)
		return std::make_pair<bool, unsigned int>(false, 0);
		
	auto & time_part = parts[1];
	
	auto seconds_part = utils::split(time_part, '.');
	
	auto time_only = seconds_part[0];
	const auto size = time_only.length();
	
	if(size == 0)
		return std::make_pair<bool, unsigned int>(false, 0);
	
	if( time_only[size-1] == 'Z' )
		time_only.remove(size-1);
	
	auto time = utils::split(time_only, ':');
	
	if(time.size() < 3 )
		return std::make_pair<bool, unsigned int>(false, 0);
	
	const auto seconds = 3600 * atoi(time[0].c_str()) + 60 * atoi(time[1].c_str()) + atoi(time[2].c_str());
	
	return std::make_pair<bool, unsigned int>(true, static_cast<unsigned int>(seconds));
}

bool bus_stop::parse_data(const String & data)
{
	DynamicJsonBuffer jsonBuffer(7000);
	
	JsonArray& root = jsonBuffer.parseArray(data);
	
	if (!root.success()) 
	{
		set_last_error("Error parsing json data");
		return false;
	}
	
	const int size = root.size();
	
	for(int i = 0; i < size; i++)
	{
		JsonObject& object = root[i];
		
		const char * const line_name_key  = "lineName";
		
		if (!object.containsKey(line_name_key)) 
		{
			set_last_error(String("Key ") + line_name_key + " not find in json");
			continue;
		}
		const char * line_name = object[line_name_key];
		
		const char * const timestamp_key  = "timestamp";
		
		if (!object.containsKey(timestamp_key)) 
		{
			set_last_error(String("Key ") + timestamp_key + " not find in json");
			continue;
		}
		const char * current_time = object[timestamp_key];
		
		const char * const arrival_key  = "expectedArrival";
		
		if (!object.containsKey(arrival_key)) 
		{
			set_last_error(String("Key ") + arrival_key + " not find in json");
			continue;
		}
		const char * arrival_time = object[arrival_key];
		
		const auto seconds_now = parse_time(current_time);
		const auto seconds_arrival = parse_time(arrival_time);

		if(!seconds_now.first)
		{
			set_last_error("Error converting current time");
			continue;
		}
		
		if(!seconds_arrival.first)
		{
			set_last_error("Error converting arrival time");
			continue;
		}
		
		bus::pointer_t bus_ptr(new bus(line_name, seconds_arrival.second - seconds_now.second));

		_next_buses[bus_ptr->seconds_to_arrive()] = bus_ptr;
	}
	
	return true;
}

void bus_stop::set_last_error(const String & error)
{
	_error = true;
	_last_error = error;
}

bus_stop::bus_stop(const String & bus_point):
	_bus_point(bus_point),
	_error(false)
{}

bool bus_stop::error() const
{
	return _error;
}

String bus_stop::last_error_message() const
{
	return _last_error;
}

const buses_map & bus_stop::next_buses() const
{
	return _next_buses;
}

bool bus_stop::request_data()
{
	_error = false;
	String url = "api.tfl.gov.uk/StopPoint/" + _bus_point + "/Arrivals";
	
	https_client bus_client(url, 443);

    if(!bus_client.execute())
    {
		set_last_error("Error requesting buses arrival.");
		return false;
	}

	return parse_data(bus_client.get_response());
}

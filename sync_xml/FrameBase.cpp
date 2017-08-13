#include <iostream>

#include "FrameBase.h"

using namespace std;

FrameBase::FrameBase()
{
	arrival_time = 0;
	departure_time = 0;
}

void FrameBase::setArrivalTime()
{
	arrival_time = getMicroSeconds();
}

void FrameBase::setArrivalTime(long long int _arrival_time)
{
	arrival_time = _arrival_time;
}

long long int FrameBase::getArrivalTime()
{
	return arrival_time;
}

void FrameBase::setDepartureTime()
{
	departure_time = getMicroSeconds();
}

void FrameBase::setDepartureTime(long long int _departure_time)
{
	departure_time = _departure_time;
}

long long int FrameBase::getDepartureTime()
{
	return departure_time;
}

long long int FrameBase::getTimeOffsetAsUsec()
{
	return (departure_time - arrival_time);
}

long long int FrameBase::getTimeOffsetAsMsec()
{
	return getTimeOffsetAsUsec() / 1000;
}

long long int FrameBase::getTimeOffsetAsSec()
{
	return getTimeOffsetAsMsec() / 1000;
}

long long int FrameBase::getTimeOffsetAsPTS()
{
	return getTimeOffsetAsUsec() * 0.090;
}

FrameBase::~FrameBase(){}

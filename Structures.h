#pragma once


struct Train
{
	int id;
	char name[16];
	char route[16];
	long firstPassengerAddress;
	int passengersCount;
};

struct Passenger
{
	int trainId;
	int productId;
	char name[16];
	int seatNumber;
	int ticket_price;
	int exists;
	long selfAddress;
	long nextAddress;
};

struct Indexer
{
	int id;	
	int address;
	int exists;
};
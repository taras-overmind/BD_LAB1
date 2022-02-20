#pragma once

#include <stdio.h>
#include "Train.h"
#include "Structures.h"

void printTrain(struct Train train)
{
	printf("Train\'s number: %s\n", train.name);
	printf("Train\'s route: %s\n", train.route);
}

void printPassenger(struct Passenger passenger, struct Train train)
{
	printf("Train: %s\n", train.name);
	printf("Route: %s\n", train.route);
	printf("Name: %s\n", passenger.name);
	printf("Price: %d\n", passenger.seatNumber);
	printf("Tiket price: %d\n", passenger.ticket_price);
}
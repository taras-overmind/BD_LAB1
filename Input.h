#pragma once
#include <stdio.h>
#include <string.h>
#include "Structures.h"
#include "Train.h"

void readTrain(struct Train* train)
{
	char name[16];
	char route[16];

	name[0] = '\0';

	printf("Enter train\'s name: ");
	scanf("%s", name);

	strcpy(train->name, name);

	printf("Enter train\'s route: ");
	scanf("%s", route);

	strcpy(train->route, route);
}

void readPassenger(struct Passenger* passenger)
{
	int x;
	char y[16];

	printf("Enter passenger\'s name: ");
	scanf("%s", y);

	strcpy(passenger->name, y);

	printf("Enter seat number: ");
	scanf("%d", &x);

	passenger->seatNumber = x;

	printf("Enter ticket_price: ");
	scanf("%d", &x);

	passenger->ticket_price = x;
}
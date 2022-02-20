#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Passenger.h"

int checkFileExistence(FILE* indexTable, FILE* database, char* error)
{
	if (indexTable == NULL || database == NULL)
	{
		strcpy(error, "database files are not created yet");
		return 0;
	}

	return 1;
}

int checkIndexExistence(FILE* indexTable, char* error, int id)
{
	fseek(indexTable, 0, SEEK_END);

	long indexTableSize = ftell(indexTable);

	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize)
	{
		strcpy(error, "no such ID in table");
		return 0;
	}

	return 1;
}

int checkRecordExistence(struct Indexer indexer, char* error)
{

	if (!indexer.exists)
	{
		strcpy(error, "the record you\'re looking for has been removed");
		return 0;
	}

	return 1;
}


int checkKeyPairUniqueness(struct Train train, int productId)
{
	FILE* passengersDb = fopen(PASSANGER_DATA, "r+b");
	struct Passenger passenger;

	fseek(passengersDb, train.firstPassengerAddress, SEEK_SET);

	for (int i = 0; i < train.passengersCount; i++)
	{
		fread(&passenger, PASSANGER_SIZE, 1, passengersDb);
		fclose(passengersDb);

		if (passenger.productId == productId)
		{
			return 0;
		}

		passengersDb = fopen(PASSANGER_DATA, "r+b");
		fseek(passengersDb, passenger.nextAddress, SEEK_SET);
	}

	fclose(passengersDb);

	return 1;
}

void info()
{
	FILE* indexTable = fopen("train.ind", "rb");

	if (indexTable == NULL)
	{
		printf("Error: database files are not created yet\n");
		return;
	}

	int trainCount = 0;
	int passengerCount = 0;

	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);

	struct Train train;

	char dummy[51];

	for (int i = 1; i <= indAmount; i++)
	{
		if (getTrain(&train, i, dummy))
		{
			trainCount++;
			passengerCount += train.passengersCount;

			printf("Train #%d has %d passenger(s)\n", i, train.passengersCount);
		}
	}

	fclose(indexTable);

	printf("Total trains: %d\n", trainCount);
	printf("Total passengers: %d\n", passengerCount);
}

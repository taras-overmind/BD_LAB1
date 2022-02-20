#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"
#include "Checks.h"
#include "Train.h"

#define PASSANGER_DATA "passenger.fl"
#define PASSANGER_GARBAGE "passenger_garbage.txt"
#define PASSANGER_SIZE sizeof(struct Passenger)


void reopenDatabase(FILE* database)
{
	fclose(database);
	database = fopen(PASSANGER_DATA, "r+b");
}

void linkAddresses(FILE* database, struct Train train, struct Passenger passenger)
{
	reopenDatabase(database);							

	struct Passenger previous;

	fseek(database, train.firstPassengerAddress, SEEK_SET);

	for (int i = 0; i < train.passengersCount; i++)		   
	{
		fread(&previous, PASSANGER_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}

	previous.nextAddress = passenger.selfAddress;				
	fwrite(&previous, PASSANGER_SIZE, 1, database);			
}

void relinkAddresses(FILE* database, struct Passenger previous, struct Passenger passenger, struct Train* train)
{
	if (passenger.selfAddress == train->firstPassengerAddress)		
	{
		if (passenger.selfAddress == passenger.nextAddress)		
		{
			train->firstPassengerAddress = -1;				
		}
		else                                               
		{
			train->firstPassengerAddress = passenger.nextAddress; 
		}
	}
	else                                                  
	{
		if (passenger.selfAddress == passenger.nextAddress)		
		{
			previous.nextAddress = previous.selfAddress;    
		}
		else                                               
		{
			previous.nextAddress = passenger.nextAddress;		
		}

		fseek(database, previous.selfAddress, SEEK_SET);	
		fwrite(&previous, PASSANGER_SIZE, 1, database);			
	}
}

void noteDeletedPassenger(long address)
{
	FILE* garbageZone = fopen(PASSANGER_GARBAGE, "rb");			

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	long* delAddresses = malloc(garbageCount * sizeof(long)); 

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%ld", delAddresses + i);		
	}

	fclose(garbageZone);									
	garbageZone = fopen(PASSANGER_GARBAGE, "wb");				
	fprintf(garbageZone, "%ld", garbageCount + 1);			

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %ld", delAddresses[i]);	
	}

	fprintf(garbageZone, " %d", address);					
	free(delAddresses);									
	fclose(garbageZone);									
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Passenger* record)
{
	long* delIds = malloc(garbageCount * sizeof(long));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	record->selfAddress = delIds[0];						
	record->nextAddress = delIds[0];

	fclose(garbageZone);									
	fopen(PASSANGER_GARBAGE, "wb");							  
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	free(delIds);											
	fclose(garbageZone);									
}

int insertPassenger(struct Train train, struct Passenger passenger, char* error)
{
	passenger.exists = 1;

	FILE* database = fopen(PASSANGER_DATA, "a+b");
	FILE* garbageZone = fopen(PASSANGER_GARBAGE, "rb");

	int garbageCount;											

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)											
	{
		overwriteGarbageAddress(garbageCount, garbageZone, &passenger);
		reopenDatabase(database);								
		fseek(database, passenger.selfAddress, SEEK_SET);			
	}
	else                                                        
	{
		fseek(database, 0, SEEK_END);

		int dbSize = ftell(database);

		passenger.selfAddress = dbSize;	
		passenger.nextAddress = dbSize;
	}

	fwrite(&passenger, PASSANGER_SIZE, 1, database);					

	if (!train.passengersCount)								   
	{
		train.firstPassengerAddress = passenger.selfAddress;
	}
	else                                                       
	{
		linkAddresses(database, train, passenger);
	}

	fclose(database);											

	train.passengersCount++;										
	updateTrain(train, error);								

	return 1;
}

int getPassenger(struct Train train, struct Passenger* passenger, int productId, char* error)
{
	if (!train.passengersCount)									
	{
		strcpy(error, "This train has no passengers yet");
		return 0;
	}

	FILE* database = fopen(PASSANGER_DATA, "rb");


	fseek(database, train.firstPassengerAddress, SEEK_SET);		
	fread(passenger, PASSANGER_SIZE, 1, database);

	for (int i = 0; i < train.passengersCount; i++)				
	{
		if (passenger->productId == productId)						
		{
			fclose(database);
			return 1;
		}

		fseek(database, passenger->nextAddress, SEEK_SET);
		fread(passenger, PASSANGER_SIZE, 1, database);
	}
	
	strcpy(error, "No such passenger in database");					
	fclose(database);
	return 0;
}

int updatePassenger(struct Passenger passenger, int productId)
{
	FILE* database = fopen(PASSANGER_DATA, "r+b");

	fseek(database, passenger.selfAddress, SEEK_SET);
	fwrite(&passenger, PASSANGER_SIZE, 1, database);
	fclose(database);
	
	return 1;
}

int deletePassenger(struct Train train, struct Passenger passenger, int productId, char* error)
{
	FILE* database = fopen(PASSANGER_DATA, "r+b");
	struct Passenger previous;

	fseek(database, train.firstPassengerAddress, SEEK_SET);

	do		                                                    
	{															
		fread(&previous, PASSANGER_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}
	while (previous.nextAddress != passenger.selfAddress && passenger.selfAddress != train.firstPassengerAddress);

	relinkAddresses(database, previous, passenger, &train);
	noteDeletedPassenger(passenger.selfAddress);						

	passenger.exists = 0;											

	fseek(database, passenger.selfAddress, SEEK_SET);				
	fwrite(&passenger, PASSANGER_SIZE, 1, database);				
	fclose(database);

	train.passengersCount--;									
	updateTrain(train, error);

}
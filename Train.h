#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Checks.h"
#include "Passenger.h"

#define MASTER_IND "train.ind"
#define MASTER_DATA "train.fl"
#define MASTER_GARBAGE "train_garbage.txt"
#define INDEXER_SIZE sizeof(struct Indexer)
#define MASTER_SIZE sizeof(struct Train)




void noteDeletedTrain(int id)
{
	FILE* garbageZone = fopen(MASTER_GARBAGE, "rb");	

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	int* delIds = malloc(garbageCount * sizeof(int));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);			
	}

	fclose(garbageZone);									
	garbageZone = fopen(MASTER_GARBAGE, "wb");				
	fprintf(garbageZone, "%d", garbageCount + 1);			

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	fprintf(garbageZone, " %d", id);						
	free(delIds);											
	fclose(garbageZone);									
}

void overwriteGarbageId(int garbageCount, FILE* garbageZone, struct Train* record)
{
	int* delIds = malloc(garbageCount * sizeof(int));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	record->id = delIds[0];									

	fclose(garbageZone);									
	fopen(MASTER_GARBAGE, "wb");							
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	free(delIds);											
	fclose(garbageZone);									
}

int insertTrain(struct Train record)
{
	FILE* indexTable = fopen(MASTER_IND, "a+b");			
	FILE* database = fopen(MASTER_DATA, "a+b");				
	FILE* garbageZone = fopen(MASTER_GARBAGE, "rb");		
	struct Indexer indexer;
	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)										
	{
		overwriteGarbageId(garbageCount, garbageZone, &record);

		fclose(indexTable);									
		fclose(database);									

		indexTable = fopen(MASTER_IND, "r+b");				
		database = fopen(MASTER_DATA, "r+b");				

		fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
		fread(&indexer, INDEXER_SIZE, 1, indexTable);
		fseek(database, indexer.address, SEEK_SET);			
	}
	else                                                   
	{
		long indexerSize = INDEXER_SIZE;

		fseek(indexTable, 0, SEEK_END);						

		if (ftell(indexTable))								
		{
			fseek(indexTable, -indexerSize, SEEK_END);		
			fread(&indexer, INDEXER_SIZE, 1, indexTable);	

			record.id = indexer.id + 1;						
		}
		else                                               
		{
			record.id = 1;									
		}
	}

	record.firstPassengerAddress = -1;
	record.passengersCount = 0;

	fwrite(&record, MASTER_SIZE, 1, database);				

	indexer.id = record.id;									
	indexer.address = (record.id - 1) * MASTER_SIZE;		
	indexer.exists = 1;										

	printf("Your train\'s id is %d\n", record.id);

	fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET); 
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);			
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int getTrain(struct Train* train, int id, char* error)
{
	FILE* indexTable = fopen(MASTER_IND, "rb");				
	FILE* database = fopen(MASTER_DATA, "rb");				
	
	if (!checkFileExistence(indexTable, database, error))
	{
		return 0;
	}
	
	struct Indexer indexer;

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);			

	if (!checkRecordExistence(indexer, error))
	{
		return 0;
	}

	fseek(database, indexer.address, SEEK_SET);				
	fread(train, sizeof(struct Train), 1, database);		
	fclose(indexTable);									
	fclose(database);

	return 1;
}

int updateTrain(struct Train train, char* error)
{
	FILE* indexTable = fopen(MASTER_IND, "r+b");			
	FILE* database = fopen(MASTER_DATA, "r+b");		

	if (!checkFileExistence(indexTable, database, error))
	{	
		return 0;
	}

	struct Indexer indexer;
	int id = train.id;

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);		

	if (!checkRecordExistence(indexer, error))
	{
		return 0;
	}

	fseek(database, indexer.address, SEEK_SET);			
	fwrite(&train, MASTER_SIZE, 1, database);		
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int deleteTrain(int id, char* error)
{
	FILE* indexTable = fopen(MASTER_IND, "r+b");			
															
	if (indexTable == NULL)
	{
		strcpy(error, "database files are not created yet");
		return 0;
	}

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	struct Train train;
	getTrain(&train, id, error);

	struct Indexer indexer;

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);			

	indexer.exists = 0;										

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);

	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);			
	fclose(indexTable);										

	noteDeletedTrain(id);									

	
	if (train.passengersCount)								
	{
		FILE* passengersDb = fopen(PASSANGER_DATA, "r+b");
		struct Passenger passenger;

		fseek(passengersDb, train.firstPassengerAddress, SEEK_SET);

		for (int i = 0; i < train.passengersCount; i++)
		{
			fread(&passenger, PASSANGER_SIZE, 1, passengersDb);
			fclose(passengersDb);
			deletePassenger(train, passenger, passenger.productId, error);
			
			passengersDb = fopen(PASSANGER_DATA, "r+b");
			fseek(passengersDb, passenger.nextAddress, SEEK_SET);
		}

		fclose(passengersDb);
	}
	return 1;
}
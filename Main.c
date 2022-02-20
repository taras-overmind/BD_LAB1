#include <stdio.h>
#include "Structures.h"
#include "Train.h"
#include "Passenger.h"
#include "Input.h"
#include "Output.h"


int main()
{
	struct Train train;
	struct Passenger passenger;

	while (1)
	{
		int choice;
		int id;
		char error[51];

		printf("Choose option:\n1 - Insert Train\n2 - Insert Passenger\n3 - Get Train\n4 - Get Passenger\n5 - Update Train\n6 - Update Passenger\n7 - Delete Train\n8 - Delete Passenger\n9 - Info\n0 - exit\n");
		scanf("%d", &choice);

		switch (choice)
		{
		case 0:
			return 0;

		case 1:
			readTrain(&train);
			insertTrain(train);
			break;

		case 3:
			printf("Enter ID: ");
			scanf("%d", &id);
			getTrain(&train, id, error) ? printTrain(train) : printf("Error: %s\n", error);
			break;

		case 5:
			printf("Enter ID: ");
			scanf("%d", &id);

			train.id = id;

			readTrain(&train);
			updateTrain(train, error) ? printf("Updated successfully\n") : printf("Error: %s\n", error);
			break;

		case 7:
			printf("Enter ID: ");
			scanf("%d", &id);
			deleteTrain(id, error) ? printf("Deleted successfully\n") : printf("Error: %s\n", error);
			break;

		case 2:
			printf("Enter train\'s ID: ");
			scanf("%d", &id);

			if (getTrain(&train, id, error))
			{
				passenger.trainId = id;
				printf("Enter product ID: ");
				scanf("%d", &id);

				passenger.productId = id;
				readPassenger(&passenger);
				insertPassenger(train, passenger, error);
				printf("Inserted successfully. To access, use train\'s and product\'s IDs\n");
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 4:
			printf("Enter train\'s ID: ");
			scanf("%d", &id);

			if (getTrain(&train, id, error))
			{
				printf("Enter passenger\'s ID: ");
				scanf("%d", &id);
				getPassenger(train, &passenger, id, error) ? printPassenger(passenger, train) : printf("Error: %s\n", error);
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 6:
			printf("Enter train\'s ID: ");
			scanf("%d", &id);

			if (getTrain(&train, id, error))
			{
				printf("Enter passenger\'s ID: ");
				scanf("%d", &id);

				if (getPassenger(train, &passenger, id, error))
				{
					readPassenger(&passenger);
					updatePassenger(passenger, id, error);
					printf("Updated successfully\n");
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 8:
			printf("Enter train\'s ID: ");
			scanf("%d", &id);

			if (getTrain(&train, id, error))
			{
				printf("Enter product\'s ID: ");
				scanf("%d", &id);

				if (getPassenger(train, &passenger, id, error))
				{
					deletePassenger(train, passenger, id, error);
					printf("Deleted successfully\n");
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 9:
			info();
			break;

		default:
			printf("Invalid input, please try again\n");
		}

		printf("---------\n");
	}

	return 0;
}

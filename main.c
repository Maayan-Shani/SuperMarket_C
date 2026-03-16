#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "General.h"
#include "Supermarket.h"
#include "SuperFile.h"

int main(int argc, char* argv[])
{
	SuperMarket	market;

	srand((unsigned int)time(NULL));
	int isCompressed, res;
	if (argc != 3)
	{
		printf("Usage: insert 0 or 1 and file name\n");
		return 0;
	}
	res = sscanf(argv[1], "%d", &isCompressed);
	if (res != 1)
	{
		printf("Error in main parameters\n");
		return 0;
	}

	if (!initSuperMarket(&market, argv[2], CUSTOMER_FILE_NAME, isCompressed))
	{
		printf("error init Super Market");
		return 0;
	}

	int option;
	int stop = 0;
	
	do
	{
		option = menu();
		switch (option)
		{
		case eShowSuperMarket:
			printSuperMarket(&market);
			break;

		case eAddProduct:
			if (!addProduct(&market))
				printf("Error adding product\n");
			break;

		case eAddCustomer:
			if (!addCustomer(&market))
				printf("Error adding customer\n");
			break;

		case eCustomerDoShopping:
			if(!doShopping(&market))
				printf("Error in shopping\n");
			break;

		case ePrintCart:
			doPrintCart(&market);
			break;

		case eCustomerManageShoppingCart:
			if(!manageShoppingCart(&market))
				printf("Error in shopping cart managment\n");
			break;

		case eSortProducts:
			sortProducts(&market);
			break;

		case eSearchProduct:
			findProduct(&market);
			break;

		case ePrintProductByType:
			printProductByType(&market);
			break;

		case EXIT:
			printMessage("Thank", "You", "For", "Shopping", "With", "Us", NULL);
			stop = 1;
			break;

		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);

	handleCustomerStillShoppingAtExit(&market);
	if (isCompressed == 1)
	{
		if(!saveDataSuperMarketToBinaryFileCompressed(&market, SUPER_FILEC_NAME, CUSTOMER_FILE_NAME))
			printf("Error saving supermarket to file\n");
	}
	else {
		if (!saveSuperMarketToFile(&market, SUPER_FILE_NAME, CUSTOMER_FILE_NAME))
			printf("Error saving supermarket to file\n");
	}
	freeMarket(&market);
	return 1;
}

int menu()
{
	int option;
	printf("\n");
	printf("Please choose one of the following options\n");
	for(int i = 0 ; i < eNofOptions ; i++)
		printf("%d - %s\n",i, menuStrings[i]);
	printf("%d - Quit\n", EXIT);
	scanf("%d", &option);
	//clean buffer
	char tav;
	scanf("%c", &tav);
	return option;
}
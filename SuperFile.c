#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "General.h"
#include "FileHelper.h"
#include "SuperFile.h"
#include "Product.h"
#include "myMacros.h"

static const char* typePrefix[eNofProductType] = { "FV", "FR", "FZ", "SH" };

int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_MSG_RETURN_0(fp, "Error open supermarket file to write\n");
	if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
	{
		fclose(fp);
		return 0;
	}
	if (!writeIntToFile(pMarket->productCount, fp, "Error write product count\n"))
	{
		fclose(fp);
		return 0;
	}
	for (int i = 0; i < pMarket->productCount; i++)
	{
		if (!saveProductToFile(pMarket->productArr[i], fp))
		{
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	saveCustomersToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);
	return 1;
}

int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	CHECK_MSG_RETURN_0(fp, "Error open company file\n");
	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}
	int count;
	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}
	pMarket->productArr = (Product**)malloc(count * sizeof(Product*));
	if (!pMarket->productArr)
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	}
	pMarket->productCount = count;
	for (int i = 0; i < count; i++)
	{
		pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
		if (!pMarket->productArr[i])
		{
			FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		}
		if (!loadProductFromFile(pMarket->productArr[i], fp))
		{
			free(pMarket->productArr[i]);
			FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		}
	}
	fclose(fp);
	pMarket->customerArr = loadCustomersFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;
	return	1;
}

int saveDate(const Product* product, FILE* fp)
{
	BYTE data[2] = { 0 };
	Date date = product->expiryDate;
	int year = date.year;
	int month = date.month;
	int day = date.day;
	data[0] = day << 3 | month >> 1;
	data[1] = ((month & 0x1) << 7) | (year - 2024) << 4;
	if (fwrite(&data, sizeof(BYTE), 2, fp) != 2)
		return 0;
	return 1;
}

int saveDataProductsCompressed(const SuperMarket* pMarket, FILE* fp)
{
	for (int i = 0; i < pMarket->productCount; i++)
	{
		BYTE data[4] = { 0 };
		Product* currPro = pMarket->productArr[i];
		char* barcode = currPro->barcode;
		int productLen = (int)strlen(currPro->name);
		data[0] = (barcode[2] - '0') << 4 | (barcode[3] - '0');
		data[1] = (barcode[4] - '0') << 4 | (barcode[5] - '0');
		data[2] = (barcode[6] - '0') << 4 | (currPro->type & 0x3) << 2 | ((productLen >> 2) & 0x3);
		data[3] = (productLen & 0x3) << 6;
		if (fwrite(&data, sizeof(BYTE), 4, fp) != 4)
			return 0;
		if (fwrite(currPro->name, sizeof(char), productLen, fp) != productLen)
			return 0;
		BYTE dataPC[3] = { 0 };
		dataPC[0] = currPro->count;
		int full = (int)currPro->price;
		int pennies = (int)(currPro->price * 100) % 100;
		dataPC[1] = (pennies << 1) | (full >> 8);
		dataPC[2] = full & 0xFF;
		if (fwrite(&dataPC, sizeof(BYTE), 3, fp) != 3)
			return 0;
		if (!saveDate(currPro, fp))
			return 0;
	}
	return 1;
}

int saveDataSuperMarketToBinaryFileCompressed(const SuperMarket* pMarket,char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_MSG_RETURN_0(fp, "Error open company file\n");
	BYTE data[2] = { 0 };
	int len = (int)strlen(pMarket->name);
	int productCount = pMarket->productCount;
	data[0] = productCount >> 2;
	data[1] = (productCount & 0x3) << 6 | len;
	if (fwrite(&data, sizeof(BYTE), 2, fp) != 2)
	{
		CLOSE_RETURN_0(fp);
	}
	if (fwrite(pMarket->name,sizeof(char), len, fp) != len)
	{
		CLOSE_RETURN_0(fp);
	}
	if (!saveDataProductsCompressed(pMarket, fp))
	{
		CLOSE_RETURN_0(fp);
	}
	fclose(fp);
	saveCustomersToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);
	return 1;
}

int loadDateProductFromBinaryFileCompressed(FILE* fp,Date* date)
{
	BYTE dataD[2];
	if (fread(&dataD, sizeof(BYTE), 2, fp) != 2)
		return 0;
	date->day = (dataD[0] >> 3) & 0x1F;
	date->month = (dataD[0] & 0x7) << 1 | (dataD[1] >> 7) & 0x1;
	date->year = ((dataD[1] >> 4) & 0x7) + 2024;
	return 1;
}

int loadProductPriceCountFromBinaryFileCompressed(FILE* fp, Product* product)
{
	BYTE dataPC[3];
	if (fread(&dataPC, sizeof(BYTE), 3, fp) != 3)
		return 0;
	product->count = dataPC[0];
	product->price = (dataPC[2] | ((dataPC[1] & 0x1) << 8)) + (((float)(dataPC[1] >> 1)) / 100);
	return 1;
}

void loadBarcodeDigitsFromBinaryFileCompressed(FILE* fp, Product* product,BYTE* data, char* digits)
{
	int count = BARCODE_LENGTH - PREFIX_LENGTH;
	digits[0] = ((data[0] >> 4) & 0xF) + '0';
	digits[1] = (data[0] & 0xF) + '0';
	digits[2] = ((data[1] >> 4) & 0xF) + '0';
	digits[3] = (data[1] & 0xF) + '0';
	digits[4] = ((data[2] >> 4) & 0xF) + '0';
}

int loadProductsFromBinaryFileCompressed(SuperMarket* pMarket, FILE* fp)
{
	pMarket->productArr = (Product**)malloc(sizeof(Product*) * pMarket->productCount);
	if (!pMarket->productArr)
		return 0;
	for (int i = 0; i < pMarket->productCount; i++)
	{
		BYTE data[4];
		pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
		if (!pMarket->productArr[i])
			return 0;
		if (fread(&data, sizeof(BYTE), 4, fp) != 4)
			return 0;
		pMarket->productArr[i]->type = (data[2] >> 2) & 0x3;
		const char* prefix = typePrefix[pMarket->productArr[i]->type];
		char digits[6] = {0};
		loadBarcodeDigitsFromBinaryFileCompressed(fp, pMarket->productArr[i], data, digits);
		int nameLen = ((data[2] & 0x3) << 2) | ((data[3] >> 6) & 0x3);
		if (fread(pMarket->productArr[i]->name, sizeof(char), nameLen, fp) != nameLen)
			return 0;
		pMarket->productArr[i]->name[nameLen] = '\0';
		strcpy(pMarket->productArr[i]->barcode, prefix);
		strcat(pMarket->productArr[i]->barcode, digits);
		if (!loadProductPriceCountFromBinaryFileCompressed(fp, pMarket->productArr[i]))
			return 0;
		Date* date = &pMarket->productArr[i]->expiryDate;
		if (!loadDateProductFromBinaryFileCompressed(fp, date))
			return 0;
	}
	return 1;
}

int loadSuperMarketFromBinaryFileCompressed(SuperMarket* pMarket,const char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}
	BYTE data[2] = { 0 };
	if (fread(&data, sizeof(BYTE), 2, fp) != 2)
	{
		CLOSE_RETURN_0(fp);
	}
	pMarket->productCount = (data[0] << 2) | (data[1] >> 6);
	int len = data[1] & 0x3F;
	pMarket->name = (char*)calloc(len + 1, sizeof(char));
	if (!pMarket->name)
	{
		CLOSE_RETURN_0(fp);
	}
	if (fread(pMarket->name, sizeof(char), len, fp) != len)
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}
	if (!loadProductsFromBinaryFileCompressed(pMarket, fp))
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}
	fclose(fp);
	pMarket->customerArr = loadCustomersFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;
	return 1;
}

int	saveCustomersToTextFile(const Customer* customerArr, int customerCount, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(customersFileName, "w");
	if (!fp) {
		printf("Error opening customers file to write\n");
		return 0;
	}

	fprintf(fp, "%d\n", customerCount);
	for (int i = 0; i < customerCount; i++)
		customerArr[i].vTable.saveToFile(&customerArr[i], fp);

	fclose(fp);
	return 1;
}

Customer* loadCustomersFromTextFile(const char* customersFileName, int* pCount)
{
	FILE* fp;

	fp = fopen(customersFileName, "r");
	if (!fp) {
		printf("Error open customers file to write\n");
		return NULL;
	}

	Customer* customerArr = NULL;
	int customerCount;

	fscanf(fp, "%d\n", &customerCount);

	if (customerCount > 0)
	{
		customerArr = (Customer*)calloc(customerCount, sizeof(Customer)); //cart will be NULL!!!
		if (!customerArr)
		{
			fclose(fp);
			return NULL;
		}

		for (int i = 0; i < customerCount; i++)
		{
			if (!loadCustomerFromFile(&customerArr[i], fp))
			{
				freeCustomerCloseFile(customerArr, i, fp);
				return NULL;
			}
		}
	}

	fclose(fp);
	*pCount = customerCount;
	return customerArr;
}

void freeCustomerCloseFile(Customer* customerArr, int count, FILE* fp)
{
	for (int i = 0; i < count; i++)
	{
		free(customerArr[i].name);
		customerArr[i].name = NULL;
		if (customerArr[i].pDerivedObj)
		{
			free(customerArr[i].pDerivedObj);
			customerArr[i].pDerivedObj = NULL;
		}
	}
	free(customerArr);
	fclose(fp);
}
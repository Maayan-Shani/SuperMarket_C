#pragma once

#include <stdio.h>
#include "Supermarket.h"
#include "Customer.h"

typedef unsigned char BYTE;

int		saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName);
int		loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName);
int		saveCustomersToTextFile(const Customer* customerArr, int customerCount,
	const char* customersFileName);
Customer* loadCustomersFromTextFile(const char* customersFileName, int* pCount);
void		freeCustomerCloseFile(Customer* customerArr, int customerIndex, FILE* fp);



int saveDataSuperMarketToBinaryFileCompressed(const SuperMarket* pMarket, char* fileName, const char* customersFileName);
int saveDate(const Product* product, FILE* fp);
int saveDataProductsCompressed(const SuperMarket* pMarket, FILE* fp);
int saveDataSuperMarketToBinaryFileCompressed(const SuperMarket* pMarket, char* fileName, const char* customersFileName);

int loadDateProductFromBinaryFileCompressed(FILE* fp, Date* date);
int loadProductPriceCountFromBinaryFileCompressed(FILE* fp, Product* product);
void loadBarcodeDigitsFromBinaryFileCompressed(FILE* fp, Product* product, BYTE* data, char* digits);
int loadProductsFromBinaryFileCompressed(SuperMarket* pMarket, FILE* fp);
int loadSuperMarketFromBinaryFileCompressed(SuperMarket* pMarket, const char* fileName, const char* customersFileName);
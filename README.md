# 🛒 SuperMarket Management System (C)

## 📌 Overview
This project implements a **SuperMarket Management System** written in **C** as part of the *Introduction to Systems Programming* course.

The program manages supermarket inventory, customers, and shopping carts through a **menu-driven terminal application**.

It demonstrates several important C programming concepts such as dynamic memory allocation, linked lists, function pointers, file handling (binary and text files), bit-level compression, and sorting/searching algorithms.

---

## 🚀 Features

### 🥦 Product Management
Each product in the supermarket contains:
- Name
- Barcode (2 letters + 5 digits)
- Product type (Fruit & Vegetables, Fridge, Frozen, Shelf)
- Price
- Quantity in stock
- Expiry date

Supported operations:
- Add new products
- Update product stock
- Print all products
- Print products by type
- Sort products using `qsort`
- Search products using `bsearch`

---

### 👥 Customer Management
Each customer contains:
- ID (9 digits)
- Dynamic name
- Shopping cart
- Optional **Club Membership**

Customers can:
- Register to the supermarket
- Perform shopping
- Manage their shopping cart
- Pay or cancel purchases

---

### 💎 Club Member Discounts
Customers can optionally be **Club Members**.  
Club members receive discounts based on membership duration.

| Membership Duration | Discount |
|---|---|
| Up to 2 years | 0.1% per month |
| 2–5 years | 2.5% + 0.5% per year |
| Over 5 years | 7.5% |

The project implements **polymorphism using virtual tables (`vTable`)** to support different behaviors between regular customers and club members.

---

### 🛍 Shopping Cart
The shopping cart is implemented using a **linked list** of shopping items.

Each shopping item contains:
- Product barcode
- Product price
- Quantity

Cart operations include:
- Adding products to the cart
- Updating quantities
- Printing cart contents
- Canceling purchases
- Completing payment

---

## 💾 File Storage
The system stores data using multiple file formats:

- **Binary file:** `SuperMarket.bin`
- **Compressed binary file:** `SuperMarket_Compress.bin`
- **Text file:** `Customers.txt`

The compressed binary file uses **bit manipulation techniques** to store supermarket data in a compact format.

---

## ⚙ Compilation
Compile the program using **gcc**:

```bash
gcc *.c -o supermarket

---

## ▶ Running the Program

Run the program with:
./supermarket <isCompressed> <fileName>

## Parameters
Parameter	Description
0	Use normal binary file
1	Use compressed binary file
---

🛠 Technologies Used
Technology	Purpose
C Programming Language	Main programming language
Linked Lists	Used for shopping cart implementation
Dynamic Memory Allocation	Managing memory with malloc/calloc/realloc
Function Pointers	Implementing polymorphism using vTable
Binary & Text File Handling	Saving and loading supermarket data
Bit Manipulation	Data compression in binary files
qsort / bsearch	Sorting and searching products

---

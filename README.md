# Cafe Management System

A console-based cafe management system written in **C** (about 3,400 lines, C11). Customers order drinks and get a receipt. Admins run the business side: menu, ingredient stock, recipes, discounts, sales analytics, finance reports and recommendations.

> School team project by [Ram Pharan](https://github.com/rampharan56-coder) and [Both-newbie](https://github.com/Both-newbie).

<!-- Add a screenshot or terminal recording here:
![Demo](docs/demo.png)
-->

## Features

**Customer**
- Browse the menu and order multiple items with quantities
- Choose sugar and ice level per item (none / less / normal / extra)
- Orders are checked against ingredient stock, and the customer is told which ingredients are missing if an order can't be made
- Discount code, or an automatic discount based on order size (the better of the two is applied)
- Printed receipt with total and savings

**Admin** (password protected, limited login attempts)
- Add, update and delete menu items
- View order history
- Sales analytics: monthly ranking and lifetime best sellers (monthly stats reset automatically each month)
- Ingredient inventory: add, update, delete and restock
- Recipe management: link each menu item to its ingredients and see the ingredient cost
- Financial report: cash balance, revenue, discounts given, ingredient cost, restocking expense, gross profit, margin, and per-item profit and ROI
- Business recommendations: which item to promote, which to reprice, low-stock alerts, slow sellers
- Discount code management

**General**
- All data is saved to text files in `data/`, so it persists between runs
- Input validation on every prompt (numbers in range, string length, yes/no)

## Quick start

You need a C compiler (GCC) and `make`.

```bash
git clone https://github.com/rampharan56-coder/Cafe_Management_System.git
cd Cafe_Management_System
make
./cafe            # on Windows: cafe.exe
```

Run it from the project root. A `data/` folder is created on first run to store the menu, orders, discounts and admin password.

```
************* WELCOME TO OUR CAFE STORE *************
=====================================================
[1]. ADMIN
[2]. CUSTOMER
[0]. EXIT
=====================================================
```

## Admin menu

```
[1]. DISPLAY ITEMS            [9].  UPDATE INGREDIENT
[2]. ADD ITEMS                [10]. DELETE INGREDIENT
[3]. UPDATE ITEMS             [11]. RECIPE MANAGEMENT
[4]. DELETE ITEMS             [12]. FINANCIAL REPORT
[5]. ORDER HISTORY            [13]. RESTOCK INGREDIENT
[6]. SALES ANALYTICS          [14]. BUSINESS RECOMMENDATIONS
[7]. VIEW INGREDIENTS         [15]. DISCOUNT MANAGEMENT
[8]. ADD INGREDIENT           [0].  BACK
```

## Project structure

```
.
├── Makefile
├── include/            # Header files, one per module
└── src/
    ├── main.c            # Entry point and main menu
    ├── customer.c        # Ordering flow
    ├── order.c           # Bill calculation, receipt, order history
    ├── menu.c            # Menu items (CRUD)
    ├── admin.c           # Admin login and admin menu
    ├── ingredient.c      # Inventory and restocking
    ├── recipe.c          # Menu item to ingredient mapping, cost
    ├── discount.c        # Promo codes and automatic discount tiers
    ├── finance.c         # Revenue, cost, profit, ROI
    ├── analytics.c       # Best sellers and monthly reset
    ├── recommendation.c  # Business recommendations
    ├── fileio.c          # Save/load to text files
    └── utils.c           # Safe input helpers
```

## How it works

- **Modules:** each feature has its own `.c`/`.h` pair, and shared state is exposed through the headers.
- **Persistence:** `fileio.c` saves and loads each data set (menu, orders, discounts, ingredients, statistics) as plain text.
- **Safe input:** `utils.c` wraps reading integers, floats, strings and yes/no answers with range checks, so bad input doesn't crash the program.
- **Compiler checks:** the build uses `-Wall -Wextra -pedantic -std=c11`, and the project compiles without warnings.

## Known limitations

- The admin password is stored in plain text in `data/admin.txt` (fine for a class project, not for production)
- Fixed-size arrays set the limits: 10 menu items, 20 orders, 10 items per order, 10 discount codes, 30 ingredients
- Data is stored in text files, not a database

## What I learned

- Splitting a C project into modules with headers and a Makefile
- Manual input validation and safe handling of user input in C
- File I/O for persistent storage
- Modelling a small business: inventory, recipes, pricing, discounts and profit

## Authors

- **Ram Pharan**: [@rampharan56-coder](https://github.com/rampharan56-coder)
- **Chon Panhaboth**: [@Both-newbie](https://github.com/Both-newbie)
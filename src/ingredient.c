/* === src/ingredient.c === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ingredient.h"
#include "utils.h"
#include "fileio.h"
#include "finance.h"

Ingredient ingredients[MAX_INGREDIENTS];
int ingredientCount = 0;
int nextIngredientID = 1;

static const char *DEFAULT_NAMES[] = {"Coffee Beans", "Milk", "Sugar",
                                        "Chocolate Syrup", "Vanilla Extract",
                                        "Matcha Powder", "Bread Flour", "Cream",
                                        "Ice"};
static const char *DEFAULT_UNITS[]  = {"g", "mL", "g", "mL", "mL", "g", "g", "mL", "piece"};
static const float DEFAULT_PRICES[] = {0.05f, 0.003f, 0.002f, 0.010f, 0.020f,
                                       0.080f, 0.004f, 0.008f, 0.010f};
static const int DEFAULT_STOCK[] = {5000, 10000, 5000, 3000, 1000, 1000, 10000, 3000, 5000};
static const int DEFAULT_MIN[] = {500, 1000, 500, 300, 100, 100, 1000, 300, 500};

void initializeIngredients(void)
{
    int count = (int)(sizeof(DEFAULT_NAMES) / sizeof(DEFAULT_NAMES[0]));
    for (int i = 0; i < count && i < MAX_INGREDIENTS; i++) {
        ingredients[i].ingredientID = nextIngredientID++;
        strncpy(ingredients[i].name, DEFAULT_NAMES[i], INGREDIENT_NAME_BUFFER - 1);
        ingredients[i].name[INGREDIENT_NAME_BUFFER - 1] = '\0';
        strncpy(ingredients[i].unit, DEFAULT_UNITS[i], sizeof(ingredients[i].unit) - 1);
        ingredients[i].unit[sizeof(ingredients[i].unit) - 1] = '\0';
        ingredients[i].purchasePrice = DEFAULT_PRICES[i];
        ingredients[i].currentStock = DEFAULT_STOCK[i];
        ingredients[i].minStock = DEFAULT_MIN[i];
        ingredientCount++;
    }
}

void viewIngredients(void)
{
    CLEAR_SCREEN();
    printf("============ INGREDIENTS ============\n");
    printf("=====================================\n");

    if (ingredientCount == 0) {
        printf("  (no ingredients registered)\n");
    } else {
        printf(" %-4s %-24s %-6s %-6s %-6s %-6s\n",
               "No.", "Name", "Unit", "Price", "Stock", "Min");
        printf("-------------------------------------\n");
        for (int i = 0; i < ingredientCount; i++) {
            printf(" [%02d] %-24s %-6s $%-5.2f %-6d %-6d\n",
                   i + 1,
                   ingredients[i].name,
                   ingredients[i].unit,
                   (double)ingredients[i].purchasePrice,
                   ingredients[i].currentStock,
                   ingredients[i].minStock);
        }
    }

    printf("=====================================\n");
}

void addIngredient(void)
{
    CLEAR_SCREEN();
    if (ingredientCount >= MAX_INGREDIENTS) {
        printf(" Error: Ingredient list is full (%d items)!\n", MAX_INGREDIENTS);
        waitKey(NULL);
        return;
    }

    Ingredient item;

    // Name input
    while (1) {
        readString("> enter ingredient name (0 to cancel): ", item.name, INGREDIENT_NAME_BUFFER);

        if (strcmp(item.name, "0") == 0) {
            printf("Canceled.\n");
            waitKey(NULL);
            return;
        }

        if (!isValidIngredientName(item.name)) {
            printf("  Error: Name must contain letters and spaces only!\n");
            continue;
        }

        // Duplicate check
        int dup = 0;
        for (int i = 0; i < ingredientCount; i++) {
            if (strcmp(ingredients[i].name, item.name) == 0) {
                printf("  Error: ingredient '%s' already exists.\n", item.name);
                dup = 1;
                break;
            }
        }
        if (!dup)
            break;
    }

    // Unit input
    while (1) {
        readString("> enter unit (g / mL / piece): ", item.unit, (int)sizeof(item.unit));
        if (isValidUnit(item.unit))
            break;
        printf("  Error: unit must be 'g', 'mL', or 'piece'.\n");
    }

    item.purchasePrice = readFloat("> enter purchase price per unit: $", 0.01f, 9999.99f);
    item.currentStock = readInt("> enter current stock: ", 0, 99999);
    item.minStock = readInt("> enter minimum stock: ", 0, 99999);
    item.ingredientID = nextIngredientID++;

    ingredients[ingredientCount] = item;
    ingredientCount++;

    saveIngredients("data/ingredients.txt");

    printf("\nIngredient '%s' added with ID %d!\n", item.name, item.ingredientID);
    waitKey(NULL);
}

void updateIngredient(void)
{
    CLEAR_SCREEN();
    viewIngredients();

    if (ingredientCount == 0) {
        waitKey(NULL);
        return;
    }

    int id = readInt("> enter ingredient ID to update (0 to cancel): ", 0, 99999);
    if (id == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    int index = findIngredientByID(id);
    if (index == -1) {
        printf("No ingredient with ID %d.\n", id);
        waitKey(NULL);
        return;
    }

    printf("\nCurrent Information\n");
    printf("-------------------\n");
    printf("ID    : %d\n", ingredients[index].ingredientID);
    printf("Name  : %s\n", ingredients[index].name);
    printf("Unit  : %s\n", ingredients[index].unit);
    printf("Price : $%.2f\n", (double)ingredients[index].purchasePrice);
    printf("Stock : %d\n", ingredients[index].currentStock);
    printf("Min   : %d\n", ingredients[index].minStock);

    // Update unit
    while (1) {
        readString("> enter new unit (g / mL / piece): ", ingredients[index].unit,
                   (int)sizeof(ingredients[index].unit));
        if (isValidUnit(ingredients[index].unit))
            break;
        printf("  Error: unit must be 'g', 'mL', or 'piece'.\n");
    }

    ingredients[index].purchasePrice = readFloat("> enter new purchase price: $", 0.01f, 9999.99f);
    ingredients[index].currentStock = readInt("> enter new current stock: ", 0, 99999);
    ingredients[index].minStock = readInt("> enter new minimum stock: ", 0, 99999);

    saveIngredients("data/ingredients.txt");

    printf("\nIngredient updated successfully!\n");
    waitKey(NULL);
}

void deleteIngredient(void)
{
    CLEAR_SCREEN();
    viewIngredients();

    if (ingredientCount == 0) {
        waitKey(NULL);
        return;
    }

    int id = readInt("> enter ingredient ID to delete (0 to cancel): ", 0, 99999);
    if (id == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    int index = findIngredientByID(id);
    if (index == -1) {
        printf("\nNo ingredient with ID %d.\n", id);
        waitKey(NULL);
        return;
    }

    int yn = readYesNo("Delete this ingredient? (Y/N): ");
    if (yn != 1) {
        if (yn == 0)
            printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    // Compact array by shifting left
    for (int i = index; i < ingredientCount - 1; i++)
        ingredients[i] = ingredients[i + 1];

    ingredientCount--;

    saveIngredients("data/ingredients.txt");

    printf("\nIngredient deleted successfully!\n");
    waitKey(NULL);
}

int findIngredientByID(int id)
{
    for (int i = 0; i < ingredientCount; i++) {
        if (ingredients[i].ingredientID == id)
            return i;
    }
    return -1;
}

int isValidIngredientName(const char *name)
{
    if (!name || name[0] == '\0')
        return 0;

    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) && !isspace((unsigned char)name[i]))
            return 0;
    }
    return 1;
}

int isValidUnit(const char *unit)
{
    return (strcmp(unit, "g") == 0 ||
            strcmp(unit, "mL") == 0 ||
            strcmp(unit, "piece") == 0);
}

void ingredient_reduceStock(int ingredientID, int quantity)
{
    int idx = findIngredientByID(ingredientID);
    if (idx == -1)
        return;

    ingredients[idx].currentStock -= quantity;
    if (ingredients[idx].currentStock < 0)
        ingredients[idx].currentStock = 0;

    saveIngredients("data/ingredients.txt");
}

int ingredient_hasStock(int ingredientID, int quantity)
{
    int idx = findIngredientByID(ingredientID);
    if (idx == -1)
        return 0;

    return ingredients[idx].currentStock >= quantity;
}

void restockIngredient(void)
{
    CLEAR_SCREEN();
    printf("========== RESTOCK INGREDIENT ==========\n");

    if (ingredientCount == 0) {
        printf("  No ingredients to restock.\n");
        waitKey(NULL);
        return;
    }

    viewIngredients();

    int id = readInt("\n> enter ingredient ID to restock (0 to cancel): ", 0, 99999);
    if (id == 0) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    int idx = findIngredientByID(id);
    if (idx == -1) {
        printf("  Error: No ingredient with ID %d.\n", id);
        waitKey(NULL);
        return;
    }

    printf("\nCurrent stock of %s: %d %s\n",
           ingredients[idx].name,
           ingredients[idx].currentStock,
           ingredients[idx].unit);
    printf("Purchase price: $%.4f per %s\n",
           (double)ingredients[idx].purchasePrice,
           ingredients[idx].unit);

    int qty = readInt("> enter quantity to add: ", 1, 99999);
    float cost = ingredients[idx].purchasePrice * qty;

    printf("\nCost: %d %s * $%.4f = $%.2f\n",
           qty, ingredients[idx].unit,
           (double)ingredients[idx].purchasePrice,
           (double)cost);

    int yn = readYesNo("Confirm restock? (Y/N): ");
    if (yn != 1) {
        printf("Canceled.\n");
        waitKey(NULL);
        return;
    }

    ingredients[idx].currentStock += qty;
    saveIngredients("data/ingredients.txt");

    finance_recordRestock(id, qty, cost);

    printf("\nRestocked %d %s of %s. New stock: %d %s\n",
           qty, ingredients[idx].unit,
           ingredients[idx].name,
           ingredients[idx].currentStock,
           ingredients[idx].unit);
    printf("Cost deducted from cash: $%.2f\n", (double)cost);
    waitKey(NULL);
}

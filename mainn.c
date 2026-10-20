#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

// Constants
#define MAX_MENU_ITEMS 10
#define MAX_ORDERS_ITEMS 10
#define MAX_ORDERS 20
#define NAME_BUFFER 50
#define PW_BUFFER 64

typedef struct
{
    int itemID;
    char itemName[NAME_BUFFER];
    float price;
} MenuItem;

typedef struct
{
    int itemID;
    int quantity;
} OrderItem;

typedef struct
{
    int orderID;
    OrderItem items[MAX_ORDERS_ITEMS];
    int itemCount;
    float totalBill;
} Order;


MenuItem menuItems[MAX_MENU_ITEMS];
Order orders[MAX_ORDERS];
int menuCount = 0;
int orderCount = 0;
int nextItem = 1;

void mainMenu();
void adminMenu();
void initializeMenu();
void addMenuItem();
void viewMenu();
void updateItem();
void deleteItem();
bool adminLogin();
void createOrder();
int findItemByID(int id);
float calculateBill(Order *order);
void printReceipt(Order *order);
void pauseScreen();
bool isValidName(const char *name);

int main()
{
    initializeMenu();
    mainMenu();

    return 0;
}
void mainMenu()
{

    while (1)
    {
        system("cls");
        int choice;
        printf("************* WECOME TO OUR CAFE STORE *************\n");
        printf("====================================================\n");
        printf("[1]. ADMIN\n");
        printf("[2]. CUSTOMER\n");
        printf("[0]. EXIT\n");
        printf("=====================================================\n");
        printf("> enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            if(adminLogin()){
            adminMenu();
            }
            else {
                printf("\nWrong Password!\n");
                system("pause");
            }
            break;
        case 2:
            createOrder();
            break;
        case 0:
            printf("=====================================================\n");
            printf("Thank you for visiting!\n");
            printf("=====================================================\n");

            exit(0);
        default:
            printf("\nInvalid choice. Please try again.\n");
            system("pause");
        }
    }
}

void initializeMenu()
{
    char *name[] = {"Americano", "Espresso", "Cappuccino", "Latte", "Mocha", "Matcha Latte", "Coffee Milk"};

    float price[] = {1.50, 1.25, 1.75, 1.75, 1.85, 2.00, 1.50};

    int count = (int)sizeof(name) / sizeof(name[0]);

    for (int i = 0; i < count && i < MAX_MENU_ITEMS; i++){
        menuItems[i].itemID = nextItem++;
        strncpy(menuItems[i].itemName, name[i], NAME_BUFFER - 1);
        menuItems[i].itemName[NAME_BUFFER - 1] = '\0';
        menuItems[i].price = price[i];
        menuCount++;
    }
}

void addMenuItem()
{
    system("cls");
    if (menuCount >= MAX_MENU_ITEMS)
    {
        printf(" Error: Menu is full (%d items)! Cannot add more items.\n", MAX_MENU_ITEMS);
        system("pause");
        return;
    }

    MenuItem *item = &menuItems[menuCount];

    do {
    printf("> enter item name(0 to cancel): ");
    scanf(" %49[^\n]", item->itemName);

    if (strcmp(item->itemName, "0") == 0)
    {
        printf("Canceled.\n");
        system("pause");
        return; 
    }
    if (!isValidName(item->itemName))
    {
        printf("Error: Name must contain letters only!\n");
    }
    } while (!isValidName(item->itemName));

        for (int i = 0; i < menuCount; i++)
    {
        if(strcmp(menuItems[i].itemName, item->itemName) == 0) {
            printf("Error: an item name '%s' already exist.\n", item->itemName);
            system("pause");
            return;
        }
    }

    printf("> enter price: $");
    scanf("%f", &item->price);

    item->itemID = nextItem++;
    menuCount++;

    printf("Item '%s' added with ID %d!.\n", item->itemName, item->itemID);
    system("pause");
}

void adminMenu()
{
    while (1)
    {
        system("cls");
        int adminChoice;
        printf("***************** THE ADMIN OFFICE ******************\n");
        printf("=====================================================\n");
        printf("[1]. DISPlAY ITEMS\n");
        printf("[2]. ADD ITEMS\n");
        printf("[3]. UPDATE ITEMS\n");
        printf("[4]. DELETE ITEMS\n");
        printf("[0]. BACK\n");
        printf("=====================================================\n");

        printf("> enter admin choice: ");
        scanf("%d", &adminChoice);

        switch (adminChoice)
        {
        case 0:
            return;
        case 1:
            viewMenu();
            system("pause");
            break;
        case 2:
            addMenuItem();
            break;
        case 3:
            viewMenu();
            updateItem();
            break;
        case 4:
            deleteItem();
            break;
        default:
            printf("\nInvalid choice\n");
            system("pause");
        }
    }
}

void viewMenu()
{
    system("cls");
    printf("************* MENU ITEMS *************\n");
    printf("=====================================\n");

    for (int i = 0; i < menuCount; i++)
    {
        printf("  [%d] %-24s  $%.2f\n",
               menuItems[i].itemID,
               menuItems[i].itemName,
               menuItems[i].price);
    }

    printf("=====================================\n");
}

void updateItem()
{
    system("cls");
    viewMenu();

    int id;
    printf("> enter item ID to update(0 to cancel): ");
    if(scanf("%d", &id) != 1 || id == 0) {
        printf("Canceled.\n");
        system("pause");
        return;
    }

    int index = findItemByID(id);
    if(index == -1) {
        printf("No item with ID %d.\n", id);
        system("pause");
        return;
    }

    printf("\nCurrent Information\n");
    printf("-------------------\n");
    printf("ID    : %d\n", menuItems[index].itemID);
    printf("Name  : %s\n", menuItems[index].itemName);
    printf("Price : $%.2f\n", menuItems[index].price);

    printf("> enter New Price: ");
    scanf("%f", &menuItems[index].price);

    printf("\nItem updated successfully!\n");

    system("pause");
}

void deleteItem()
{
    system("cls");
    viewMenu();

    int id;

    printf("> enter Item ID to delete(0 to cancel): ");
    if(scanf("%d", &id) != 1 || id == 0){
        printf("Cancelled.\n");
        system("pause");
        return;
    }

    int index = findItemByID(id);

    if (index == -1)
    {
        printf("\nNo item with ID %d.\n!", id);
        system("pause");
        return;
    }

    printf("\nDelete '%s' ? ([y]/[n]): ",
           menuItems[index].itemName);

    char confirm;
    scanf(" %c", &confirm);

    if (confirm != 'Y' && confirm != 'y')
    {
        return;
    }

    for (int i = index; i < menuCount - 1; i++)
    {
        menuItems[i] = menuItems[i + 1];
    }

    menuCount--;
    printf("\nItem deleted successfully!\n");
    system("pause");
}

bool adminLogin(){

    char password[PW_BUFFER];
    int attempts = 3;

    while (attempts > 0)
    {
        system("cls");

        printf("\n=== ADMIN LOGIN ===\n");
        printf("\n> enter password: ");
        scanf("%63s", password);

        if(strcmp(password, "Adminhere") == 0){
            printf("\nLogin Succesful\n");
            system("pause");
            return true;
        }

        attempts--;

        if(attempts > 0){
            printf("\nWrong Password. %d attempts left.\n", attempts);
            system("pause");
        }
    }

    printf("\nToo Many Failed Attempts!\n");
    system("pause");

    return false;
}

void createOrder()
{
    Order newOrder;

    newOrder.orderID = orderCount + 1;
    newOrder.itemCount = 0;
    newOrder.totalBill = 0;

    char choice;

    do
    {
        int id;
        int quantity;

        viewMenu();

        printf("> enter Item ID: ");
        scanf("%d", &id);

        if (findItemByID(id) == -1)
        {
            printf("Invalid Item ID!\n");
            system("pause");
            return;
        }

        printf("> enter Quantity: ");
        scanf("%d", &quantity);

        int index = newOrder.itemCount;

        newOrder.items[index].itemID = id;
        newOrder.items[index].quantity = quantity;

        newOrder.itemCount++;

        printf("\nAdd More Items? (Y/N): ");
        scanf(" %c", &choice);

    } while (choice == 'Y' || choice == 'y');

    calculateBill(&newOrder);

    orders[orderCount] = newOrder;
    orderCount++;

    printReceipt(&newOrder);

    system("pause");
}

int findItemByID(int id)
{
    for (int i = 0; i < menuCount; i++)
    {
        if (menuItems[i].itemID == id)
        {
            return i;
        }
    }

    return -1;
}

float calculateBill(Order *order)
{
    float total = 0;

    for (int i = 0; i < order->itemCount; i++)
    {
        int id = order->items[i].itemID;
        int quantity = order->items[i].quantity;

        int menuIndex = findItemByID(id);

        if (menuIndex != -1)
        {
            total += menuItems[menuIndex].price * quantity;
        }
    }

    order->totalBill = total;

    return total;
}

void printReceipt(Order *order)
{
    system("cls");
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timeBuffer[26];

    strftime(timeBuffer, sizeof(timeBuffer), "%d-%m-%Y %H:%M:%S", tm_info);

    printf("\n==========================================\n");
    printf("             * [RECEIPT] * \n");
    printf("==========================================\n");

    printf(" Date/Time: %s\n", timeBuffer);
    printf(" Order ID : #%04d\n", order->orderID);
    printf("------------------------------------------\n");
    printf(" %-22s %-5s %-10s\n", "ITEM", "QTY", "PRICE");
    printf("\n");
    for (int i = 0; i < order->itemCount; i++)
    {
        int itemID = order->items[i].itemID;
        int quantity = order->items[i].quantity;
        int menuIndex = findItemByID(itemID);

        if (menuIndex != -1)
        {
            float subtotal = menuItems[menuIndex].price * quantity;

            printf(" %-22s x%-4d $%-9.2f\n",
                   menuItems[menuIndex].itemName,
                   quantity,
                   subtotal);
        }
        else
        {
            printf(" %-22s x%-4d %-10s\n", "Unknown/Deleted Item", quantity, "[N/A]");
        }
    }

    printf("------------------------------------------\n");
    printf(" %-29s $%-9.2f\n", "TOTAL BILL:", order->totalBill);
    printf("==========================================\n");
    printf("        Thank you for your visit!         \n");
    printf("             Have a great day!            \n");
    printf("==========================================\n");

}

void pauseScreen()
{
    printf("Press Enter...");
    getchar();
}

bool isValidName(const char *name){
    if(strlen(name) == 0)
        return false;

    for (int i = 0; name[i] != '\0'; i++){
        if(!isalpha(name[i]) && !isspace(name[i])) {
            return false;
        }
    }
    return true;
}
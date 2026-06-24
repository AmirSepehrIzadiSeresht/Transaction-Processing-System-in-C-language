#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PATH "your_path"

typedef struct {
    int account;
    char lastName[15];
    char firstName[15];
    double balance;
} ClientData;

void textFile(FILE* readPtr);
void deleteRecord(FILE* fPtr);
void newRecord(FILE* fPtr);
void updateRecord(FILE* fPtr);
int enterChoice(void);
int getAcc(void);

int main() {
    FILE* cfPtr = NULL;

    cfPtr = fopen(PATH"accounts.dat", "rb+");
    if(cfPtr == NULL && errno == ENOENT) {
        cfPtr = fopen(PATH "accounts.dat", "wb+");

        if (cfPtr == NULL) {
            perror("create data file");
            return EXIT_FAILURE;
        }

        ClientData blankClient = {0, "", "",  0.0};

        for (int i = 1; i <= 100; ++i)
            fwrite(&blankClient, sizeof(ClientData), 1, cfPtr);
    }

    void (*options[4])(FILE*) =
        {textFile, updateRecord, newRecord, deleteRecord};

    int choice;
    while((choice = enterChoice()) != 5) {
        if(choice > 4 || choice < 1) {
            puts("wrong option number");
            continue;
        }

        options[--choice](cfPtr);
    }

    fclose(cfPtr);

    return EXIT_SUCCESS;
}

//create formated text file for printing
void textFile(FILE* readPtr) {
    FILE* writePtr = NULL;
    if ((writePtr = fopen(PATH"accounts.txt", "w")) == NULL) {
        puts("\"accounts.txt\" couldn't be opened");
        return;
    }

    rewind(readPtr);

    fprintf(
        writePtr,
        "%-6s%-16s%-11s%10s\n",
        "Acct", "Last Name", "First Name", "Balance"
    );

    while(!feof(readPtr)) {
        ClientData client = {0, "", "", 0};

        size_t result =
            fread(&client, sizeof(ClientData), 1, readPtr);

        if (result != 0 && client.account != 0) {
            fprintf(
                writePtr,
                "%-6d%-16s%-11s%10.2f\n",
                client.account, client.firstName, client.lastName, client.balance
            );
        }
    }

    fclose(writePtr);
}

//delete an existing record
void deleteRecord(FILE* fPtr) {
    puts("Enter an account to delete.");
    int acc = getAcc();

    fseek(fPtr, (acc-1)*sizeof(ClientData), SEEK_SET);
    
    ClientData client = {0, "", "", 0};
    
    fread(&client, sizeof(ClientData), 1, fPtr);
    
    if(client.account == 0) {
        printf("account #%d is already empty\n", acc);
        return;
    }
    
    client.account = 0;
    memcpy(client.firstName, "", 1);
    memcpy(client.lastName, "", 1);
    client.balance = 0;
    
    fseek(fPtr, (acc-1)*sizeof(ClientData), SEEK_SET);

    fwrite(&client, sizeof(ClientData), 1, fPtr);
}

//create and insert record
void newRecord(FILE* fPtr) {
    puts("Enter new account number.");

    int acc = getAcc();

    ClientData client = {0, "", "", 0};

    fseek(fPtr, (acc-1)*sizeof(ClientData), SEEK_SET);

    fread(&client, sizeof(ClientData), 1, fPtr);

    if (client.account != 0) {
        printf("account #%d already contains information\n", acc);
        return;
    }

    printf("%s", "Enter firstName: ");
    scanf("%14s", &client.firstName);
    printf("%s", "\nEnter lastname: ");
    scanf("%14s", &client.lastName);
    printf("%s", "\nEnter balance: ");
    scanf("%lf", &client.balance);
    client.account = acc;

    fseek(fPtr, (acc-1)*sizeof(ClientData), SEEK_SET);

    fwrite(&client, sizeof(ClientData), 1, fPtr);
}

// update balance in record
void updateRecord(FILE* fPtr) {
    int account = getAcc();

    fseek(fPtr, (account - 1)*sizeof(ClientData), SEEK_SET);

    ClientData client = {0, "", "", 0};

    fread(&client, sizeof(ClientData), 1, fPtr);

    if(0 == client.account) {
        printf("account #%d has no information\n", account);
        return;
    }

    printf(
        "%-6d%-16s%-11s%10.2f\n", 
        client.account, client.firstName, client.lastName, client.balance
    );

    puts("charge(+) | payment(-):\n>> ");
    double transaction =  0.0;
    scanf("%lf", &transaction);
    client.balance += transaction;

    printf(
        "%-6d%-16s%-11s%10.2f\n", 
        client.account, client.firstName, client.lastName, client.balance
    );

    fseek(fPtr, (account - 1)*sizeof(ClientData), SEEK_SET);

    fwrite(&client, sizeof(ClientData), 1, fPtr);
}

int getAcc(void) {
    int account;
    do {
        puts("Enter an account (1 - 100):\n>> ");
        scanf("%d", &account);
    } while (account < 1 || account > 100);

    return account;
}

int enterChoice(void) {
    printf(
        "%s",
        "\nEnter your choice:\n"
        "1. store accounts data in \"accounts.txt\" for printing\n"
        "2. upadate account\n"
        "3. add account\n"
        "4. delete an account\n"
        "5. exit program\n"
        ">> "
    );

    int menuChoice = 0;
    scanf("%d", &menuChoice);
    return menuChoice;
}

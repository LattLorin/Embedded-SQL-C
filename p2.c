#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlenv.h>
#include <ctype.h>

const char DB_NAME_KEY[16] = "db2.name";
const char DB_USER_KEY[16] = "db2.username";
const char DB_PASS_KEY[16] = "db2.password";
const char BATCH_KEY[16] = "p2.batch.input";
int currentid;

struct sqlca sqlca;

EXEC SQL BEGIN DECLARE SECTION;
  char db_name[20];
  char db_user[20];
  char db_pass[20];

  int cus_id;
  char cus_name[15];
  char cus_gender[1];
  int cus_age;
  int cus_pin;
  int acc_num;
  int acc_id;
  int acc_bal;
  char acc_type;
  char acc_status;
  int resultID;
  //newCustomer
  char nameInput[15];
  char genderInput;
  int ageInput;
  int pinInput;
  //openAccount
  int idInput;
  char typeInput[1];
  int amountInput;
  //closeAccoutn
  int closeAccountNum;
  //static void deposit(char* accNum, char* amount)
  int daccNum;
  int damount;
  //static void withdraw(char* accNum, char* amount)
  int waccNum;
  int wamount;
  //static void transfer(char* srcAccNum, char* destAccNum, char* amount)
  // reportA
  int validateID;
  int resultPIN;

  int src_cid;
  int dest_cid;
  int trans_amt;
  int a_total;
  int b_min;
  int b_max;
  int b_avg;
  //Report A
  char cA[1];
  char cI[1];
EXEC SQL END DECLARE SECTION;

static void open_db();
static void close_db();
void mainMenu();
void promptNewUser();
void customerMainMenu();

static int isActive(char* accNum){
  struct sqlca sqlca;
  acc_num = atoi(accNum);
  EXEC SQL DECLARE check CURSOR FOR SELECT STATUS FROM P2.ACCOUNT WHERE NUMBER = :acc_num;
  EXEC SQL OPEN check;
  EXEC SQL FETCH check INTO :acc_status;
  if (sqlca.sqlcode != 100){
    if(acc_status == 'A')  {
      close_db();
      open_db();
      return 1;
    }
    else {
      close_db();
      open_db();
      return 0;
    }
  }
  else {
    close_db();
    open_db();
    return 0;
  }
}

/**
 * Create a new customer.
 * @param name customer name
 * @param gender customer gender
 * @param age customer age
 * @param pin customer pin
 */
static void newCustomer(char* name, char* gender, char* age, char* pin)
{
  struct sqlca sqlca;
  printf(":: CREATE NEW CUSTOMER - RUNNING\n");
  strcpy(nameInput,name);
  genderInput = *gender;
  ageInput = atoi(age);
  pinInput = atoi(pin);
  EXEC SQL DECLARE n1 CURSOR FOR SELECT ID FROM FINAL TABLE(INSERT INTO P2.CUSTOMER(NAME, GENDER, AGE, PIN) VALUES (:nameInput,:genderInput,:ageInput,:pinInput));
  EXEC SQL OPEN n1;
  EXEC SQL FETCH n1 INTO :resultID;
  if(sqlca.sqlcode == -545){
      printf("One of your inputs does not satisfy the constraints of the database\n" );
  }
   else if (sqlca.sqlcode == 100)
   {
     printf(":: ERROR - SELECTING ID\n");
   }
   else if (sqlca.sqlcode != 100)
   {
     printf(":: NEW CUSTOMER ID: %d\n", resultID);
     printf(":: CREATE NEW CUSTOMER - SUCCESS\n");
   }
  EXEC SQL CLOSE n1;
  close_db();
  open_db();
}

static void validateCustomer(char* id, char* pin){
   struct sqlca sqlca;
   printf(":: VALIDATE CUSTOMER - RUNNING\n");
   validateID = atoi(id);
   int validatePIN = atoi(pin);
   EXEC SQL DECLARE v1 CURSOR FOR SELECT PIN FROM P2.CUSTOMER WHERE ID = :validateID;
   EXEC SQL OPEN v1;
   EXEC SQL FETCH v1 INTO :resultPIN;
   if (sqlca.sqlcode == 100)
   {
     printf(":: ERROR - No account is found\n");
   }
   if (sqlca.sqlcode != 100)
   {
      if(resultPIN != validatePIN){
          printf(":: ERROR - UNMATCHED PIN\n");
          EXEC SQL CLOSE v1;
          mainMenu();
      }
      else
      {
        currentid = atoi(id);
        printf(":: VALIDATE CUSTOMER - SUCCESS\n");
        EXEC SQL CLOSE v1;
        customerMainMenu();
      }
   }
   EXEC SQL CLOSE v1;
   close_db();
   open_db();
}
/**
 * Open a new account.
 * @param id customer id
 * @param type type of account
 * @param amount initial deposit amount
 */
static void openAccount(char* id, char* type, char* amount)
{
  printf(":: OPEN ACCOUNT - RUNNING\n");
  idInput = atoi(id);
  strcpy(typeInput, type);
  amountInput = atoi(amount);
  strcpy(cA, "A");
  //printf("INSERT INTO P2.ACCOUNT(ID, BALANCE, TYPE, STATUS) VALUES ('%d','%s','%d','%s')", idInput, typeInput, amountInput, cA);
  EXEC SQL DECLARE open CURSOR FOR SELECT NUMBER FROM FINAL TABLE(INSERT INTO P2.ACCOUNT(ID, BALANCE, TYPE, STATUS) VALUES (:idInput, :amountInput, :typeInput,:cA));
  EXEC SQL OPEN open;
  EXEC SQL FETCH open INTO :acc_num;
  if (sqlca.sqlcode == -501){
    printf(":: ERROR - ID does not existed in the customer table\n");
  }
  else if (sqlca.sqlcode == -545){
    printf(":: ERROR - One of your inputs does not satisfy the constraints of the database.\n");
  }
  else if (sqlca.sqlcode != 100)
  {
    printf(":: NEW ACCOUNT NUMBER: %d\n", acc_num);
    printf(":: OPEN ACCOUNT - SUCCESS\n");
  }
  EXEC SQL CLOSE open;
  close_db();
  open_db();
}

/**
 * Close an account.
 * @param accNum account number
 */
static void closeAccount(char* accNum)
{
  printf(":: CLOSE ACCOUNT - RUNNING\n");
  if(isActive(accNum)){
    closeAccountNum = atoi(accNum);
    strcpy(cI, "I");
    EXEC SQL UPDATE P2.ACCOUNT SET STATUS = :cI, BALANCE = 0 WHERE NUMBER = :closeAccountNum;
    printf(":: CLOSE ACCOUNT - SUCCESS\n");
  }
  else printf(":: ERROR - ACCOUNT IS NOT ACTIVE\n");
  close_db();
  open_db();
}

/**
 * Deposit into an account.
 * @param accNum account number
 * @param amount deposit amount
 */
static void deposit(char* accNum, char* amount)
{
  printf(":: DEPOSIT - RUNNING\n");
  if(isActive(accNum)){
  daccNum = atoi(accNum);
  damount = atoi(amount);
  EXEC SQL UPDATE P2.ACCOUNT SET BALANCE = BALANCE + :damount WHERE NUMBER = :daccNum;
  printf(":: DEPOSIT - SUCCESS\n");
  }
  else printf(":: ERROR - ACCOUNT IS NOT ACTIVE\n");
  close_db();
  open_db();
}

/**
 * Withdraw from an account.
 * @param accNum account number
 * @param amount withdraw amount
 */
static void withdraw(char* accNum, char* amount)
{
  printf(":: WITHDRAW - RUNNING\n");
  if(isActive(accNum)){
  waccNum = atoi(accNum);
  wamount = atoi(amount);
  EXEC SQL UPDATE P2.ACCOUNT SET BALANCE = BALANCE + :wamount WHERE NUMBER = :waccNum;
  printf(":: WITHDRAW - SUCCESS\n");
  }
  else printf(":: ERROR - ACCOUNT IS NOT ACTIVE\n");
  close_db();
  open_db();
}

/**
 * Transfer amount from source account to destination account.
 * @param srcAccNum source account number
 * @param destAccNum destination account number
 * @param amount transfer amount
 */
static void transfer(char* srcAccNum, char* destAccNum, char* amount)
{
  printf(":: TRANSFER - RUNNING\n");
  if(isActive(srcAccNum) && isActive(destAccNum)){
  src_cid = atoi(srcAccNum);
  dest_cid = atoi(destAccNum);
  trans_amt = atoi(amount);
  EXEC SQL UPDATE P2.ACCOUNT SET BALANCE = BALANCE - :trans_amt WHERE NUMBER = :src_cid;
  EXEC SQL UPDATE P2.ACCOUNT SET BALANCE = BALANCE + :trans_amt WHERE NUMBER = :dest_cid;
  printf(":: TRANSFER - SUCCESS\n");
  }
  else printf(":: ERROR - ONE OF THE ACCOUNTS IS NOT ACTIVE\n");
  close_db();
  open_db();
}

/**
 * Display account summary.
 * @param cusID customer ID
 */
static void accountSummary(char* cusID)
{
  struct sqlca sqlca;
  printf(":: ACCOUNT SUMMARY - RUNNING\n");
  acc_id = atoi(cusID);
  EXEC SQL DECLARE c1 CURSOR FOR SELECT NUMBER, BALANCE FROM P2.ACCOUNT WHERE ID = :acc_id AND STATUS = :cA;
  EXEC SQL OPEN c1;
  EXEC SQL FETCH c1 INTO :acc_num, :acc_bal;
  int total = 0;
  printf(" %-10s","NUMBER");
	printf(" %-10s\n","BALANCE");
	printf(" %-10s","----------");
	printf(" %-10s\n","----------");
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  while (sqlca.sqlcode != 100)
  {
    printf(" %10d",acc_num);
    printf(" %10d\n",acc_bal);
    total = total + acc_bal;
    EXEC SQL FETCH c1 INTO :acc_num, :acc_bal;
    /* EMB_SQL_CHECK("Cursor -- Fetch"); */
  }
  printf(" %-20s\n","---------------------");
  printf(" %-10s %10d\n","TOTAL", total);
  printf(":: ACCOUNT SUMMARY - SUCCESS\n");
  EXEC SQL CLOSE c1;
  close_db();
  open_db();
}

/**
 * Display Report A - Customer Information with Total Balance in Decreasing Order.
 */
static void reportA()
{
  struct sqlca sqlca;
  printf(":: REPORT A - RUNNING\n");
  EXEC SQL DECLARE c2 CURSOR FOR SELECT B.ID, NAME, GENDER, AGE, TOTAL FROM P2.CUSTOMER C, P2.TOTAL_BALANCE B WHERE C.ID = B.ID ORDER BY TOTAL DESC;
  EXEC SQL OPEN c2;
  EXEC SQL FETCH c2 INTO :cus_id, :cus_name, :cus_gender, :cus_age, :a_total;
  printf(" %-10s","ID");
  printf(" %-10s","NAME");
  printf(" %-10s","GENDER");
  printf(" %-10s","AGE");
  printf(" %-10s\n","TOTAL");
  printf(" %-10s","----------");
  printf(" %-10s","----------");
  printf(" %-10s","----------");
  printf(" %-10s","----------");
  printf(" %-10s\n","----------");
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  while (sqlca.sqlcode != 100)
  {
    printf(" %10d %-10s %-10s %10d %10d\n", cus_id, cus_name, cus_gender, cus_age, a_total);
    EXEC SQL FETCH c2 INTO :cus_id, :cus_name, :cus_gender, :cus_age, :a_total;
    /* EMB_SQL_CHECK("Cursor -- Fetch"); */
  }
  EXEC SQL CLOSE c2;
  printf(":: REPORT A - SUCCESS\n");
  close_db();
  open_db();
}
/**
 * Display Report B - Customer Information with Total Balance in Decreasing Order.
 * @param min minimum age
 * @param max maximum age
 */
static void reportB(char* min, char* max)
{
  printf(":: REPORT B - RUNNING\n");
  b_min = atoi(min);
  b_max = atoi(max);
  EXEC SQL SELECT AVG(TOTAL) AS AVERAGE INTO :b_avg FROM P2.TOTAL_BALANCE B, P2.CUSTOMER C WHERE C.ID = B.ID AND (C.AGE >= :b_min AND C.AGE <= :b_max);
  printf(" %-10s\n","AVERAGE");
	printf(" %-10s\n","----------");
  printf("%10d\n", b_avg);
  printf(":: REPORT B - SUCCESS\n");
  close_db();
  open_db();
}

/**
 * Retrieve database connection info from properties file.
 * @param filename name of properties file
 */
static void init(char filename[])
{
  // open file
  FILE *fp = fopen(filename, "r");
  // print error if file cannot be opened
  if (fp == NULL)
  {
    printf("Cannot open properties file %s\n", filename);
  }
  else
  {
    char *cur_line = (char *) malloc(256);
    // read each line
    while ( fscanf(fp, "%s", cur_line) != EOF)
    {
      char *line_copy = (char *) malloc(256);
      char *token;
      // copy current line
      strcpy(line_copy, cur_line);
      // tokenize line and save connection values
      while ( (token = strsep(&line_copy, "=")) != NULL )
      {
        if ( strcmp(token, DB_NAME_KEY) == 0)
        {
          token = strsep(&line_copy, "=");
          strcpy(db_name, token);
        }
        else if ( strcmp(token, DB_USER_KEY) == 0)
        {
          token = strsep(&line_copy, "=");
          strcpy(db_user, token);
        }
        else if ( strcmp(token, DB_PASS_KEY) == 0)
        {
          token = strsep(&line_copy, "=");
          strcpy(db_pass, token);
        }
        else
        {
          return;
        }
      }
    }
  }
  // close file
  fclose(fp);
}


/**
 * Open database connection.
 */
static void open_db()
{
  EXEC SQL CONNECT TO :db_name USER :db_user USING :db_pass;
}

/**
 * Close database connection.
 */
static void close_db()
{
  EXEC SQL CONNECT RESET;
}

/**
 * Test database connection.
 */
static void test_connection()
{
  printf(":: TEST - CONNECTING TO DATABASE\n");
  open_db();
  // check returned sqlcode
  if (sqlca.sqlcode == 0)
  {
    printf(":: TEST - SUCCESSFULLY CONNECTED TO DATABASE\n");
    close_db();
    printf(":: TEST - DISCONNECTED FROM DATABASE\n");
  }
  else
  {
    printf(":: TEST - FAILED CONNECTED TO DATABASE\n");
    printf(":::: ERROR SQLCODE %i\n", sqlca.sqlcode);
  }
}

/**
 * Execute function from batch input.
 * @param arr array of strings
 * @param size length of p array
 */
void batch_execute(char** arr, int size)
{
  if (size < 0)
  {
    printf("Invalid parameter count");
  }
  else
  {
    printf("\n");
    if (strcmp(arr[0], "#newCustomer") == 0)
    {
      newCustomer(arr[1], arr[2], arr[3], arr[4]);
    }
    else if (strcmp(arr[0], "#openAccount") == 0)
    {
      openAccount(arr[1], arr[2], arr[3]);
    }
    else if (strcmp(arr[0], "#closeAccount") == 0)
    {
      closeAccount(arr[1]);
    }
    else if (strcmp(arr[0], "#deposit") == 0)
    {
      deposit(arr[1], arr[2]);
    }
    else if (strcmp(arr[0], "#withdraw") == 0)
    {
      withdraw(arr[1], arr[2]);
    }
    else if (strcmp(arr[0], "#transfer") == 0)
    {
      transfer(arr[1], arr[2], arr[3]);
    }
    else if (strcmp(arr[0], "#accountSummary") == 0)
    {
      accountSummary(arr[1]);
    }
    else if (strcmp(arr[0], "#reportA") == 0)
    {
      reportA();
    }
    else if (strcmp(arr[0], "#reportB") == 0)
    {
      reportB(arr[1], arr[2]);
    }
    else
    {
      printf("Could not find function to batch_execute: %s", arr[0]);
    }
    printf("\n");
  }
}

/**
 * Run batch input given filename.
 * @param filename name of properties file
 */
static void batch_run(char filename[])
{
  // open file
  FILE *fp = fopen(filename, "r");
  // print error if file cannot be opened
  if (fp == NULL)
  {
    printf("Cannot open properties file %s\n", filename);
  }
  else
  {
    int batch_mode = 0;
    char *cur_line = (char *) malloc(256);
    // read each line
    while ( fscanf(fp, "%s", cur_line) != EOF)
    {
      char *line_copy = (char *) malloc(256);
      char *token;
      // copy current line
      strcpy(line_copy, cur_line);
      if (batch_mode == 0)
      {
        // tokenize line and find batch key
        while ( (token = strsep(&line_copy, "=")) != NULL )
        {
          if ( strcmp(token, BATCH_KEY) == 0)
          {
            batch_mode = 1;
          }
          else
          {
            break;
          }
        }
      }
      else if (batch_mode == 1)
      {
        char *token;
        char **token_arr = (char **) malloc(sizeof(char*) * 1);
        int token_cnt = 0;
        // get each token and save to array
        while ( (token = strsep(&line_copy, ",")) != NULL )
        {
          token_arr = (char **) realloc(token_arr, (token_cnt + 1) * sizeof(char*));
          token_arr[token_cnt] = (char *) malloc( strlen(token) + 1 );
          strcpy(token_arr[token_cnt], token);
          token_cnt++;
        }
        // run function
        batch_execute(token_arr, token_cnt);
      }
    }
  }
  // close file
  fclose(fp);
}
int checkDigit(char* input){
   const int len = strlen(input);
   for(int i = 0; i < len; i = i + 1){
       if(!isdigit(input[i])) return 0;
   }
   return 1;
}

int checkLetter(char* input){
  const int len = strlen(input);
  for(int i = 0; i < len; i = i + 1){
      if(!isalpha(input[i])) return 0;
  }
  return 1;
}
void adminMainMenu(){
  printf("--------------------------------------------------\nAdministrator Main Menu\n");
  printf("1. Account Summary for a Customer\n2. Report A :: Customer Information with Total Balance in Decreasing Order\n3. Report B :: Find the Average Total Balance Between Age Groups\n4. Exit\n");
}
void customerMainMenu(){
  int option;
  char id[3];
  char type;
  char depositAmount[10];
  char accNum[4];
  char accNumDes[4];
  printf("--------------------------------------------------\nCustomer Main Menu:\n1.Open Account\n2.Close Account\n3.Deposit\n4.Withdraw\n5.Transfer\n6.Account Summary\n7.Exit\n");
  fscanf(stdin,"%d", &option);
  switch (option) {
    case 1: {
      printf("--------------------------------------------------\nOPEN NEW ACCOUNT\n");
      printf("ID: \n");
      scanf("%s", id);
      while (getchar() != '\n');
      printf("Account Type (C for Checking OR S for Saving)\n");
      scanf("%c", &type);
      while (getchar() != '\n');
      printf("Initial Deposit Amount\n");
      scanf("%s", depositAmount);
      while (getchar() != '\n');
      printf("COMPARE RESULT: %d\n", atoi(id) != currentid);
      if(checkDigit(id) == 0 || strlen(id) != 3){
          printf("ID needs to be 3 digits\n");
      }
      else if(type != 'C' && type != 'S'){
          printf("Gender needs to be either C or S\n");
      }
      else if(checkDigit(depositAmount) == 0){
          printf("Deposit must be a number\n");
      }
      else {
          openAccount(id, &type, depositAmount);
      }
      customerMainMenu();
      break;
    }
    case 2: {
      printf("--------------------------------------------------\nCLOSE ACCOUNT\n");
      printf("Account Number: \n");
      scanf("%s", accNum);
      if(checkDigit(accNum) == 0 || strlen(accNum) != 4){
          printf("Please enter 4 digits for account number\n");
      }
      else {
        closeAccount(accNum);
      }
      customerMainMenu();
      break;
    }
    case 3: {
      printf("--------------------------------------------------\nDEPOSIT MONEY\n");
      printf("Account Number: \n");
      scanf("%s", accNum);\
      printf("Deposit Amount: \n");
      scanf("%s", depositAmount);
      if(checkDigit(accNum) == 0 || strlen(accNum) != 4){
          printf("Please enter 4 digits for account number\n");
      }
      else if(checkDigit(depositAmount) == 0){
          printf("Please enter digits for deposit account\n");
      }
      else {
          deposit(accNum, depositAmount);
      }
      customerMainMenu();
      break;
    }
    case 4: {
      printf("--------------------------------------------------\nWITHDRAW MONEY\n");
      printf("Account Number: \n");
      scanf("%s", accNum);\
      printf("Withdraw Amount: \n");
      scanf("%s", depositAmount);
      if(checkDigit(accNum) == 0 || strlen(accNum) != 4){
          printf("Please enter 4 digits for account number\n");
      }
      else if(checkDigit(depositAmount) == 0){
          printf("Please enter digits for withdraw account\n");
      }
      else {
          withdraw(accNum, depositAmount);
      }
      customerMainMenu();
      break;
    }
    case 5: {
      printf("--------------------------------------------------\nTRANSFER MONEY\n");
      printf("Source Account Number: \n");
      scanf("%s", accNum);\
      printf("Source Destination Number: \n");
      scanf("%s", accNumDes);\
      printf("Transfer Amount: \n");
      scanf("%s", depositAmount);
      if(checkDigit(accNum) == 0 || strlen(accNum) != 4 || checkDigit(accNumDes) == 0 || strlen(accNumDes) != 4 ){
          printf("Please enter 4 digits for account number\n");
      }
      else if(checkDigit(depositAmount) == 0){
          printf("Please enter digits for withdraw account\n");
      }
      else {
          transfer(accNum, accNumDes, depositAmount);
      }
      customerMainMenu();
      break;
    }
    case 6: {
      printf("--------------------------------------------------\nACCOUNT SUMMARY FOR CUSTOMER ID: %d\n", currentid );
      char id[3];
      sprintf(id, "%d", currentid);
      accountSummary(id);
      customerMainMenu();
      break;
    }
    case 7:
    {
      printf("Entered 7\n");
      mainMenu();
      break;
    }
    default:{
      customerMainMenu();
      break;
    }
  }
}
void promptLogIn(){
    char pin[4];
    char id[3];
    printf("--------------------------------------------------\nLOG IN PAGE\nPlease enter your ID and PIN to sign in \nID (3 digits)\n");
    scanf("%s", id);
    while (getchar() != '\n');
    printf("PIN\n");
    scanf("%s", pin);
    while (getchar() != '\n');
    if(atoi(id) == 0 && atoi(pin) == 0){
       adminMainMenu();
    }
    else if(checkDigit(id) == 0 || strlen(id) != 3){
        printf("ID needs to be 3 digits\n");
        mainMenu();
    }
    else if(checkDigit(pin) == 0 || strlen(pin) != 4){
      printf("PIN needs to be 4 digits\n");
      mainMenu();
    }
    else {
      validateCustomer(id, pin);
      mainMenu();
    }


}
void promptNewUser(){
   char name[15];
   char gender;
   char age[4];
   char pin[4];
   printf("--------------------------------------------------\nSIGN UP PAGE\nName\n");
   scanf("%s", name);
   while (getchar() != '\n');
   printf("Gender\n");
   scanf("%c", &gender);
   while (getchar() != '\n');
   printf("Age\n");
   scanf("%s",age);
   while (getchar() != '\n');
   printf("PIN\n");
   scanf("%s", pin);
   while (getchar() != '\n');
   if(name == NULL || age == NULL || pin == NULL || name[0] == '\n' || age[0] == '\n'|| pin[0] == '\n') {
     printf(":: ERROR - One of the required inputs is empty. Please try again.\n");
     mainMenu();
   }
   else if(gender != 'M' && gender != 'F'){
     printf("Please enter either F or M for Gender\n");
     mainMenu();
   }
   else if(checkDigit(age) == 0 || strlen(age) > 4){
      printf("Please enter digits for age\n");
      mainMenu();
   }
   else if(checkDigit(age) == 0 || strlen(pin) != 4){
      printf("Please enter digits for pin\n" );
      mainMenu();
   }
   else{
      newCustomer(name, &gender, age, pin);
      mainMenu();
   }
}
void mainMenu(){
  int a;
  printf("--------------------------------------------------\n:: Welcome to the Self Services Banking System! – Main Menu\n");
  printf("1.New Customer\n2.Customer Login\n3.Exit\n");
  fscanf(stdin,"%d", &a);
  while (getchar() != '\n');
  if(a == 1){
      promptNewUser();
      mainMenu();
  }
  else if(a == 2){
     promptLogIn();
     mainMenu();
  }
  else if(a == 3){
     exit(0);
  }
  else{
    printf("Please enter one digit to select the following options\n");
    mainMenu();
  }

}
void start(){
  mainMenu();
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Need properties file\n");
  }
  else
  {
    init(argv[1]);
    test_connection();
    open_db();
    start();
    //batch_run(argv[1]);
    close_db();
  }
  return 0;
}

//remember to openDB and closeDB

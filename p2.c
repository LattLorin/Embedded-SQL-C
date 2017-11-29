  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sqlenv.h>

  const char DB_NAME_KEY[16] = "db2.name";
  const char DB_USER_KEY[16] = "db2.username";
  const char DB_PASS_KEY[16] = "db2.password";
  const char BATCH_KEY[16] = "p2.batch.input";

  struct sqlca sqlca;

  EXEC SQL BEGIN DECLARE SECTION;
    char db_name[20];
    char db_user[20];
    char db_pass[20];

    int cus_id;
    char cus_name[15];
    char cus_gender;
    int cus_age;
    int cus_pin;
    int acc_num;
    int acc_id;
    int acc_bal;
    char acc_type;
    char acc_status; 
    //newCustomer
    char nameInput[20];
    char genderInput[20];
    char ageInput[20];
    char pinInput[20];
    //openAccount
    int idInput;
    char typeInput[1];
    int amountInput;
    //closeAccoutn
    char closeAccountNum[4];
    //static void deposit(char* accNum, char* amount)
    char daccNum[4];
    char damount[20];
    //static void withdraw(char* accNum, char* amount)
    char waccNum[4];
    char wamount[20];
    //static void transfer(char* srcAccNum, char* destAccNum, char* amount)
    char tsrcAccNum[4];
    char tdestAccNum[4];
    char tamount[20];
    //accountSummary
    char cusIDSummary[4];
    //static void reportB(char* min, char* max)
    char reportMin[2];
    char reportMax[2];
    // reportA 

    int src_cid;
    int dest_cid;
    int trans_amt;
    int a_total;
    int b_min;
    int b_max;
    int b_avg;

    int accountNum;
    int balance;
    //Report A
    int id;
    char name[10];
    char gender[1];
    int age;
    int total;
    int average;

    char cA[1];
    char cI[1];
  EXEC SQL END DECLARE SECTION;

  static void open_db();
  static void close_db();
  
/**
   * Create a new customer.
   * @param name customer name
   * @param gender customer gender
   * @param age customer age
   * @param pin customer pin
   */
  static void newCustomer(char* name, char* gender, char* age, char* pin)
  {
    printf(":: CREATE NEW CUSTOMER - RUNNING\n"); 
    strcpy(nameInput,name);
    strcpy(genderInput, gender);
    strcpy(ageInput, age);
    strcpy(pinInput, pin);
    EXEC SQL INSERT INTO P2.CUSTOMER(NAME, GENDER, AGE, PIN) VALUES (:nameInput,:genderInput,:ageInput,:pinInput); 
    close_db();
    open_db(); 
    printf(":: CREATE NEW CUSTOMER - SUCCESS\n"); 
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
    printf("%d",idInput);
    printf("%s",typeInput);
    printf("%d",amountInput);
    printf("%s",cA);
    //printf("INSERT INTO P2.ACCOUNT(ID, BALANCE, TYPE, STATUS) VALUES ('%d','%s','%d','%s')", idInput, typeInput, amountInput, cA);
    EXEC SQL INSERT INTO P2.ACCOUNT(ID, BALANCE, TYPE, STATUS) VALUES (:idInput, :amountInput, :typeInput,:cA);   
    close_db();
    open_db();   
    printf(":: OPEN ACCOUNT - SUCCESS\n");
}

  /**
   * Close an account.
   * @param accNum account number
   */
  static void closeAccount(char* accNum)
  {
    printf(":: CLOSE ACCOUNT - RUNNING\n");
    strcpy(closeAccountNum, accNum);
    printf("%s",closeAccountNum);
    strcpy(cI, "I");
    printf("%s",cI);
    EXEC SQL UPDATE P2.ACCOUNT SET STATUS = :cI WHERE NUMBER = :closeAccountNum;
    printf(":: CLOSE ACCOUNT - SUCCESS\n");
  }

  /**
   * Deposit into an account.
   * @param accNum account number
   * @param amount deposit amount
   */
  static void deposit(char* accNum, char* amount)
  {
    printf(":: DEPOSIT - RUNNING\n");
    strcpy(daccNum, accNum);
    strcpy(damount, amount);
    EXEC SQL UPDATE P2.ACCOUNT SET BALANCE = BALANCE + :damount WHERE NUMBER = :daccNum;
    printf(":: DEPOSIT - SUCCESS\n");
  }

  /**
   * Withdraw from an account.
   * @param accNum account number
   * @param amount withdraw amount
   */
  static void withdraw(char* accNum, char* amount)
  {
    printf(":: WITHDRAW - RUNNING\n");
    strcpy(waccNum, accNum);
    strcpy(wamount, amount);
    EXEC SQL UPDATE P2.ACCOUNT SET BALANCE = BALANCE + :wamount WHERE NUMBER = :waccNum;
    printf(":: WITHDRAW - SUCCESS\n");
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
    strcpy(tsrcAccNum, srcAccNum);
    strcpy(tdestAccNum, destAccNum);
    strcpy(tamount, amount);
    EXEC SQL UPDATE P2.ACCOUNT SET BALANCE = BALANCE - :tamount WHERE NUMBER = :tsrcAccNum;
    EXEC SQL UPDATE P2.ACCOUNT SET BALANCE = BALANCE + :tamount WHERE NUMBER = :tdestAccNum; 
    printf(":: TRANSFER - SUCCESS\n");

}

  /**
   * Display account summary.
   * @param cusID customer ID
   */
  static void accountSummary(char* cusID)
  {
    printf(":: ACCOUNT SUMMARY - RUNNING\n");
    strcpy(cusIDSummary, cusID);
    EXEC SQL DECLARE c1 CURSOR FOR SELECT NUMBER, BALANCE FROM P2.ACCOUNT WHERE ID = :cusIDSummary;
    EXEC SQL OPEN c1;
    EXEC SQL FETCH c1 INTO :accountNum, :balance;
    printf(":: ACCOUNT SUMMARY - SUCCESS\n");
  }

  /**
   * Display Report A - Customer Information with Total Balance in Decreasing Order.
   */
  static void reportA()
  {
    struct sqlca sqlca;

    printf(":: REPORT A - RUNNING\n");
    EXEC SQL DECLARE c2 CURSOR FOR SELECT B.ID, NAME, GENDER, AGE, TOTAL FROM P1.CUSTOMER C, P1.TOTAL_BALANCE B WHERE C.ID = B.ID ORDER BY TOTAL DESC;
    EXEC SQL OPEN c2;
    EXEC SQL FETCH c2 INTO :id, :name, :gender, :age, :total;
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
      printf("%10d%-10s%-10s%10d%10d\n", id, name, gender, age, total);
      EXEC SQL FETCH c2 INTO :id, :name, :gender, :age, :total;
      /* EMB_SQL_CHECK("Cursor -- Fetch"); */
    }

    EXEC SQL CLOSE c2;
    printf(":: REPORT A - SUCCESS\n");

  }
  /**
   * Display Report B - Customer Information with Total Balance in Decreasing Order.
   * @param min minimum age
   * @param max maximum age
   */
  static void reportB(char* min, char* max)
  {
    printf(":: REPORT B - RUNNING\n");
    strcpy(reportMin, min);
    strcpy(reportMax, max);
    EXEC SQL SELECT AVG(TOTAL) AS AVERAGE INTO :average FROM P1.TOTAL_BALANCE B, P1.CUSTOMER C WHERE C.ID = B.ID AND (C.AGE >= :reportMin AND C.AGE <= :reportMax);
    printf(":: REPORT B - SUCCESS\n"); 
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
      batch_run(argv[1]);
      close_db();
    }
    return 0;
  }

  //remember to openDB and closeDB


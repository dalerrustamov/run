# RunTransactions
RunTransactons simulates transactions being processed on bank accounts. There is always a chance that more than one thread will want to update an account at the same time. In that case, ATMs that are being used by the same account at the same time by different users might not get the most up-to-date account information(balance), which can cause banks financial loss. So the program uses critical sections to avoid that issue, and makes other threads wait until the account is ready to be reused without giving users any errors.

NOTE: Every file but source.c are binary files. So leave them in the same folder as source so the program can read and write them. 

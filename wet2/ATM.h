//
// Created by Yamin on 12/12/2018.
//

#ifndef WET2_ATM_H
#define WET2_ATM_H

#include "Bank.h"
#include <iostream>

using namespace std;


class ATM {
public:
	ATM(int id,Bank *bank):bank_(bank),id_(id){}
	~ATM();

	void open_account(int id, char password[PASSWORD_LEN + 1], int init_balance);
	void make_VIP(int id, char password[PASSWORD_LEN + 1]);
	void deposit(int id, char password[PASSWORD_LEN + 1], int amount);
	void withdrawl(int id, char password[PASSWORD_LEN + 1], int amount);
	void check_balance(int id, char password[PASSWORD_LEN + 1]);
	void transfer_money(int id, char password[PASSWORD_LEN + 1], int target_id, int amount);
private:
	int id_;
	Bank *bank_;
};


#endif //WET2_ATM_H

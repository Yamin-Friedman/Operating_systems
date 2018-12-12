//
// Created by Yamin on 12/12/2018.
//

#ifndef WET2_BANK_H
#define WET2_BANK_H

#include <map>
#include "Account.h"

using namespace std;

class Bank {
public:
	Bank();
	~Bank();
	Account *get_account(int id){
		if(account_map.find(id) != account_map.end())
			return account_map[id];
		return NULL;
	}
	bool insert_account(Account *account);
private:
	map<int, Account*> account_map;
};


#endif //WET2_BANK_H

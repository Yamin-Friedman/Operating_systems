//
// Created by Yamin on 12/12/2018.
//

#ifndef WET2_ACCOUNT_H
#define WET2_ACCOUNT_H

#include <cstring>

#define PASSWORD_LEN 4


class Account {
public:
	Account(int id, char password[PASSWORD_LEN + 1], int balance, bool VIP): id_(id),balance_(balance),VIP_(VIP){strcpy(password_,password);}
	~Account();
	int get_ID() const {
		return id_;
	}
	int get_balance() const {
		return balance_;
	}
	bool check_password(char password[PASSWORD_LEN + 1]) const {
		if(strcmp(password,password_) == 0)
			return true;
		return false;
	}
	bool is_VIP() const {
		return VIP_;
	}
	void change_VIP_status(bool is_VIP){
		VIP_ = is_VIP;
	}
	Account &operator+=(int &rhs){
		balance_ += rhs;
		return *this;
	}

	Account &operator-=(int &rhs){
		if(rhs > balance_){
			throw rhs;
		}else{
			balance_ -= rhs;
		}
	}
private:
	int id_;
	char password_[PASSWORD_LEN + 1];
	int balance_;
	bool VIP_;
};


#endif //WET2_ACCOUNT_H

//
// Created by Yamin on 12/12/2018.
//

#include "ATM.h"

void ATM::open_account(int id, char password[PASSWORD_LEN + 1], int init_balance){
	if(bank_->get_account(id) != NULL){
		cout << "Error " << id_ << ": Your transaction failed – account with the same id exists" << endl;
		return;
	}
	Account *new_account = new Account(id,password,init_balance,false);
	bank_->insert_account(new_account);
	cout << id_ << ": New account id is " << id << " with password " << password << " and initial balance " << init_balance << endl;
}

void ATM::make_VIP(int id, char password[PASSWORD_LEN + 1]){
	Account *account = bank_->get_account(id);
	if(account == NULL || !account->check_password(password)){
		cout << "Error " << id_ << ": Your transaction failed – password for account id " << id << " is incorrect" << endl;
	}else{
		account->change_VIP_status(true);
	}
}

void ATM::deposit(int id, char password[PASSWORD_LEN + 1], int amount){
	Account *account = bank_->get_account(id);

	if(account == NULL){
		cout << "Error " << id_ << ": Your transaction failed – account id " << id <<" does not exist" << endl;
		return;
	}

	if(!account->check_password(password)){
		cout << "Error " << id_ << ": Your transaction failed – password for account id " << id << " is incorrect" << endl;
		return;
	}

	*account += amount;
	cout << id_ << ": Account " << id << " new balance is " << account->get_balance() << " after " << amount << " $ was deposited" << endl;
}

void ATM::withdrawl(int id, char password[PASSWORD_LEN + 1], int amount){
	Account *account = bank_->get_account(id);

	if(account == NULL){
		cout << "Error " << id_ << ": Your transaction failed – account id " << id <<" does not exist" << endl;
		return;
	}

	if(!account->check_password(password)){
		cout << "Error " << id_ << ": Your transaction failed – password for account id " << id << " is incorrect" << endl;
		return;
	}

	try{
		*account -= amount;
		cout << id_ << ": Account " << id << " new balance is " << account->get_balance() << " after " << amount << " $ was withdraw" << endl;
	}
	catch (int){
		cout << "Error " << id_ << ": Your transaction failed – account id " << id <<" balance is lower than " << amount << endl;
	}
}

void ATM::check_balance(int id, char password[PASSWORD_LEN + 1]){
	Account *account = bank_->get_account(id);

	if(account == NULL){
		cout << "Error " << id_ << ": Your transaction failed – account id " << id <<" does not exist" << endl;
		return;
	}

	if(!account->check_password(password)){
		cout << "Error " << id_ << ": Your transaction failed – password for account id " << id << " is incorrect" << endl;
		return;
	}

	cout << id_ << ": Account " << id << " balance is " << account->get_balance() << endl;
}

void ATM::transfer_money(int id, char password[PASSWORD_LEN + 1], int target_id, int amount){
	
}
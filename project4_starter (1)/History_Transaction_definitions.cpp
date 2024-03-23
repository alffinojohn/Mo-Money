#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
Transaction::Transaction(std::string ticker_symbol, unsigned int day_date, unsigned int month_date,
unsigned year_date, bool buy_sell_trans, unsigned int number_shares, double
trans_amount){
  p_next = nullptr;
  symbol = ticker_symbol;
  day = day_date;
  month = month_date;
  year = year_date;
  shares = number_shares;
  amount = trans_amount;

  if (buy_sell_trans){
    trans_type = "Buy";
  }
  else {
    trans_type = "Sell";
  }

  trans_id = assigned_trans_id++;
}

// Destructor
Transaction::~Transaction(){
}


// Overloaded < operator.
bool Transaction::operator<(Transaction const &other){
  if (year < other.year) {
          return true;
      } else if (year == other.year) {
          if (month < other.month) {
              return true;
          } else if (month == other.month) {
              if (day < other.day) {
                  return true;
              } else if (day == other.day) {
                  if (trans_id < other.trans_id){
                    return true;
                  }
              }
          }
    }
  return false;
}

// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//


// Constructor
History::History(){
  p_head = nullptr;
}


// Destructor
History::~History(){
  Transaction *current = p_head;
  Transaction *temp = nullptr;
  while (current != nullptr) {
    temp = current->get_next();
    delete current;
    current = temp;
  }
}


// insert(...): Insert transaction into linked list.
void History::insert(Transaction *p_new_trans){
  if (p_head == nullptr) {
        p_head = p_new_trans;
      } else {
          Transaction *current = p_head;
          while (current->get_next() != nullptr) {
              current = current->get_next();
          }
          current->set_next(p_new_trans);
      }
}

// read_history(...): Read the transaction history from file.
void History::read_history(){
  ece150::open_file();
  while(ece150::next_trans_entry()){
    std::string symbol = ece150::get_trans_symbol();
    unsigned int day = ece150::get_trans_day();
    unsigned int month = ece150::get_trans_month();
    unsigned int year = ece150::get_trans_year();
    unsigned int shares = ece150::get_trans_shares();
    double amount = ece150::get_trans_amount();
    bool type = ece150::get_trans_type();
    Transaction *new_transaction = new Transaction(symbol, day, month, year, type, shares, amount);
    insert(new_transaction);
  }
  ece150::close_file();
}


// print() Print the transaction history.
void History::print(){
  Transaction *current = p_head;
  std::cout << "========== BEGIN TRANSACTION HISTORY ============" << std::endl;
    while (current != nullptr) {
        current->print(); 
        current = current->get_next();
    }
  std::cout << "========== END TRANSACTION HISTORY ============" << std::endl;
}


// sort_by_date(): Sort the linked list by trade date.
void History::sort_by_date(){
    if (p_head == nullptr || p_head->get_next() == nullptr) {
        // Nothing to sort if the list is empty or has only one element
        return;
    }

    // Initialize the sorted list as empty
    Transaction* sortedList = nullptr;

    while (p_head != nullptr) {
        // Remove the first transaction from the unsorted list
        Transaction* currentTransaction = p_head;
        p_head = p_head->get_next();
        currentTransaction->set_next(nullptr);

        // Insert the removed transaction into the sorted list
      if (sortedList == nullptr || *currentTransaction < *sortedList) {
          // If the sorted list is empty or the new transaction is less than the first transaction in the sorted list,
          // update the head of the sorted list
          currentTransaction->set_next(sortedList);
          sortedList = currentTransaction;
      } else {
          // Otherwise, find the correct position to insert the new transaction in the sorted list
          Transaction* current = sortedList;
          while (current->get_next() != nullptr && !(*currentTransaction < *current->get_next())) {
              current = current->get_next();
          }

          // Insert the new transaction into the sorted list
          currentTransaction->set_next(current->get_next());
          current->set_next(currentTransaction);
      }
      }

    // Update the head of the History object with the sorted list
    p_head = sortedList;
}



// update_acb_cgl(): Updates the ACB and CGL values.
void History::update_acb_cgl(){
if (p_head == nullptr || p_head->get_next() == nullptr) {
        // Nothing to update if the list is empty or has only one element
        return;
    }

    Transaction* current = p_head;
    Transaction* prev = nullptr;

    while (current != nullptr) {
        // Update ACB, Share Balance, ACB/Share, and CGL for each transaction
        if (current->get_trans_type() == true) { // Buy transaction
            // Update ACB on Buy
            current->set_acb((prev == nullptr) ? current->get_amount() : (prev->get_acb() + current->get_amount()));

            // Update Share Balance on Buy
            current->set_share_balance((prev == nullptr) ? current->get_shares() : (prev->get_share_balance() + current->get_shares()));

            // Update ACB/Share on Buy
            current->set_acb_per_share(current->get_acb() / current->get_share_balance());

            // No CGL on Buy
            current->set_cgl(0.0);
        } else { // Sell transaction
            // Update ACB on Sell
            current->set_acb(prev->get_acb() - (current->get_shares() * prev->get_acb_per_share()));

            // Update Share Balance on Sell
            current->set_share_balance(prev->get_share_balance() - current->get_shares());

            // Update ACB/Share on Sell
            current->set_acb_per_share(prev->get_acb_per_share());

            // Update CGL on Sell
            double cgl = current->get_amount() - (current->get_shares() * prev->get_acb_per_share());
            current->set_cgl(cgl);
        }

        // Move to the next transaction
        prev = current;
        current = current->get_next();
    }
}

// compute_cgl(): )Compute the ACB, and CGL.
double History::compute_cgl(unsigned int year){
double total_cgl = 0.0;

    Transaction* current = p_head;

    while (current != nullptr) {
        // Check if the transaction is in the specified year
        if (current->get_year() == year) {
            // Update CGL for the transaction
            if (current->get_trans_type() == false) { // Sell transaction
                total_cgl += current->get_cgl();
            }
        }

        // Move to the next transaction
        current = current->get_next();
    }

    return total_cgl;
}


// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { return p_head; }



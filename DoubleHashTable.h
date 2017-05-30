#ifndef DOUBLE_HASH_TABLE_H
#define DOUBLE_HASH_TABLE_H

/*****************************************
 * UW User ID:  anavid
 * Submitted for ECE 250
 * Semester of Submission:  (Fall) 2014
 *
 * By submitting this file, I affirm that
 * I am the author of all modifications to
 * the provided code.
 *****************************************/

#include "Exception.h"
#include "ece250.h"

enum state { EMPTY, OCCUPIED, DELETED };

template<typename T>
class DoubleHashTable {
	private:
		int count;
		int power;
		int array_size;
		T *array;
		state *array_state;

		int h1( T const & ) const; // first hash function
		int h2( T const & ) const; // second hash function

	public:
		DoubleHashTable( int = 5 );
		~DoubleHashTable();
		int size() const;
		int capacity() const;		
		bool empty() const;
		bool member( T const & ) const;
		T bin( int ) const;

		void print() const;

		void insert( T const & );
		bool remove( T const & );
		void clear();
};

//constructor initializes the variables and arrays. It also accepts the integer m, whereas the size of the hashtable would be 2^m
template<typename T >
DoubleHashTable<T >::DoubleHashTable( int m ):
//initializes the variables and arrays to be used in the methods
count( 0 ), power( m ),
array_size( 1 << power ),
array( new T [array_size] ),
array_state( new state[array_size] ) {

	for ( int i = 0; i < array_size; ++i ) {
		array_state[i] = EMPTY;
	}
}

//destroctor deletes the memory allocated to the arrays 
template<typename T >
DoubleHashTable<T >::~DoubleHashTable() {
	delete[] array;
	delete[] array_state;
}

//returns the amount of elements currently in the hashtable
template<typename T >
int DoubleHashTable<T >::size() const { 
	return count;
}

//returns the total capacity of the hashtable
template<typename T >
int DoubleHashTable<T >::capacity() const { 
	return array_size;
}


//Checks if the hashtable is empty or not
template<typename T >
bool DoubleHashTable<T >::empty() const {
	if (count == 0){
		return true;
	}
	else{
		return false;
	}
}

//primary hash function method. Takes in the value to be inserted as input
//and outputs the position it should be inserted in following the primary hash function
template<typename T >
int DoubleHashTable<T >::h1( T const &obj ) const {
	int a = (static_cast<int> (obj)) % array_size;
	if (a < 0){
		a = a + array_size;
	}

	return a;
}

//Secondary hash function method. Takes in the value to be inserted
//and outputs the length of the jump calculated using secondary hash function in case of a collision
template<typename T >
int DoubleHashTable<T >::h2( T const &obj ) const {
	int a = ((static_cast<int> (obj)) / array_size) % array_size;
	if (a < 0){
		a = a + array_size;
	}
	if ((a % 2) == 0){
		a++;
	}

	return a;
}

//Checks if the value inputted is in the hashtable or not
template<typename T >
bool DoubleHashTable<T >::member( T const &obj ) const {
	int pos = h1(obj);
	if (array_state[pos] == EMPTY){
		return false;
	}
	else if (array[pos] == obj && array_state[pos] == OCCUPIED){
		return true;
	}
	else{
		int secondaryFunctionValue = h2(obj);
		int i = 0;
		while (i < array_size){
			pos = (pos + secondaryFunctionValue) % array_size;
			if (array_state[pos] == EMPTY){
				return false;
			}
			else if (array[pos] == obj && array_state[pos] == OCCUPIED){
				return true;
			}
			i++;
		}
	}

	return false;
}

//Returns the value at the position in the hashtable specified by the input
template<typename T >
T DoubleHashTable<T >::bin( int n ) const {              
	return array[n];
}

//Inserts the inputted value into the hash table. Uses primary function at first. If there is a collision, uses secondary hash function in order to 
//jump through the hash table and find an empty spot for the value
template<typename T >
void DoubleHashTable<T >::insert( T const &obj ) {
	//If hashtable is full, throws overflow error
	if (size() == capacity()){
		throw overflow();
	}
	int pos = h1(obj);

	//if the position gotten through primary hash function is not OCCUPIED, insert the value at this position
	if (array_state[pos] != OCCUPIED){
		array[pos] = obj;
		array_state[pos] = OCCUPIED;
		count++;
	}

	//If there is collision, use secondary hash function to find a value and jump through the hastable accordingly
	else{
		int secondaryFunctionValue = h2(obj);
		while (array_state[pos] == OCCUPIED){
			pos = (pos + secondaryFunctionValue) % array_size;
		}
		array[pos] = obj;
		array_state[pos] = OCCUPIED;
		count++;
	}

}

//Removes the inputted value from the hash table. If the value is not found, returns false. First, checks the initial position gotten using primary hash function.
//Next, if the value is not there, use secondary hash function to find a value in order to go through the hash table searching the value.
//When the value is found, the state of the postion of the value is set to DELETED and the method returns true
template<typename T >
bool DoubleHashTable<T >::remove( T const &obj ) { 	
	
	//Throw underflow error if there are no elements in the hashtable
	if (size() == 0){
		return false;
	}
	int pos = h1(obj);

	//If the position given by primary hash function is EMPTY, it proves that the value is not in the hashtable. So, return false
	if (array_state[pos] == EMPTY){
		return false;
	}

	//If the value is found, set the state of the position of the value to DELETED and return true
	else if (array[pos] == obj && array_state[pos] == OCCUPIED){
		array_state[pos] = DELETED;
		count--;
		return true;
	}

	//if there are other value occupying the position given by the primary hash function, use the secondary hash function to find a value in order 
	//to jump through the hashtable searching for the value. When the value is found, the state of the position of value is set to DELETED and return true.
	//If value is not found, return false
	else{
		int secondaryFunctionValue = h2(obj);
		int i = 0;
		while (i < array_size){
			pos = (pos + secondaryFunctionValue) % array_size;
			if (array_state[pos] == EMPTY){
				return false;
			}
			else if (array[pos] == obj && array_state[pos] == OCCUPIED){
				array_state[pos] = DELETED;
				count--;
				return true;
			}
			i++;
		}
	}
	return false;
}

//Clears the hashtable and deletes all the values
template<typename T >
void DoubleHashTable<T >::clear() {
	for (int i = 0; i < array_size-1; i++) {
		array_state[i] = EMPTY;
	}
	count = 0;
	return ; 
}

//Prints the values in the hashtable at all positions in the hashtable and also prints the state of the position (EMPTY, OCCUPIED, or DELETED). 
//Also prints the size and capacity
template<typename T >
void DoubleHashTable<T >::print() const{ 
	for (int i = 0; i < array_size; i++){
		std::cout << "\n Position" << i << "   Value:" << array[i] << "   State:" << array_state[i];
	}
	std::cout << "\n Size =" << size() << '\n';
}

#endif

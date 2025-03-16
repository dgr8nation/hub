/*
 * Khash.h
 *
 * Hash table implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

/*
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * The MIT License
 Copyright (c) 2008, 2009, 2011 by Attractive Chaos <attractor@live.co.uk>
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef WH_BASE_DS_KHASH_H_
#define WH_BASE_DS_KHASH_H_
#include "functors.h"
#include "Twiddler.h"
#include "../common/Memory.h"

namespace wanhive {
/**
 * Hash table of POD (plain old data) types.
 * @note C++ adaption of khash version 0.2.8 by Attractive Chaos
 * @ref https://github.com/attractivechaos/klib/blob/master/khash.h
 * @tparam KEY key's type
 * @tparam VALUE value's type
 * @tparam ISMAP true for hash map, false for hash set
 * @tparam HFN hash functor (returns unsigned int hash value of a key)
 * @tparam EQFN equality functor (returns true on equal keys, false otherwise)
 */
template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> class Khash: private NonCopyable {
public:
	/**
	 * Default constructor: creates an empty hash table.
	 */
	Khash() noexcept;
	/**
	 * Destructor
	 */
	~Khash();
	//-----------------------------------------------------------------
	/**
	 * Hash-map/hash-set operation: checks if the given key exists in the
	 * hash table.
	 * @param key key to search for
	 * @return true if the key exists, false otherwise
	 */
	bool contains(KEY const &key) const noexcept;
	/**
	 * Hash-map/hash-set operation: removes the given key from the hash table.
	 * @param key key for removal
	 * @return true if the key existed, false otherwise
	 */
	bool removeKey(KEY const &key) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Hash-map operation: reads the value associated with the given key.
	 * @param key key to search for
	 * @param val object to store the value associated with the given key
	 * @return true on success (key found), false otherwise (key doesn't exist)
	 */
	bool hmGet(KEY const &key, VALUE &val) const noexcept;
	/**
	 * Hash-map operation: inserts a new key-value pair in the hash table.
	 * @param key key to insert
	 * @param val value to associate with the given key
	 * @return true on success, false on failure (the key already exists)
	 */
	bool hmPut(KEY const &key, VALUE const &val) noexcept;
	/**
	 * Hash-map operation: stores a key-value pair in the hash table. If the key
	 * already exists then assigns the desired value to the existing key and
	 * returns the old value.
	 * @param key key to insert or update
	 * @param val value to assign to the given key
	 * @param oldVal object for returning the old value
	 * @return true if old value was replaced (key exists), false otherwise
	 */
	bool hmReplace(KEY const &key, VALUE const &val, VALUE &oldVal) noexcept;
	/**
	 * Hash-map operation: swaps values associated with the given pair of keys.
	 * If only one of the two keys exists in the hash table then the existing
	 * key is removed and it's value gets assigned to the non-existing key. If
	 * both the keys exist and swapping is allowed then the values associated
	 * with the given keys are exchanged.
	 * @param first first key
	 * @param second second key
	 * @param iterators stores iterators associated with the updated keys in
	 * their given order. The values remain valid until a hash table update.
	 * @param swap true to enable swapping, false otherwise
	 * @return true on success, false on failure (neither of the two keys exists
	 * or both the keys exist and swapping is disabled).
	 */
	bool hmSwap(KEY const &first, KEY const &second,
			unsigned int (&iterators)[2], bool swap) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Hash-set operation: inserts a new key into the hash table.
	 * @param key key to add
	 * @return true on success, false otherwise (key already exists)
	 */
	bool hsPut(KEY const &key) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Resizes the hash table.
	 * @param newCapacity new capacity
	 * @return always 0
	 */
	int resize(unsigned int newCapacity) noexcept;
	/**
	 * Returns iterator to a given key.
	 * @param key key to search for
	 * @return iterator to the found element, or Khash::end() if not found
	 */
	unsigned int get(KEY const &key) const noexcept;
	/**
	 * Inserts a key into the hash table.
	 * @param key key to insert
	 * @param ret stores an extra return code: 0 if the key is already
	 * present, 1 if the bucket was empty and not deleted, 2 if the bucket
	 * was deleted previously.
	 * @return iterator to the inserted element
	 */
	unsigned int put(KEY const &key, int &ret) noexcept;
	/**
	 * Removes a key from the hash table.
	 * @param x key's iterator (see Khash::get())
	 * @param shrink true to shrink the hash table if it is sparsely populated,
	 * false otherwise. Shrinking invalidates the iterators.
	 */
	void remove(unsigned int x, bool shrink = true) noexcept;
	/**
	 * Iterates a callback function over the hash table. The Callback function
	 * must return zero (0) to continue iterating, 1 to remove the key at it's
	 * current position, and any other value to stop the iteration.
	 * @param fn callback function. It's first argument is an iterator to
	 * the next item and its second argument is a generic pointer.
	 * @param arg second argument of the callback function
	 */
	void iterate(int (&fn)(unsigned int index, void *arg), void *arg);
	/**
	 * Returns hash table's capacity.
	 * @return number of buckets
	 */
	unsigned int capacity() const noexcept;
	/**
	 * Returns the total number of filled buckets in the hash table.
	 * @return number of existing keys
	 */
	unsigned int size() const noexcept;
	/**
	 * Returns the number of occupied buckets in the hash table. A bucket is
	 * occupied if it is either filled or deleted.
	 * @return number of occupied buckets
	 */
	unsigned int occupied() const noexcept;
	/**
	 * Returns the maximum number of buckets which can be occupied at the
	 * current capacity.
	 * @return maximum permissible number of occupied buckets
	 */
	unsigned int upperBound() const noexcept;
	/**
	 * Checks if the bucket at a given index is filled, i.e. it is neither
	 * empty nor deleted.
	 * @param x index to check
	 * @return true if the bucket is filled, false if the bucket is either
	 * empty or deleted.
	 */
	bool exists(unsigned int x) const noexcept;
	/**
	 * Returns the key present in the bucket at a given index.
	 * @param x the index
	 * @param key object for storing the key
	 * @return true on success, false otherwise (invalid index)
	 */
	bool getKey(unsigned int x, KEY &key) const noexcept;
	/**
	 * Returns the value present in the bucket at a given index.
	 * @param x the index
	 * @param value object for storing the value
	 * @return true on success, false otherwise (invalid index)
	 */
	bool getValue(unsigned int x, VALUE &value) const noexcept;
	/**
	 * Updates value stored at a given index.
	 * @param x the index
	 * @param value new value
	 * @return true on success, false otherwise (invalid index)
	 */
	bool setValue(unsigned int x, VALUE const &value) noexcept;
	/**
	 * Returns pointer to the stored value at a given index.
	 * @param x the index
	 * @return stored value's pointer, nullptr if the index is invalid
	 */
	VALUE* getValueReference(unsigned int x) const noexcept;
	/**
	 * Returns the start iterator that determines the inclusive lower bound.
	 * @return start iterator
	 */
	unsigned int begin() const noexcept;
	/**
	 * Returns the end iterator that determines the exclusive upper bound.
	 * @return end iterator
	 */
	unsigned int end() const noexcept;
	/**
	 * Empties the hash table (doesn't deallocate memory).
	 */
	void clear() noexcept;
private:
	//Delete the key-value buffer
	void deleteContainer() noexcept {
		if constexpr (ISMAP) {
			Memory<Pair>::free(bucket.entries);
			bucket.entries = nullptr;
		} else {
			Memory<KEY>::free(bucket.keys);
			bucket.keys = nullptr;
		}
	}
	//Resize the  key-value buffer
	void resizeContainer(unsigned int size) noexcept {
		if constexpr (ISMAP) {
			Memory<Pair>::resize(bucket.entries, size);
		} else {
			Memory<KEY>::resize(bucket.keys, size);
		}
	}

	//Update the capacity
	void capacity(unsigned int capacity) noexcept {
		bucket.capacity = capacity;
	}
	//Update the size (number of items in the hash table)
	void size(unsigned int size) noexcept {
		bucket.size = size;
	}
	//Update the occupied slots count (size + deleted)
	void occupied(unsigned int occupied) noexcept {
		bucket.occupied = occupied;
	}
	//Update the upper bound
	void upperBound(unsigned int upperBound) noexcept {
		bucket.upperBound = upperBound;
	}

	//Get the iterator's key
	const KEY& getKey(unsigned int x) const noexcept {
		if constexpr (ISMAP) {
			return bucket.entries[x].key;
		} else {
			return bucket.keys[x];
		}
	}

	//Set the iterator's key
	void setKey(unsigned int x, KEY const &key) noexcept {
		if constexpr (ISMAP) {
			bucket.entries[x].key = key;
		} else {
			bucket.keys[x] = key;
		}
	}

	//Get the iterator's value
	const VALUE& getValue(unsigned int x) const noexcept {
		return (bucket.entries[x].value);
	}

	//Get the iterator's value as lvalue
	VALUE& valueAt(unsigned int x) noexcept {
		return (bucket.entries[x].value);
	}

	//Get the flags buffer
	uint32_t* getFlags() const noexcept {
		return (bucket.flags);
	}

	//Set a new flags buffer (frees the existing one)
	void setFlags(uint32_t *flags) noexcept {
		Memory<uint32_t>::free(bucket.flags);
		bucket.flags = flags;
	}

	//Reset the flags (set default value)
	void resetFlags() noexcept {
		resetFlags(bucket.flags, bucket.capacity);
	}

	//Delete the flags buffer
	void deleteFlags() noexcept {
		Memory<uint32_t>::free(bucket.flags);
		bucket.flags = nullptr;
	}

	//Create a new flags buffer for the given capacity
	static uint32_t* createFlags(unsigned int entries) noexcept {
		auto flags = Memory<uint32_t>::allocate(fSize(entries));
		resetFlags(flags, entries);
		return flags;
	}

	//Reset the flags (set default value)
	static void resetFlags(uint32_t *flags, unsigned int entries) noexcept {
		if (flags) {
			memset(flags, 0xaa, fSize(entries) * sizeof(uint32_t));
		}
	}

	//Check whether the bucket at <i> is empty
	static bool isEmpty(const uint32_t *flag, unsigned int i) noexcept {
		return ((flag[i >> 4] >> ((i & 0xfU) << 1)) & 2);
	}
	//Check whether the value in the bucket at <i> has been deleted
	static bool isDeleted(const uint32_t *flag, unsigned int i) noexcept {
		return ((flag[i >> 4] >> ((i & 0xfU) << 1)) & 1);
	}
	//Check whether bucket at <i> is either empty or deleted
	static bool isEither(const uint32_t *flag, unsigned int i) noexcept {
		return ((flag[i >> 4] >> ((i & 0xfU) << 1)) & 3);
	}
	//Set deleted bit=false for bucket at <i>
	static void setIsdeletedFalse(uint32_t *flag, unsigned int i) noexcept {
		(flag[i >> 4] &= ~(1ul << ((i & 0xfU) << 1)));
	}
	//set empty bit=false for the bucket at <i>
	static void setIsemptyFalse(uint32_t *flag, unsigned int i) noexcept {
		(flag[i >> 4] &= ~(2ul << ((i & 0xfU) << 1)));
	}
	//Set both deleted and empty bits=false for the bucket at <i>
	static void setIsbothFalse(uint32_t *flag, unsigned int i) noexcept {
		(flag[i >> 4] &= ~(3ul << ((i & 0xfU) << 1)));
	}
	//Set deleted bit=true for the bucket at <i>
	static void setIsdeletedTrue(uint32_t *flag, unsigned int i) noexcept {
		(flag[i >> 4] |= 1ul << ((i & 0xfU) << 1));
	}
	//Calculate the number of 32-bit slots (@2 bits per entry)
	static unsigned int fSize(unsigned int entries) noexcept {
		return ((entries + 15) >> 4);
	}

	//Linear probe
	static unsigned int linearProbe(const unsigned int index,
			const unsigned int step, const unsigned int mask) noexcept {
		return (index + step) & mask;
	}
	//Calculate the upper bound for given capacity
	static unsigned int calculateUpperBound(unsigned int capacity) noexcept {
		return (unsigned int) ((capacity * LOAD_FACTOR) + 0.5);
	}
private:
	struct Pair {
		KEY key;
		VALUE value;
	};

	struct {
		unsigned int capacity;	//Total number of slots, always power of two
		unsigned int size;			//Total number of filled up slots
		unsigned int occupied;//Total number of occupied slots (size + deleted)
		unsigned int upperBound;	//Upper Limit on occupied slots
		uint32_t *flags;	//Status of each slot, need to specify data width
		Pair *entries; //Used by HASH-MAP
		KEY *keys; //Used by HASH-SET
	} bucket;

	HFN hash; //Hash functor (must return unsigned int)
	EQFN equal; //Equality functor

	//Minimum designated capacity of the hash table
	static constexpr unsigned int MIN_CAPACITY = 16;
	//The load factor
	static constexpr double LOAD_FACTOR = 0.77;
	//KEY must be POD
	WH_POD_ASSERT(KEY);
	//VALUE must be POD
	WH_POD_ASSERT(VALUE);
};

/**
 * Hash map specialization
 */
template<typename KEY, typename VALUE, typename HFN = wh_hash_fn,
		typename EQFN = wh_eq_fn> using Kmap = Khash<KEY, VALUE, true, HFN, EQFN>;
/**
 * Hash set specialization
 */
template<typename KEY, typename HFN = wh_hash_fn, typename EQFN = wh_eq_fn> using Kset = Khash<KEY, char, false, HFN, EQFN>;

} /* namespace wanhive */

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::Khash() noexcept {
	memset(&bucket, 0, sizeof(bucket));
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::~Khash() {
	deleteContainer();
	deleteFlags();
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::contains(
		const KEY &key) const noexcept {
	auto i = get(key);
	return (i != end());
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::removeKey(
		const KEY &key) noexcept {
	auto i = get(key);
	if (i != end()) {
		remove(i);
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::hmGet(const KEY &key,
		VALUE &val) const noexcept {
	auto i = get(key);
	if (i != end()) {
		val = getValue(i);
		return true;
	}
	return false;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::hmPut(const KEY &key,
		const VALUE &val) noexcept {
	if constexpr (ISMAP) {
		int ret;
		auto i = put(key, ret);
		if (ret) {
			valueAt(i) = val;
			return true;
		}
	}
	return false;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::hmReplace(const KEY &key,
		const VALUE &val, VALUE &oldVal) noexcept {
	if constexpr (ISMAP) {
		int ret;
		auto i = put(key, ret);
		if (ret) {
			//Key did not exist
			valueAt(i) = val;
			return false;
		} else {
			oldVal = getValue(i);
			valueAt(i) = val;
			return true;
		}
	}
	return false;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::hmSwap(KEY const &first,
		KEY const &second, unsigned int (&iterators)[2], bool swap) noexcept {
	if constexpr (!ISMAP) {
		iterators[0] = end();
		iterators[1] = end();
		return false;
	}

	auto fi = get(first);
	auto si = (first != second) ? get(second) : fi;
	//Correct iterators are returned even on failure
	iterators[0] = fi;
	iterators[1] = si;

	if (fi == si) {
		return exists(fi);
	} else if (exists(fi) && exists(si) && swap) {
		VALUE fv = getValue(fi);
		VALUE sv = getValue(si);
		setValue(fi, sv);
		setValue(si, fv);
		return true;
	} else if (exists(fi) && !exists(si)) {
		int x;
		VALUE fv = getValue(fi);
		remove(fi);
		si = put(second, x);
		setValue(si, fv);
		iterators[0] = end();
		iterators[1] = si;
		return true;
	} else if (!exists(fi) && exists(si)) {
		int x;
		VALUE sv = getValue(si);
		remove(si);
		fi = put(first, x);
		setValue(fi, sv);
		iterators[0] = fi;
		iterators[1] = end();
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::hsPut(
		const KEY &key) noexcept {
	if constexpr (!ISMAP) {
		int ret;
		put(key, ret);
		if (ret) {
			return true;
		}
	}
	return false;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::resize(
		unsigned int newCapacity) noexcept {
	uint32_t *newFlags = nullptr;
	auto rehash = true;
	{
		newCapacity = Twiddler::power2Ceil(newCapacity);
		if (newCapacity < MIN_CAPACITY) {
			newCapacity = MIN_CAPACITY;
		}
		if (size() >= calculateUpperBound(newCapacity)) {
			/* requested size is too small */
			rehash = false;
		} else {
			/* hash table size to be changed (shrink or expand); rehash */
			newFlags = createFlags(newCapacity);

			/*
			 * If the requested size is bigger then expand
			 * otherwise shrink (inside rehash block)
			 */
			if (capacity() < newCapacity) {
				/* expand */
				resizeContainer(newCapacity);
			}
		}
	}

	/* rehashing is needed */
	if (rehash) {
		for (unsigned int j = 0; j != capacity(); ++j) {
			if (!isEither(getFlags(), j)) {
				KEY key = getKey(j);
				VALUE val;
				auto newMask = newCapacity - 1;
				if constexpr (ISMAP) {
					val = valueAt(j);
				}
				setIsdeletedTrue(getFlags(), j);

				/*
				 * The kick-out process; sort of like in *Cuckoo Hashing*
				 * If the slot is free then just insert the item
				 * If the slot is occupied then kick out the occupant to
				 * accommodate the item. The kicked-out item will be inserted
				 * back in the next iteration in similar fashion.
				 */
				while (true) {
					auto i = newMask & (unsigned int) hash(key);
					unsigned int step = 0;
					while (!isEmpty(newFlags, i)) {
						//Quadratic Probe
						i = linearProbe(i, (++step), newMask);
					}
					setIsemptyFalse(newFlags, i);
					if (i < capacity() && !isEither(getFlags(), i)) {
						/* kick out the existing element */
						{
							KEY tmp = getKey(i);
							setKey(i, key);
							key = tmp;
						}
						if constexpr (ISMAP) {
							VALUE tmp = getValue(i);
							valueAt(i) = val;
							val = tmp;
						}
						/* mark it as deleted in the old hash table */
						setIsdeletedTrue(getFlags(), i);
					} else {
						/* write the element and jump out of the loop */
						setKey(i, key);
						if constexpr (ISMAP) {
							valueAt(i) = val;
						}
						break;
					}
				}
			}
		}
		if (capacity() > newCapacity) {
			/* shrink the hash table */
			resizeContainer(newCapacity);
		}
		setFlags(newFlags);
		capacity(newCapacity);
		occupied(size());
		upperBound(calculateUpperBound(capacity()));
	}
	return 0;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
unsigned int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::get(
		const KEY &key) const noexcept {
	if (capacity()) {
		auto mask = capacity() - 1;
		auto index = mask & (unsigned int) hash(key);
		auto last = index;	//the initial position
		unsigned int step = 0;
		while (!isEmpty(getFlags(), index)
				&& (isDeleted(getFlags(), index) || !equal(getKey(index), key))) {
			//Quadratic Probe
			index = linearProbe(index, (++step), mask);
			if (index == last) {
				return end();
			}
		}
		return isEither(getFlags(), index) ? end() : index;
	} else {
		return 0;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
unsigned int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::put(const KEY &key,
		int &ret) noexcept {
	if (occupied() >= upperBound()) { /* update the hash table */
		if (capacity() > (size() << 1)) {
			resize(capacity() - 1); /* clear "deleted" elements */
		} else {
			resize(capacity() + 1); /* expand the hash table */
		}
	}

	unsigned int index;
	{
		auto mask = capacity() - 1;
		auto i = mask & (unsigned int) hash(key);
		if (isEmpty(getFlags(), i)) {
			index = i; /* for speed up */
		} else {
			auto start = i;			//save the initial position
			auto step = 0;			//counter for quadratic probing
			auto site = end();		//the last deleted slot
			index = end();
			while (!isEmpty(getFlags(), i)
					&& (isDeleted(getFlags(), i) || !equal(getKey(i), key))) {
				if (isDeleted(getFlags(), i)) {
					//Record the last deleted slot and probe ahead
					site = i;
				}
				//Quadratic Probe
				i = linearProbe(i, (++step), mask);
				if (i == start) {
					//We went full circle, save the last deleted slot and break
					index = site;
					break;
				}
			}
			if (index == end()) {
				if (isEmpty(getFlags(), i) && site != end()) {
					//We hit an empty slot succeeding a number of deleted ones
					index = site;
				} else {
					//Key already present
					index = i;
				}
			}
		}
	}

	if (isEmpty(getFlags(), index)) { //Not present
		setKey(index, key);
		setIsbothFalse(getFlags(), index);
		size(size() + 1);
		occupied(occupied() + 1);
		ret = 1;
	} else if (isDeleted(getFlags(), index)) { //Deleted
		setKey(index, key);
		setIsbothFalse(getFlags(), index);
		size(size() + 1);
		ret = 2;
	} else { //Present and not deleted
		ret = 0;
	}
	return index;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
void wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::remove(unsigned int x,
		bool shrink) noexcept {
	if (exists(x)) {
		setIsdeletedTrue(getFlags(), x);
		size(size() - 1);
	}

	//If the hash table has become too sparse then fix it
	if (shrink && (size() > 4096) && (size() < (capacity() >> 2))) {
		Khash::resize((size() / LOAD_FACTOR) * 1.5);
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
void wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::iterate(
		int (&fn)(unsigned int index, void *arg), void *arg) {
	for (auto k = begin(); k < end(); ++k) {
		if (!exists(k)) {
			continue;
		}

		int ret = fn(k, arg);
		if (ret == 0) {
			continue;
		} else if (ret == 1) { //Remove the key
			remove(k, false); //Shrinking will invalidate the iterators
		} else { //Stop iterating
			break;
		}
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
unsigned int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::capacity() const noexcept {
	return (bucket.capacity);
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
unsigned int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::size() const noexcept {
	return (bucket.size);
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
unsigned int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::occupied() const noexcept {
	return (bucket.occupied);
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
unsigned int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::upperBound() const noexcept {
	return (bucket.upperBound);
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::exists(
		unsigned int x) const noexcept {
	return (x < end() && !isEither(bucket.flags, (x)));
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::getKey(unsigned int x,
		KEY &key) const noexcept {
	if (exists(x)) {
		key = ISMAP ? bucket.entries[x].key : bucket.keys[x];
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::getValue(unsigned int x,
		VALUE &value) const noexcept {
	if (ISMAP && exists(x)) {
		value = bucket.entries[x].value;
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::setValue(unsigned int x,
		VALUE const &value) noexcept {
	if (ISMAP && exists(x)) {
		bucket.entries[x].value = value;
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
VALUE* wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::getValueReference(
		unsigned int x) const noexcept {
	if (ISMAP && exists(x)) {
		return &(bucket.entries[x].value);
	} else {
		return nullptr;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
unsigned int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::begin() const noexcept {
	return 0;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
unsigned int wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::end() const noexcept {
	return capacity();
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
void wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::clear() noexcept {
	resetFlags();
	size(0);		//No elements in the hash table
	occupied(0);	//No deleted slots in the hash table
}

#endif /* WH_BASE_DS_KHASH_H_ */

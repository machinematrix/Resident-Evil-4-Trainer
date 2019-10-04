#ifndef __BIMAP__

#define __BIMAP__
#include <map>
#include <functional>

template <typename KTY, typename ValTy>
class Bimap
{
public:
	typedef KTY KeyType;
	typedef ValTy ValueType;
private:
	static bool comparator(const ValueType*, const ValueType*);
	std::map<KeyType, ValueType> map;
	std::map<const ValueType*, const KeyType*, std::function<decltype(comparator)>> reverseMap;

	void fillReverseMap();
public:
	typedef typename decltype(map)::iterator Iterator;
	typedef typename decltype(map)::const_iterator ConstIterator;
	typedef typename decltype(map)::size_type SizeType;
	Bimap();
	Bimap(std::initializer_list<typename decltype(map)::value_type> il); //functions as default constructor
	Bimap(const Bimap &other);
	Bimap(Bimap &&) = default;

	Bimap& operator=(const Bimap &other);
	Bimap& operator=(Bimap &&) = default;
	ValueType& at(const KeyType &key);
	const ValueType& at(const KeyType &key) const;
	const KeyType& at(const ValueType &val) const;
	SizeType size() const;
	bool empty() const;

	ValueType& getValue(const KeyType &key); //throws std::out_of_range
	const ValueType& getValue(const KeyType &key) const; //throws std::out_of_range
	const KeyType& getKey(const ValueType &value) const; //throws std::out_of_range

	std::pair<Iterator, bool> insert(const typename decltype(map)::value_type &val);
	SizeType erase(const KeyType &key);

	Iterator begin();
	ConstIterator begin() const;
	Iterator end();
	ConstIterator end() const;
	Iterator find(const KeyType &key);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename KeyType, typename ValueType>
bool Bimap<KeyType, ValueType>::comparator(const ValueType *lhs, const ValueType *rhs)
{
	return *lhs < *rhs;
}

template<typename KeyType, typename ValueType>
void Bimap<KeyType, ValueType>::fillReverseMap()
{
	reverseMap.clear();
	for (const auto &keyValue : map) {
		reverseMap.emplace(&keyValue.second, &keyValue.first);
	}
}

template<typename KeyType, typename ValueType>
Bimap<KeyType, ValueType>::Bimap()
	:reverseMap(comparator)
{}

template<typename KeyType, typename ValueType>
Bimap<KeyType, ValueType>::Bimap(std::initializer_list<typename decltype(map)::value_type> il)
	:map(il),
	 reverseMap(comparator)
{
	fillReverseMap();
}

template<typename KeyType, typename ValueType>
Bimap<KeyType, ValueType>::Bimap(const Bimap &other)
	:map(other.map),
	 reverseMap(comparator)
{
	fillReverseMap();
}

template<typename KeyType, typename ValueType>
Bimap<KeyType, ValueType>& Bimap<KeyType, ValueType>::operator=(const Bimap &other)
{
	map = other.map;
	fillReverseMap();
	return *this;
}

template<typename KeyType, typename ValueType>
ValueType& Bimap<KeyType, ValueType>::at(const KeyType &key)
{
	return map.at(key);
}

template<typename KeyType, typename ValueType>
const ValueType& Bimap<KeyType, ValueType>::at(const KeyType &key) const
{
	return map.at(key);
}

template<typename KeyType, typename ValueType>
const KeyType& Bimap<KeyType, ValueType>::at(const ValueType &val) const
{
	return *reverseMap.at(&val);
}

template<typename KTY, typename ValTy>
inline typename Bimap<KTY, ValTy>::SizeType Bimap<KTY, ValTy>::size() const
{
	return map.size();
}

template<typename KTY, typename ValTy>
inline bool Bimap<KTY, ValTy>::empty() const
{
	return map.empty();
}

template<typename KeyType, typename ValueType>
ValueType& Bimap<KeyType, ValueType>::getValue(const KeyType &key)
{
	return map.at(key);
}

template<typename KeyType, typename ValueType>
const ValueType& Bimap<KeyType, ValueType>::getValue(const KeyType &key) const
{
	return map.at(key);
}

template<typename KeyType, typename ValueType>
const KeyType& Bimap<KeyType, ValueType>::getKey(const ValueType &value) const
{
	return *reverseMap.at(&value);
}

template<typename KeyType, typename ValueType>
std::pair<typename Bimap<KeyType, ValueType>::Iterator, bool> Bimap<KeyType, ValueType>::insert(const typename decltype(map)::value_type &val)
{
	std::pair<typename Bimap::Iterator, bool> insertResult = map.insert(val);
	if (insertResult.second) { //if the key isn't in the map
		reverseMap.emplace(&insertResult.first->second, &insertResult.first->first);
	}
	return insertResult;
}

template<typename KeyType, typename ValueType>
typename Bimap<KeyType, ValueType>::SizeType Bimap<KeyType, ValueType>::erase(const KeyType &key)
{
	SizeType result = 1;
	try {
		typename decltype(map)::mapped_type &toErase = map.at(key);
		reverseMap.erase(&toErase);
		map.erase(key);
	}
	catch (const std::out_of_range&) {
		result = 0;
	}
	return result;
}

template<typename KTY, typename ValTy>
inline typename Bimap<KTY, ValTy>::Iterator Bimap<KTY, ValTy>::begin()
{
	return map.begin();
}

template<typename KTY, typename ValTy>
inline typename Bimap<KTY, ValTy>::ConstIterator Bimap<KTY, ValTy>::begin() const
{
	return map.cbegin();
}

template<typename KTY, typename ValTy>
inline typename Bimap<KTY, ValTy>::Iterator Bimap<KTY, ValTy>::end()
{
	return map.end();
}

template<typename KTY, typename ValTy>
inline typename Bimap<KTY, ValTy>::ConstIterator Bimap<KTY, ValTy>::end() const
{
	return map.cend();
}

template<typename KTY, typename ValTy>
inline typename Bimap<KTY, ValTy>::Iterator Bimap<KTY, ValTy>::find(const KeyType &key)
{
	return map.find(key);
}

#endif
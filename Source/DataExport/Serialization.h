#pragma once

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include "TFreeList.h"

class IArchive
{
protected:
	const char* _head;
	const char* _tail;
	char* _index;

public:
	IArchive(const char* head, const char* tail, char* index)
		: _head(head), _tail(tail), _index(index)
	{}

	char* GetBuf()
	{
		return const_cast<char*>(_head);
	}

	std::size_t GetSize() const
	{
		return _index - _head;
	}

	std::size_t GetRemain() const
	{
		return _tail - _index;
	}
};

// save
template<int N>
class Archive : public IArchive//, public ObjectPool<Archive<N>, 1024>
{
protected:
	char _buf[N];

public:
	Archive()
		: IArchive(_buf, _buf + N, _buf)
	{}
	~Archive() {}

private:
	bool IsOverflow(std::size_t size) const
	{
		return GetRemain() < size;
	}

	template<typename T>
	void Write(const T* buf, std::size_t  size)
	{
		if (IsOverflow(size))
			return;

		memcpy(_index, buf, size);
		_index += size;
	}

	// POD 포인터
	template<typename T>
	void Write(const T& value, std::true_type, std::true_type)
	{
		typedef std::remove_const_t<std::remove_pointer_t<T>> type;
		static_assert(!std::is_same<char, type>::value, "can't use char*");
		static_assert(!std::is_same<wchar_t, type>::value, "can't use wchar_t*");

		static_assert(sizeof(std::remove_pointer_t<T>) < N, "buffer size < write size");

		bool isNull = (nullptr == value);
		Write(&isNull, sizeof(isNull));

		if (nullptr == value)
			return;

		Write(value, sizeof(value));
	}

	// POD
	template<typename T>
	void Write(const T& value, std::true_type, std::false_type)
	{
		static_assert(sizeof(value) < N, "buffer size < write size");

		Write(&value, sizeof(value));
	}

	// POD 아닌 포인터
	template<typename T>
	void Write(const T& value, std::false_type, std::true_type)
	{
		bool isNull = (nullptr == value);
		Write(&isNull, sizeof(isNull));

		if (nullptr == value)
			return;

		value->Serialize(*this);
	}

	// 사용자 정의형
	template<typename T>
	void Write(const T& value, std::false_type, std::false_type)
	{
		value.Serialize(*this);	// 사용자 정의형일 경우 Serialize를 정의해야됨
	}

public:

	void Clear() { _head = _buf; _tail = _buf + N; _index = _buf; }

	template<typename T>
	Archive& operator &(const T& value)
	{
		return operator <<(value);
	}

	template<typename T>
	Archive& operator <<(const T& value)
	{
		Write(value, std::is_pod<std::remove_pointer_t<T>>(), std::is_pointer<T>());
		return *this;
	}

	template<>
	Archive& operator <<(const std::string& value)
	{
		*this << value.size();
		Write(value.c_str(), value.size());
		return *this;
	}

	template<>
	Archive& operator <<(const std::wstring& value)
	{
		*this << value.size();
		Write(value.c_str(), value.size() * sizeof(wchar_t));
		return *this;
	}

	template<typename T>
	void Write(const T& c)
	{
		*this << c.size();

		for (auto iter = c.begin(); iter != c.end(); ++iter)
		{
			*this << *iter;
		}
	}

	Archive& operator <<(void*)
	{
		return *this;
	}

	template<typename T>
	Archive& operator <<(const std::vector<T>& value)
	{
		Write(value);
		return *this;
	}

	template<typename T>
	Archive& operator <<(const std::list<T>& value)
	{
		Write(value);
		return *this;
	}

	template<typename T>
	Archive& operator <<(const std::set<T>& value)
	{
		Write(value);
		return *this;
	}

	template<typename K, typename V>
	Archive& operator <<(const std::map<K, V>& c)
	{
		*this << c.size();

		for (typename std::map<K, V>::const_iterator it = c.begin(); it != c.end(); ++it)
		{
			*this << it->first << it->second;
		}

		return *this;
	}

	template<typename K, typename V>
	Archive& operator <<(const std::unordered_map<K, V>& c)
	{
		*this << c.size();

		for (typename std::unordered_map<K, V>::const_iterator it = c.begin(); it != c.end(); ++it)
		{
			*this << it->first << it->second;
		}

		return *this;
	}

	template<typename K, typename V>
	Archive& operator <<(const std::multimap<K, V>& c)
	{
		*this << c.size();

		for (typename std::multimap<K, V>::const_iterator it = c.begin(); it != c.end(); ++it)
		{
			*this << it->first << it->second;
		}

		return *this;
	}
};

// load
//////////////////////////////////////////
//  |    Size     |      Remain     |   //
// head===========|================tail //
//              index                   //
//////////////////////////////////////////
template<>
class Archive<0> : public IArchive, public ObjectPool<Archive<0>, 1024>
{
public:
	Archive(const char * buf, std::size_t size)
		: IArchive(buf, buf + size, const_cast<char*>(buf))
	{}

private:
	bool IsUnderflow(std::size_t size)
	{
		return GetRemain() < size;
	}

	template<typename T>
	void Read(T* buf, std::size_t size)
	{
		if (IsUnderflow(size))
			return;

		memcpy(buf, _index, size);
		_index += size;
	}

	template<typename T>
	void Read(const T& c)
	{
		std::size_t size = ReadSize(c);
		c.reserve(size);

		for (std::size_t i = 0; i < size; ++i)
		{
			T value;
			*this >> value;
			c.emplace_back(value);
		}

		return *this;
	}

	template<typename T>
	typename T::size_type ReadSize(const T&)
	{
		std::size_t size;
		*this >> size;

		if (IsUnderflow(size))
			return 0;

		return size;
	}

	// POD 포인터
	template<typename T>
	void Read(T& value, std::true_type, std::true_type)
	{
		typedef std::remove_const_t<std::remove_pointer_t<T>> type;
		static_assert(!std::is_same<char, type>::value, "can't use char*");
		static_assert(!std::is_same<wchar_t, type>::value, "can't use wchar_t*");

		bool isNull;
		Read(&isNull, sizeof(isNull));
		if (isNull)
		{
			value = nullptr;
		}
		else
		{
			value = new std::remove_pointer_t<T>;
			Read(value, sizeof(std::remove_pointer_t<T>));
		}
	}

	// POD
	template<typename T>
	void Read(T& value, std::true_type, std::false_type)
	{
		Read(&value, sizeof(value));
	}

	// POD 아닌 포인터
	template<typename T>
	void Read(T& value, std::false_type, std::true_type)
	{
		typedef std::remove_const_t<std::remove_pointer_t<T>> type;
		static_assert(!std::is_pod<type>::value, "can't use point");

		bool isNull;
		Read(&isNull, sizeof(isNull));

		if (isNull)
		{
			value = nullptr;
		}
		else
		{
			value = new std::remove_pointer_t<T>;
			value->Serialize(*this);
		}
	}

	// 사용자 정의
	template<typename T>
	void Read(T& value, std::false_type, std::false_type)
	{
		value.Serialize(*this);
	}

public:
	template<typename T>
	Archive& operator &(T& value)
	{
		return operator >> (value);
	}

	Archive& operator >> (void*)
	{
		return *this;
	}

	template<typename T>
	Archive& operator >> (T& value)
	{
		Read(value, std::is_pod<std::remove_pointer_t<T>>(), std::is_pointer<T>());
		return *this;
	}

	Archive& operator >> (std::string& value)
	{
		std::string::size_type size = ReadSize(value);

		value.assign(_index, size);
		_index += size;

		return *this;
	}

	Archive& operator >> (std::wstring& value)
	{
		std::wstring::size_type size = ReadSize(value);

		value.assign((wchar_t*)_index, size);
		_index += size * sizeof(wchar_t);

		return *this;
	}

	template<typename T>
	Archive& operator >> (std::vector<T>& c)
	{
		std::size_t size = ReadSize(c);
		c.reserve(size);

		for (std::size_t i = 0; i < size; ++i)
		{
			T value;
			*this >> value;
			c.emplace_back(value);
		}

		return *this;
	}

	template<typename T>
	Archive& operator >> (std::list<T>& c)
	{
		std::size_t size = ReadSize(c);

		for (std::size_t i = 0; i < size; ++i)
		{
			T value;
			*this >> value;
			c.emplace_back(value);
		}

		return *this;
	}

	template<typename T>
	Archive& operator >> (std::set<T>& c)
	{
		std::size_t size = ReadSize(c);

		for (std::size_t i = 0; i < size; ++i)
		{
			T value;
			*this >> value;
			c.emplace(value);
		}

		return *this;
	}

	template<typename K, typename V>
	Archive& operator >> (std::map<K, V>& c)
	{
		std::size_t size = ReadSize(c);

		for (std::size_t i = 0; i < size; ++i)
		{
			K key;
			V value;
			*this >> key >> value;
			c.emplace(std::make_pair(key, value));
		}

		return *this;
	}

	template<typename K, typename V>
	Archive& operator >> (std::unordered_map<K, V>& c)
	{
		std::size_t size = ReadSize(c);

		for (std::size_t i = 0; i < size; ++i)
		{
			K key;
			V value;
			*this >> key >> value;
			c.emplace(std::make_pair(key, value));
		}

		return *this;
	}

	template<typename K, typename V>
	Archive& operator >> (std::multimap<K, V>& c)
	{
		std::size_t size = ReadSize(c);

		for (std::size_t i = 0; i < size; ++i)
		{
			K key;
			V value;
			*this >> key >> value;
			c.emplace(std::make_pair(key, value));
		}

		return *this;
	}
};

#define SERIALIZE(arg)						\
template<std::size_t N>						\
inline void Serialize(Archive<N>& ar)		\
{											\
	ar & arg;								\
}											\
template<std::size_t N>						\
inline void Serialize(Archive<N>& ar) const	\
{											\
	ar & arg;								\
}

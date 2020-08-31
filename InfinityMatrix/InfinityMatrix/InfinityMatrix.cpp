// InfinityMatrix.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"

#include <cassert>
#include <iostream>

#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/signal.hpp>
#include <boost/bind.hpp>

template<typename ElementType>
// Элемент не предназначен для корректной работы после уничтожения матрицы
class Element
{
	boost::optional<ElementType>& _element;
	boost::signal<void()> _on_value_added;
	boost::signal<void()> _on_value_deleted;

public:
	Element(
		decltype(_element) element,
		std::function<void()> on_value_added,
		std::function<void()> on_value_deleted
	)
		: _element(element)
	{
		_on_value_added.connect(on_value_added);
		_on_value_deleted.connect(on_value_deleted);
	}
	Element(Element &&e) noexcept
		: _element(e._element)
		, _on_value_added(std::move(e._on_value_added))
		, _on_value_deleted(std::move(e._on_value_deleted))
	{
	}

	Element<ElementType>& operator=(ElementType const &val)
	{
		bool const were_empty = IsEmpty();
		_element = val;
		if (were_empty)
			_on_value_added();
		return *this;
	}
	Element<ElementType>& operator=(ElementType &&val)
	{
		bool const were_empty = IsEmpty();
		_element = val;
		if (were_empty)
			_on_value_added();
		return *this;
	}
	Element<ElementType>& operator=(boost::none_t none)
	{
		bool const were_not_empty = !IsEmpty();
		_element = none;
		if (were_not_empty)
			_on_value_deleted();
		return *this;
	}

	operator ElementType&()
	{
		assert(!IsEmpty());
		return *_element;
	}

	bool IsEmpty() const
	{
		return !(bool)_element;
	}
};

template<typename ElementType>
class IMatrix
{
public:
	virtual ~IMatrix() = default;
	virtual Element<ElementType> operator[](size_t id) = 0;
	virtual IMatrix<ElementType>& operator()(size_t id) = 0;
	virtual size_t size() = 0;
};


template<typename ElementType>
class Matrix
	: public IMatrix<ElementType>
{	
	using Dimensions = std::unordered_map<
		size_t,
		std::pair<
			boost::optional<ElementType>,
			std::unique_ptr<IMatrix<ElementType>>
		>
	>;
	Dimensions _dims;
	size_t _dims_size;

	boost::signal<void()> _on_value_added;
	boost::signal<void()> _on_value_deleted;
public:
	Matrix()
		: _dims_size{ 0 }
	{
	}
	Matrix(std::function<void()> on_value_added,
		std::function<void()> on_value_deleted
	)
		: _dims_size{ 0 }
	{
		_on_value_added.connect(on_value_added);
		_on_value_deleted.connect(on_value_deleted);
	}
	
	Element<ElementType> operator[](size_t id) override
	{
		return Element<ElementType>(
			_create_dim_if_required(id)->second.first,
			boost::bind(&Matrix::OnValueAdded, this),
			boost::bind(&Matrix::OnValueDeleted, this)
		);
	}
	IMatrix<ElementType>& operator()(size_t id) override
	{
		auto &dim = _create_dim_if_required(id)->second.second;
		return *( dim 
			? dim
			: dim = std::make_unique<Matrix>(
				boost::bind(&Matrix::OnValueAdded, this),
				boost::bind(&Matrix::OnValueDeleted, this)
			) 
		);
	}

	size_t size() override
	{
		return _dims_size;
	}
private:
	typename Dimensions::iterator _create_dim_if_required(size_t id)
	{
		auto element = _dims.find(id);
		if (element == _dims.end())
		{
			bool is_success = false;
			typename Dimensions::iterator iterator;
			std::tie(iterator, is_success) = _dims.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(id),
				std::forward_as_tuple(boost::none, nullptr)
			);

			if (!is_success)
				throw std::exception{};
			return iterator;
		}
		return element;
	}
	
	// виртуальность, чтобы если что прикрутить потокобезопасность
	virtual void OnValueAdded()
	{
		++_dims_size;
		_on_value_added();
	}
	virtual void OnValueDeleted()
	{
		--_dims_size;
		_on_value_deleted();
	}
};

int main()
{
	Matrix<int> matrix; // бесконечная матрица int заполнена значениями -1
	assert(matrix.size() == 0); // все ячейки свободны
	auto a = matrix(0)[0];
	assert(a.IsEmpty());
	assert(matrix.size() == 0);
	matrix(100)[100] = 314;
	assert(matrix(100)[100] == 314);
	assert(matrix.size() == 1);
	matrix(100)[100] = boost::none;
	assert(matrix.size() == 0);
}